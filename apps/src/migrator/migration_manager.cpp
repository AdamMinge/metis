/* ----------------------------------- Local -------------------------------- */
#include "migration_manager.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QHash>
#include <QQueue>
/* ---------------------------------- Metis -------------------------------- */
#include <metis/migrator.h>
#include <metis/plugin_manager.h>
/* -------------------------------------------------------------------------- */

/* ------------------------------ MigrationProcess -------------------------- */

MigrationManager::MigrationManager()
    : m_migrators(
        metis::PluginManager::getInstance().getObjects<metis::Migrator>())
{}

MigrationManager::~MigrationManager() = default;

QByteArray MigrationManager::migrate(
  const QByteArray &data, const QVersionNumber &from,
  const QVersionNumber &to) const
{
  auto migration_path = findPath(from, to);
  if (migration_path.isEmpty()) return {};

  auto source_data = QByteArray{};
  auto dest_data = data;
  for (auto migrator : migration_path)
  {
    std::swap(source_data, dest_data);
    dest_data = migrator->migrate(source_data);
    if (dest_data.isEmpty()) return {};
  }

  return dest_data;
}

QList<metis::Migrator *> MigrationManager::findPath(
  const QVersionNumber &from, const QVersionNumber &to) const
{
  auto to_process = QQueue<QVersionNumber>{};
  auto parent_map = QHash<QVersionNumber, metis::Migrator *>{};
  auto visited = QHash<QVersionNumber, bool>{};

  to_process.enqueue(from);
  visited[from] = true;

  while (!to_process.isEmpty())
  {
    const auto currentVersion = to_process.dequeue();
    if (currentVersion == to) { return reconstructPath(from, to, parent_map); }

    for (const auto &migrator : m_migrators)
    {
      if (migrator->fromVersion() == currentVersion)
      {
        const auto nextVersion = migrator->toVersion();
        if (!visited.contains(nextVersion))
        {
          to_process.enqueue(nextVersion);
          visited[nextVersion] = true;
          parent_map[nextVersion] = migrator;
        }
      }
    }
  }

  return {};
}

QList<metis::Migrator *> MigrationManager::reconstructPath(
  const QVersionNumber &from, const QVersionNumber &to,
  const QHash<QVersionNumber, metis::Migrator *> &parent_map) const
{
  auto path = QList<metis::Migrator *>{};

  auto currentVersion = to;
  while (parent_map.contains(currentVersion))
  {
    auto migrator = parent_map[currentVersion];
    path.prepend(migrator);
    currentVersion = migrator->fromVersion();
  }

  return path;
}