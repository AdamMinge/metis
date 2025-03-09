/* ----------------------------------- Local -------------------------------- */
#include "diff_v3.0.0/differ.h"

#include "snap_v3.0.0_proto/snap_v3.0.0.grpc.pb.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QColor>
/* ----------------------------------- Metis -------------------------------- */
#include <metis/logging_manager.h>
/* -------------------------------------------------------------------------- */

#undef GetMessage

namespace
{

  QVariant getValue(
    const google::protobuf::Message &msg,
    const google::protobuf::FieldDescriptor *field,
    const google::protobuf::Reflection *reflection)
  {
    switch (field->cpp_type())
    {
      case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
        return QString::fromStdString(reflection->GetString(msg, field));
        break;
      case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
        return reflection->GetInt32(msg, field);
        break;
      case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
        return qint64(reflection->GetInt64(msg, field));
        break;
      case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
        return reflection->GetUInt32(msg, field);
        break;
      case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
        return quint64(reflection->GetUInt64(msg, field));
        break;
      case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
        return reflection->GetBool(msg, field);
        break;
      case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
        return reflection->GetDouble(msg, field);
        break;
      case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
        return reflection->GetFloat(msg, field);
        break;
    }

    return QVariant{};
  }

  std::unique_ptr<DiffNode> createDiffNode(
    const google::protobuf::Message &msg1,
    const google::protobuf::Message &msg2)
  {
    const auto descriptor = msg1.GetDescriptor();
    const auto reflection1 = msg1.GetReflection();
    const auto reflection2 = msg2.GetReflection();
    Q_ASSERT(descriptor == msg2.GetDescriptor());

    auto object_node =
      std::make_unique<ObjectNode>(QString::fromStdString(descriptor->name()));
    object_node->setChange(ChangeType::Unchanged);

    for (int i = 0; i < descriptor->field_count(); ++i)
    {
      const auto field = descriptor->field(i);
      if (field->is_repeated()) continue;

      const auto has1 = reflection1->HasField(msg1, field);
      const auto has2 = reflection2->HasField(msg2, field);

      if (
        field->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE)
      {
        if (has1 && has2)
        {
          object_node->addChild(createDiffNode(
            reflection1->GetMessage(msg1, field),
            reflection2->GetMessage(msg2, field)));
        } else
        {
          const auto empty_msg =
            google::protobuf::MessageFactory::generated_factory()->GetPrototype(
              field->message_type());
          Q_ASSERT(empty_msg);

          if (has1)
          {
            auto removed_node =
              createDiffNode(reflection1->GetMessage(msg1, field), *empty_msg);
            removed_node->setChange(ChangeType::Removed);
            object_node->addChild(std::move(removed_node));
          } else if (has2)
          {
            auto added_node =
              createDiffNode(*empty_msg, reflection2->GetMessage(msg2, field));
            added_node->setChange(ChangeType::Added);
            object_node->addChild(std::move(added_node));
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

        object_node->addChild(std::move(property_node));
      }
    }

    return object_node;
  }

}// namespace

/* --------------------------------- DiffNode ------------------------------- */

DiffNode::DiffNode(ChangeType change) : m_change(change) {}

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

/* --------------------------------- ObjectNode ----------------------------- */

ObjectNode::ObjectNode(QString name) : DiffNode(), m_name(std::move(name)) {}

QString ObjectNode::name() const { return m_name; }

/* -------------------------------- PropertyNode ---------------------------- */

PropertyNode::PropertyNode(QString name, QVariant oldValue, QVariant newValue)
    : DiffNode(), m_name(std::move(name)), m_oldValue(std::move(oldValue)),
      m_newValue(std::move(newValue))
{}

QString PropertyNode::name() const { return m_name; }

QVariant PropertyNode::oldValue() const { return m_oldValue; }

QVariant PropertyNode::newValue() const { return m_newValue; }

/* --------------------------------- DiffModel ------------------------------ */

DiffModel *DiffModel::create(
  const google::protobuf::Message &src, const google::protobuf::Message &dest,
  QObject *parent)
{
  auto diff_node = createDiffNode(src, dest);
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

int DiffModel::columnCount(const QModelIndex &) const { return 3; }

QVariant DiffModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) return QVariant();

  auto node = static_cast<DiffNode *>(index.internalPointer());
  if (!node) return QVariant();

  if (role == Qt::DisplayRole)
  {
    if (auto object_node = dynamic_cast<const ObjectNode *>(node))
    {
      if (index.column() == 0) return object_node->name();
    } else if (auto property_node = dynamic_cast<const PropertyNode *>(node))
    {
      if (index.column() == 0) return property_node->name();
      if (index.column() == 1) return property_node->oldValue();
      if (index.column() == 2) return property_node->newValue();
    }
  } else if (role == Qt::BackgroundRole)
  {
    switch (node->change())
    {
      case ChangeType::Added:
        return QColor(Qt::green);
      case ChangeType::Removed:
        return QColor(Qt::red);
      case ChangeType::Modified:
        return QColor(Qt::yellow);
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

  return createIndex(parent_node->row(), 0, parent_node);
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
