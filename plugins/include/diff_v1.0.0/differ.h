#ifndef DIFFER_H
#define DIFFER_H

/* ----------------------------------- Metis -------------------------------- */
#include <metis/differ.h>
/* ---------------------------------- Protobuf ------------------------------ */
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/reflection.h>
/* ----------------------------------- Local -------------------------------- */
#include "diff_v1.0.0/export.h"
/* -------------------------------------------------------------------------- */

enum class ChangeType
{
  Unchanged,
  Added,
  Removed,
  Modified
};

class DiffNode
{
public:
  explicit DiffNode(ChangeType change = ChangeType::Unchanged);
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

protected:
  ChangeType m_change;
  DiffNode *m_parent;
  std::vector<std::unique_ptr<DiffNode>> m_children;
};

class ObjectNode : public DiffNode
{
public:
  explicit ObjectNode(QString name);

  QString name() const;

private:
  QString m_name;
};

class PropertyNode : public DiffNode
{
public:
  explicit PropertyNode(QString name, QVariant oldValue, QVariant newValue);

  QString name() const;
  QVariant oldValue() const;
  QVariant newValue() const;

private:
  QString m_name;
  QVariant m_oldValue;
  QVariant m_newValue;
};

class LIB_PLUGIN_API DiffModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  [[nodiscard]] static DiffModel *create(
    const google::protobuf::Message &src, const google::protobuf::Message &dest,
    QObject *parent = nullptr);

public:
  ~DiffModel() override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

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

class LIB_PLUGIN_API Differ : public metis::Differ
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
