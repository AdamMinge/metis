/* ----------------------------------- Local -------------------------------- */
#include "metis/plugin_manager.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QCoreApplication>
#include <QDirIterator>
#include <QPluginLoader>
/* -------------------------------------------------------------------------- */

namespace metis {

std::unique_ptr<PluginManager> PluginManager::m_instance =
    std::unique_ptr<PluginManager>(nullptr);

PluginManager &PluginManager::getInstance() {
  if (!m_instance) m_instance.reset(new PluginManager);

  return *m_instance;
}

void PluginManager::deleteInstance() { m_instance.reset(nullptr); }

PluginManager::PluginManager() {
  m_default_plugins_paths << QCoreApplication::applicationDirPath() +
                                 QStringLiteral("/plugins");
}

PluginManager::~PluginManager() = default;

void PluginManager::setDefaultPluginsPaths(QStringList plugins_paths) {
  m_default_plugins_paths = std::move(plugins_paths);
}

QStringList PluginManager::getDefaultPluginsPaths() const {
  return m_default_plugins_paths;
}

void PluginManager::loadPlugins(const QStringList &plugins_paths) {
  m_plugins.clear();

  auto static_plugin_instances = QPluginLoader::staticInstances();
  for (auto instance : static_plugin_instances) {
    Plugin::create(instance);
  }

  const auto &paths =
      plugins_paths.isEmpty() ? m_default_plugins_paths : plugins_paths;
  for (const auto &path : paths) {
    auto plugin_iterator = QDirIterator(path, QDir::Files | QDir::Readable);
    while (plugin_iterator.hasNext()) {
      const auto plugin_file = plugin_iterator.next();
      if (!QLibrary::isLibrary(plugin_file)) continue;

      auto plugin = Plugin::create(plugin_file);
      m_plugins.push_back(std::move(plugin));
    }
  }
}

std::list<Plugin *> PluginManager::getPlugins() {
  auto plugins = std::list<Plugin *>{};
  std::transform(m_plugins.begin(), m_plugins.end(),
                 std::back_inserter(plugins),
                 [](auto &plugin) { return &plugin; });
  return plugins;
}

std::list<Plugin *> PluginManager::getStaticPlugins() {
  auto plugins = getPlugins();
  plugins.remove_if([](auto plugin) { return !plugin->isStatic(); });
  return plugins;
}

std::list<Plugin *> PluginManager::getDynamicPlugins() {
  auto plugins = getPlugins();
  plugins.remove_if([](auto plugin) { return !plugin->isDynamic(); });
  return plugins;
}

void PluginManager::addObject(QObject *object) {
  Q_ASSERT(object);

  m_objects.push_front(object);
  Q_EMIT objectAdded(object);
}

void PluginManager::removeObject(QObject *object) {
  Q_ASSERT(object);

  m_objects.removeOne(object);
  Q_EMIT objectRemoved(object);
}

}  // namespace metis
