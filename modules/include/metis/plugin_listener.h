#ifndef METIS_PLUGIN_LISTENER_H
#define METIS_PLUGIN_LISTENER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QObject>
/* ----------------------------------- Local -------------------------------- */
#include "metis/export.h"
#include "metis/plugin_manager.h"
/* -------------------------------------------------------------------------- */

namespace metis {

template <typename OBJECT>
class LIB_METIS_API PluginListener {
 public:
  explicit PluginListener();
  virtual ~PluginListener();

 protected:
  virtual void addedObject(OBJECT *object);
  virtual void removedObject(OBJECT *object);

  void loadObjects();

  [[nodiscard]] QList<OBJECT *> getObjects() const;

 private:
  QMetaObject::Connection added_object_conn;
  QMetaObject::Connection removed_object_conn;
};

template <typename OBJECT>
PluginListener<OBJECT>::PluginListener() {
  added_object_conn = QObject::connect(
      &PluginManager::getInstance(), &PluginManager::objectAdded,
      [this](auto current_object) {
        if (auto object = qobject_cast<OBJECT *>(current_object); object)
          this->addedObject(object);
      });

  removed_object_conn = QObject::connect(
      &PluginManager::getInstance(), &PluginManager::objectRemoved,
      [this](auto current_object) {
        if (auto object = qobject_cast<OBJECT *>(current_object); object)
          this->removedObject(object);
      });
}

template <typename OBJECT>
PluginListener<OBJECT>::~PluginListener() {
  QObject::disconnect(added_object_conn);
  QObject::disconnect(removed_object_conn);
}

template <typename OBJECT>
QList<OBJECT *> PluginListener<OBJECT>::getObjects() const {
  return PluginManager::getInstance().getObjects<OBJECT>();
}

template <typename OBJECT>
void PluginListener<OBJECT>::addedObject(OBJECT *object) {}

template <typename OBJECT>
void PluginListener<OBJECT>::removedObject(OBJECT *object) {}

template <typename OBJECT>
void PluginListener<OBJECT>::loadObjects() {
  auto objects = getObjects();
  for (auto object : objects) addedObject(object);
}

}  // namespace metis

#endif  // METIS_PLUGIN_LISTENER_H
