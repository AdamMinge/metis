#ifndef DIFFER_H
#define DIFFER_H

/* ----------------------------------- Metis -------------------------------- */
#include <metis/differ.h>
/* ---------------------------------- Protobuf ------------------------------ */
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/reflection.h>
/* -------------------------------------------------------------------------- */

/* --------------------------------- ChangeType ----------------------------- */

enum class ChangeType
{
  Unchanged,
  Added,
  Removed,
  Modified
};

/* ---------------------------------- DiffNode ------------------------------ */

class DiffNode
{
public:
  explicit DiffNode(QString name, ChangeType change = ChangeType::Unchanged);
  virtual ~DiffNode();

  DiffNode(const DiffNode &) = delete;
  DiffNode &operator=(const DiffNode &) = delete;

  DiffNode(DiffNode &&) = default;
  DiffNode &operator=(DiffNode &&) = default;

  void addChild(std::unique_ptr<DiffNode> &&child);
  const DiffNode *getChild(int index) const;
  const DiffNode *getParent() const;

  int childCount() const;
  int row() const;

  void setChange(ChangeType change);
  ChangeType change() const;

  void setName(QString name);
  QString name() const;

protected:
  ChangeType m_change;
  QString m_name;
  DiffNode *m_parent;
  std::vector<std::unique_ptr<DiffNode>> m_children;
};

/* --------------------------------- ObjectNode ----------------------------- */

class ObjectNode : public DiffNode
{
public:
  explicit ObjectNode(QString name);
};

/* -------------------------------- PropertyNode ---------------------------- */

class PropertyNode : public DiffNode
{
public:
  explicit PropertyNode(QString name, QVariant oldValue, QVariant newValue);

  QVariant oldValue() const;
  QVariant newValue() const;

private:
  QVariant m_oldValue;
  QVariant m_newValue;
};

/* --------------------------------- DiffBuilder ---------------------------- */

class DiffBuilder
{
public:
  explicit DiffBuilder();
  ~DiffBuilder();

  std::unique_ptr<DiffNode> build(
    const google::protobuf::Message &msg1,
    const google::protobuf::Message &msg2) const;

private:
  [[nodiscard]] std::unique_ptr<DiffNode> createFromField(
    const google::protobuf::Message &msg1,
    const google::protobuf::Message &msg2,
    const google::protobuf::FieldDescriptor *field) const;
  [[nodiscard]] std::unique_ptr<DiffNode> createFromRepeatedField(
    const google::protobuf::Message &msg1,
    const google::protobuf::Message &msg2,
    const google::protobuf::FieldDescriptor *field) const;

  [[nodiscard]] QVariant getRepeatedValue(
    const google::protobuf::Message &msg,
    const google::protobuf::FieldDescriptor *field,
    const google::protobuf::Reflection *reflection, int index) const;
  [[nodiscard]] QVariant getValue(
    const google::protobuf::Message &msg,
    const google::protobuf::FieldDescriptor *field,
    const google::protobuf::Reflection *reflection) const;
};

/* ---------------------------------- DiffModel ----------------------------- */

class DiffModel : public QAbstractItemModel
{
  Q_OBJECT

  enum Column
  {
    Name,
    OldValue,
    NewValue,
    Count
  };

public:
  [[nodiscard]] static DiffModel *create(
    const google::protobuf::Message &src, const google::protobuf::Message &dest,
    QObject *parent = nullptr);

public:
  ~DiffModel() override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  QVariant data(const QModelIndex &index, int role) const override;

  QModelIndex parent(const QModelIndex &index) const override;
  QModelIndex index(
    int row, int column,
    const QModelIndex &parent = QModelIndex()) const override;

protected:
  explicit DiffModel(
    std::unique_ptr<DiffNode> &&root, QObject *parent = nullptr);

private:
  std::unique_ptr<DiffNode> m_root;
};

/* ----------------------------------- Differ ------------------------------- */

class Differ : public metis::Differ
{
  Q_OBJECT

public:
  Differ(QObject *parent = nullptr);
  ~Differ() override;

  QVersionNumber version() const override;

  QAbstractItemModel *diff(
    const QByteArray &src, const QByteArray &dest,
    QObject *parent = nullptr) const override;
};

#endif// DIFFER_H
