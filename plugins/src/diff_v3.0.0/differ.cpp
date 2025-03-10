/* ----------------------------------- Local -------------------------------- */
#include "diff_v3.0.0/differ.h"

#include "snap_v3.0.0_proto/snap_v3.0.0.grpc.pb.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QAction>
#include <QColor>
#include <QFont>
/* ----------------------------------- Metis -------------------------------- */
#include <metis/logging_manager.h>
/* -------------------------------------------------------------------------- */

#undef GetMessage

/* --------------------------------- DiffNode ------------------------------- */

DiffNode::DiffNode(QString name, ChangeType change)
    : m_change(change), m_name(std::move(name))
{}

DiffNode::~DiffNode() = default;

void DiffNode::addChild(std::unique_ptr<DiffNode> &&child)
{
  child->m_parent = this;
  m_children.push_back(std::move(child));
}

const DiffNode *DiffNode::getChild(int index) const
{
  return (index >= 0 && index < static_cast<int>(m_children.size()))
           ? m_children[index].get()
           : nullptr;
}

const DiffNode *DiffNode::getParent() const { return m_parent; }

int DiffNode::childCount() const { return static_cast<int>(m_children.size()); }

int DiffNode::row() const
{
  auto parent = getParent();
  if (!parent) return 0;

  for (size_t i = 0; i < parent->m_children.size(); ++i)
  {
    if (parent->m_children[i].get() == this) return static_cast<int>(i);
  }
  return 0;
}

void DiffNode::setChange(ChangeType change) { m_change = change; }

ChangeType DiffNode::change() const { return m_change; }

void DiffNode::setName(QString name) { m_name = std::move(name); }

QString DiffNode::name() const { return m_name; }

/* --------------------------------- ObjectNode ----------------------------- */

ObjectNode::ObjectNode(QString name) : DiffNode(std::move(name)) {}

/* -------------------------------- PropertyNode ---------------------------- */

PropertyNode::PropertyNode(QString name, QVariant oldValue, QVariant newValue)
    : DiffNode(std::move(name)), m_oldValue(std::move(oldValue)),
      m_newValue(std::move(newValue))
{}

QVariant PropertyNode::oldValue() const { return m_oldValue; }

QVariant PropertyNode::newValue() const { return m_newValue; }

/* -------------------------------- DiffBuilder ----------------------------- */

DiffBuilder::DiffBuilder() = default;

DiffBuilder::~DiffBuilder() = default;

std::unique_ptr<DiffNode> DiffBuilder::build(
  const google::protobuf::Message &msg1,
  const google::protobuf::Message &msg2) const
{
  const auto descriptor = msg1.GetDescriptor();
  const auto reflection1 = msg1.GetReflection();
  const auto reflection2 = msg2.GetReflection();
  if (descriptor != msg2.GetDescriptor()) return nullptr;

  auto object_node =
    std::make_unique<ObjectNode>(QString::fromStdString(descriptor->name()));

  auto any_field_diff = false;
  for (auto i = 0; i < descriptor->field_count(); ++i)
  {
    const auto field = descriptor->field(i);
    auto new_diff_node = std::unique_ptr<DiffNode>();

    if (field->is_repeated())
      new_diff_node = createFromRepeatedField(msg1, msg2, field);
    else
      new_diff_node = createFromField(msg1, msg2, field);

    Q_ASSERT(new_diff_node);
    any_field_diff |= new_diff_node->change() != ChangeType::Unchanged;
    object_node->addChild(std::move(new_diff_node));
  }

  object_node->setChange(
    any_field_diff ? ChangeType::Modified : ChangeType::Unchanged);
  return object_node;
}

std::unique_ptr<DiffNode> DiffBuilder::createFromField(
  const google::protobuf::Message &msg1, const google::protobuf::Message &msg2,
  const google::protobuf::FieldDescriptor *field) const
{
  Q_ASSERT(!field->is_repeated());

  const auto reflection1 = msg1.GetReflection();
  const auto reflection2 = msg2.GetReflection();

  const auto has1 = reflection1->HasField(msg1, field);
  const auto has2 = reflection2->HasField(msg2, field);

  if (field->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE)
  {
    if (has1 && has2)
    {
      return build(
        reflection1->GetMessage(msg1, field),
        reflection2->GetMessage(msg2, field));
    } else
    {
      const auto empty_msg =
        google::protobuf::MessageFactory::generated_factory()->GetPrototype(
          field->message_type());
      Q_ASSERT(empty_msg);

      if (has1)
      {
        auto removed_node =
          build(reflection1->GetMessage(msg1, field), *empty_msg);
        removed_node->setChange(ChangeType::Removed);
        return removed_node;
      } else if (has2)
      {
        auto added_node =
          build(*empty_msg, reflection2->GetMessage(msg2, field));
        added_node->setChange(ChangeType::Added);
        return added_node;
      }
    }
  } else
  {
    auto value1 = has1 ? getValue(msg1, field, reflection1) : QVariant{};
    auto value2 = has2 ? getValue(msg2, field, reflection2) : QVariant{};

    auto property_node = std::make_unique<PropertyNode>(
      QString::fromStdString(field->name()), value1, value2);

    if (has1 && has2)
    {
      property_node->setChange(
        value1 == value2 ? ChangeType::Unchanged : ChangeType::Modified);
    } else if (has1)
    {
      property_node->setChange(ChangeType::Removed);
    } else if (has2)
    {
      property_node->setChange(ChangeType::Added);
    }

    return property_node;
  }

  return nullptr;
}


std::unique_ptr<DiffNode> DiffBuilder::createFromRepeatedField(
  const google::protobuf::Message &msg1, const google::protobuf::Message &msg2,
  const google::protobuf::FieldDescriptor *field) const
{
  Q_ASSERT(field->is_repeated());

  const auto reflection1 = msg1.GetReflection();
  const auto reflection2 = msg2.GetReflection();

  const auto size1 = reflection1->FieldSize(msg1, field);
  const auto size2 = reflection2->FieldSize(msg2, field);

  auto max_size = std::max(size1, size2);
  auto repeated_field_node =
    std::make_unique<ObjectNode>(QString::fromStdString(
      field->name() + "[" + std::to_string(0) + "-" + std::to_string(max_size) +
      "]"));

  auto any_field_diff = false;
  for (auto j = 0; j < max_size; ++j)
  {
    auto node = std::unique_ptr<DiffNode>();
    if (field->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE)
    {
      const auto empty_msg =
        google::protobuf::MessageFactory::generated_factory()->GetPrototype(
          field->message_type());
      Q_ASSERT(empty_msg);

      auto message1 = j < size1
                        ? &reflection1->GetRepeatedMessage(msg1, field, j)
                        : empty_msg;
      auto message2 = j < size2
                        ? &reflection2->GetRepeatedMessage(msg2, field, j)
                        : empty_msg;

      node = build(*message1, *message2);
      node->setName(
        QString::fromStdString(field->name() + "[" + std::to_string(j) + "]"));

    } else
    {
      QVariant value1, value2;
      if (j < size1) { value1 = getRepeatedValue(msg1, field, reflection1, j); }
      if (j < size2) { value2 = getRepeatedValue(msg2, field, reflection2, j); }

      node = std::make_unique<PropertyNode>(
        QString::fromStdString(field->name() + "[" + std::to_string(j) + "]"),
        value1, value2);

      if (j < size1 && j < size2)
      {
        node->setChange(
          value1 == value2 ? ChangeType::Unchanged : ChangeType::Modified);
      }
    }

    if (j >= size1)
    {
      node->setChange(ChangeType::Added);
    } else if (j >= size2)
    {
      node->setChange(ChangeType::Removed);
    }

    Q_ASSERT(node);
    any_field_diff |= node->change() != ChangeType::Unchanged;
    repeated_field_node->addChild(std::move(node));
  }

  repeated_field_node->setChange(
    any_field_diff ? ChangeType::Modified : ChangeType::Unchanged);
  return repeated_field_node;
}

QVariant DiffBuilder::getValue(
  const google::protobuf::Message &msg,
  const google::protobuf::FieldDescriptor *field,
  const google::protobuf::Reflection *reflection) const
{
  switch (field->cpp_type())
  {
    case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
      return QString::fromStdString(reflection->GetString(msg, field));
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
      return reflection->GetInt32(msg, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
      return qint64(reflection->GetInt64(msg, field));
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
      return reflection->GetUInt32(msg, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
      return quint64(reflection->GetUInt64(msg, field));
    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
      return reflection->GetBool(msg, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
      return reflection->GetDouble(msg, field);
    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
      return reflection->GetFloat(msg, field);
  }

  return QVariant{};
}

QVariant DiffBuilder::getRepeatedValue(
  const google::protobuf::Message &msg,
  const google::protobuf::FieldDescriptor *field,
  const google::protobuf::Reflection *reflection, int index) const
{
  const auto size = reflection->FieldSize(msg, field);
  if (index < 0 || index >= size) return QVariant{};

  switch (field->cpp_type())
  {
    case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
      return QString::fromStdString(
        reflection->GetRepeatedString(msg, field, index));
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
      return reflection->GetRepeatedInt32(msg, field, index);
    case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
      return qint64(reflection->GetRepeatedInt64(msg, field, index));
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
      return reflection->GetRepeatedUInt32(msg, field, index);
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
      return quint64(reflection->GetRepeatedUInt64(msg, field, index));
    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
      return reflection->GetRepeatedBool(msg, field, index);
    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
      return reflection->GetRepeatedDouble(msg, field, index);
    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
      return reflection->GetRepeatedFloat(msg, field, index);
  }

  return QVariant{};
}

/* --------------------------------- DiffModel ------------------------------ */

DiffModel *DiffModel::create(
  const google::protobuf::Message &src, const google::protobuf::Message &dest,
  QObject *parent)
{
  auto diff_builder = DiffBuilder{};
  auto diff_node = diff_builder.build(src, dest);
  Q_ASSERT(diff_node);

  return new DiffModel(std::move(diff_node), parent);
}

DiffModel::DiffModel(std::unique_ptr<DiffNode> &&root, QObject *parent)
    : QAbstractItemModel(parent), m_root(std::move(root))
{}

DiffModel::~DiffModel() = default;

int DiffModel::rowCount(const QModelIndex &parent) const
{
  const auto parent_node =
    !parent.isValid() ? m_root.get()
                      : static_cast<const DiffNode *>(parent.internalPointer());
  return parent_node ? parent_node->childCount() : 0;
}

int DiffModel::columnCount(const QModelIndex &) const { return Column::Count; }

QVariant
DiffModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch (section)
    {
      case Column::Name:
        return tr("Name");
      case Column::OldValue:
        return tr("Old Value");
      case Column::NewValue:
        return tr("New Value");
      default:
        return QVariant();
    }
  }
  return QVariant();
}

QVariant DiffModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) return QVariant();

  auto node = static_cast<DiffNode *>(index.internalPointer());
  if (!node) return QVariant();

  if (role == Qt::DisplayRole)
  {
    if (auto object_node = dynamic_cast<const ObjectNode *>(node))
    {
      if (index.column() == Column::Name) return object_node->name();
    } else if (auto property_node = dynamic_cast<const PropertyNode *>(node))
    {
      if (index.column() == Column::Name) return property_node->name();
      if (index.column() == Column::OldValue) return property_node->oldValue();
      if (index.column() == Column::NewValue) return property_node->newValue();
    }
  } else if (role == Qt::FontRole)
  {
    if (node->change() != ChangeType::Unchanged)
    {
      QFont font;
      font.setBold(true);
      return font;
    } else if (node->change() == ChangeType::Removed)
    {
      QFont font;
      font.setStrikeOut(true);
      return font;
    }
  } else if (role == Qt::DecorationRole && index.column() == 0)
  {
    switch (node->change())
    {
      case ChangeType::Added:
        return QIcon(":/diff_v3.0.0/icons/green.png");
      case ChangeType::Removed:
        return QIcon(":/diff_v3.0.0/icons/red.png");
      case ChangeType::Modified:
        return QIcon(":/diff_v3.0.0/icons/blue.png");
      default:
        return QVariant();
    }
  }

  return QVariant();
}

QModelIndex DiffModel::parent(const QModelIndex &index) const
{
  if (!index.isValid()) return QModelIndex();

  auto child_node = static_cast<const DiffNode *>(index.internalPointer());
  if (!child_node) return QModelIndex();

  auto parent_node = child_node->getParent();
  if (!parent_node) return QModelIndex();

  if (parent_node == m_root.get()) return QModelIndex();

  return createIndex(parent_node->row(), Column::Name, parent_node);
}

QModelIndex
DiffModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent)) return QModelIndex();

  const auto parent_node =
    !parent.isValid() ? m_root.get()
                      : static_cast<const DiffNode *>(parent.internalPointer());
  if (!parent_node) return QModelIndex();

  const auto child_node = parent_node->getChild(row);
  return child_node ? createIndex(row, column, child_node) : QModelIndex();
}

/* ------------------------------------ Differ ------------------------------ */

Differ::Differ(QObject *parent) : metis::Differ(parent) {}

Differ::~Differ() = default;

QVersionNumber Differ::version() const { return QVersionNumber(3, 0, 0); }

QAbstractItemModel *Differ::diff(
  const QByteArray &src, const QByteArray &dest, QObject *parent) const
{
  auto snapshot_src = snap_v3_0_0::Snapshot{};
  auto snapshot_dest = snap_v3_0_0::Snapshot{};

  if (!snapshot_src.ParseFromArray(src.data(), src.size()))
  {
    metis::LOG_ERROR(QString("Failed to parse Snapshot %1 from input data. The "
                             "data may be corrupted or incompatible.")
                       .arg(version().toString()));
    return nullptr;
  }

  if (!snapshot_dest.ParseFromArray(dest.data(), dest.size()))
  {
    metis::LOG_ERROR(QString("Failed to parse Snapshot %1 from input data. The "
                             "data may be corrupted or incompatible.")
                       .arg(version().toString()));
    return nullptr;
  }

  return DiffModel::create(snapshot_src, snapshot_dest, parent);
}
