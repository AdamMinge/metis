#ifndef MIGRATOR_H
#define MIGRATOR_H

/* ----------------------------------- Metis -------------------------------- */
#include <metis/migrator.h>
/* ----------------------------------- Local -------------------------------- */
#include "migrate_v1.0.0_to_v2.0.0/export.h"
/* -------------------------------------------------------------------------- */

namespace snap_v2_0_0
{
  class Snapshot;
}

namespace snap_v3_0_0
{
  class Snapshot;
}

class LIB_PLUGIN_API Migrator : public metis::Migrator
{
  Q_OBJECT

public:
  Migrator(QObject *parent = nullptr);
  ~Migrator() override;

  QVersionNumber fromVersion() const override;
  QVersionNumber toVersion() const override;

  QByteArray migrate(const QByteArray &from) const override;

private:
  snap_v3_0_0::Snapshot
  migrate(const snap_v2_0_0::Snapshot &snapshot_from) const;
};

class LIB_PLUGIN_API ReverseMigrator : public metis::Migrator
{
  Q_OBJECT

public:
  ReverseMigrator(QObject *parent = nullptr);
  ~ReverseMigrator() override;

  QVersionNumber fromVersion() const override;
  QVersionNumber toVersion() const override;

  QByteArray migrate(const QByteArray &from) const override;

private:
  snap_v2_0_0::Snapshot
  migrate(const snap_v3_0_0::Snapshot &snapshot_from) const;
};

#endif// MIGRATOR_H
