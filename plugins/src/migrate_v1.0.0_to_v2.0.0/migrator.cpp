/* ----------------------------------- Local -------------------------------- */
#include "migrate_v1.0.0_to_v2.0.0/migrator.h"

#include "snap_v1.0.0_proto/snap_v1.0.0.grpc.pb.h"
#include "snap_v2.0.0_proto/snap_v2.0.0.grpc.pb.h"
/* ----------------------------------- Metis -------------------------------- */
#include <metis/logging_manager.h>
/* -------------------------------------------------------------------------- */

/* ----------------------------------- Migrator ----------------------------- */

Migrator::Migrator(QObject *parent) : metis::Migrator(parent) {}

Migrator::~Migrator() = default;

QVersionNumber Migrator::fromVersion() const { return QVersionNumber(1, 0, 0); }

QVersionNumber Migrator::toVersion() const { return QVersionNumber(2, 0, 0); }

QByteArray Migrator::migrate(const QByteArray &from) const
{
  auto snapshot_from = snap_v1_0_0::Snapshot{};
  auto snapshot_to = snap_v2_0_0::Snapshot{};

  if (!snapshot_from.ParseFromArray(from.data(), from.size()))
  {
    metis::LOG_ERROR(QString("Failed to parse Snapshot %1 from input data. The "
                             "data may be corrupted or incompatible.")
                       .arg(fromVersion().toString()));
    return QByteArray{};
  }

  snapshot_to.set_name(snapshot_from.name());
  snapshot_to.set_nr(1);

  auto output_data = QByteArray{};
  output_data.resize(snapshot_to.ByteSizeLong());
  if (!snapshot_to.SerializeToArray(output_data.data(), output_data.size()))
  {
    metis::LOG_ERROR(
      QString("Failed to serialize Snapshot %1 to output data. The "
              "output buffer may not be large enough.")
        .arg(toVersion().toString()));
    return QByteArray{};
  }

  metis::LOG_INFO(
    "Step successful: " + fromVersion().toString() + " -> " +
    toVersion().toString());

  return output_data;
}

/* ------------------------------- ReverseMigrator -------------------------- */

ReverseMigrator::ReverseMigrator(QObject *parent) : metis::Migrator(parent) {}

ReverseMigrator::~ReverseMigrator() = default;

QVersionNumber ReverseMigrator::fromVersion() const
{
  return QVersionNumber(2, 0, 0);
}

QVersionNumber ReverseMigrator::toVersion() const
{
  return QVersionNumber(1, 0, 0);
}

QByteArray ReverseMigrator::migrate(const QByteArray &from) const
{
  auto snapshot_from = snap_v2_0_0::Snapshot{};
  auto snapshot_to = snap_v1_0_0::Snapshot{};

  if (!snapshot_from.ParseFromArray(from.data(), from.size()))
  {
    metis::LOG_ERROR(QString("Failed to parse Snapshot %1 from input data. The "
                             "data may be corrupted or incompatible.")
                       .arg(fromVersion().toString()));
    return QByteArray{};
  }

  snapshot_to.set_name(
    (QString("%1-%2").arg(snapshot_from.name().c_str()).arg(snapshot_from.nr()))
      .toStdString());

  auto output_data = QByteArray{};
  output_data.resize(snapshot_to.ByteSizeLong());
  if (!snapshot_to.SerializeToArray(output_data.data(), output_data.size()))
  {
    metis::LOG_ERROR(
      QString("Failed to serialize Snapshot %1 to output data. The "
              "output buffer may not be large enough.")
        .arg(toVersion().toString()));
    return QByteArray{};
  }

  metis::LOG_INFO(
    "Step successful: " + fromVersion().toString() + " -> " +
    toVersion().toString());

  return output_data;
}
