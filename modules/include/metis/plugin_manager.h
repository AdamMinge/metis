#ifndef METIS_PLUGIN_MANAGER_H
#define METIS_PLUGIN_MANAGER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QObject>
/* --------------------------------- Standard ------------------------------- */
#include <memory>
/* ----------------------------------- Local -------------------------------- */
#include "metis/export.h"
#include "metis/plugin.h"
/* -------------------------------------------------------------------------- */

namespace metis {

class LIB_METIS_API PluginManager : public QObject {
  Q_OBJECT

 public:
  [[nodiscard]] static PluginManager &getInstance();
  static void deleteInstance();

 public:
  ~PluginManager() override;

  void setDefaultPluginsPaths(QStringList plugins_paths);
  [[nodiscard]] QStringList getDefaultPluginsPaths() const;

  void loadPlugins(const QStringList &plugins_paths = {});
  [[nodiscard]] std::list<Plugin *> getPlugins();
  [[nodiscard]] std::list<Plugin *> getStaticPlugins();
  [[nodiscard]] std::list<Plugin *> getDynamicPlugins();

  void addObject(QObject *object);
  void removeObject(QObject *object);

  template <typename TYPE>
  void forEach(const std::function<void(TYPE *)> &function);
  template <typename TYPE>
  TYPE *forOne(const std::function<bool(TYPE *)> &function);
  template <typename TYPE>
  TYPE *findIf(const std::function<bool(const TYPE *)> &function);
  template <typename TYPE>
  [[nodiscard]] QList<TYPE *> getObjects();

 Q_SIGNALS:
  void objectAdded(QObject *object);
  void objectRemoved(QObject *object);

 protected:
  explicit PluginManager();

 private:
  static std::unique_ptr<PluginManager> m_instance;

  QObjectList m_objects;
  std::list<Plugin> m_plugins;
  QStringList m_default_plugins_paths;
};

template <typename TYPE>
void PluginManager::forEach(const std::function<void(TYPE *)> &function) {
  for (auto object : m_objects) {
    if (auto result = qobject_cast<TYPE *>(object); result) function(result);
  }
}

template <typename TYPE>
TYPE *PluginManager::forOne(const std::function<bool(TYPE *)> &function) {
  for (auto object : m_objects) {
    if (auto result = qobject_cast<TYPE *>(object); result) {
      if (function(result)) return result;
    }
  }

  return nullptr;
}

template <typename TYPE>
TYPE *PluginManager::findIf(const std::function<bool(const TYPE *)> &function) {
  for (auto object : m_objects) {
    if (auto result = qobject_cast<TYPE *>(object); result) {
      if (function(result)) return result;
    }
  }

  return nullptr;
}

template <typename TYPE>
QList<TYPE *> PluginManager::getObjects() {
  QList<TYPE *> objects{};
  for (auto object : m_objects) {
    if (auto result = qobject_cast<TYPE *>(object); result)
      objects.append(result);
  }

  return objects;
}

}  // namespace metis

#endif  // METIS_PLUGIN_MANAGER_H
