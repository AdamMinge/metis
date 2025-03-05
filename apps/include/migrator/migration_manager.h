#ifndef MIGRATION_PROCESS_H
#define MIGRATION_PROCESS_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QObject>
#include <QVersionNumber>
/* -------------------------------------------------------------------------- */

namespace metis
{
  class Migrator;
}

class MigrationManager
{
public:
  MigrationManager();
  ~MigrationManager();

  [[nodiscard]] QByteArray migrate(
    const QByteArray &data, const QVersionNumber &from,
    const QVersionNumber &to) const;

private:
  [[nodiscard]] QList<metis::Migrator *>
  findPath(const QVersionNumber &from, const QVersionNumber &to) const;

  [[nodiscard]] QList<metis::Migrator *> reconstructPath(
    const QVersionNumber &from, const QVersionNumber &to,
    const QHash<QVersionNumber, metis::Migrator *> &parent_map) const;

private:
  QList<metis::Migrator *> m_migrators;
};

#endif// MIGRATION_PROCESS_H
