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
  if (!snapshot_from.ParseFromArray(from.data(), from.size()))
  {
    metis::LOG_ERROR(QString("Failed to parse Snapshot %1 from input data. The "
                             "data may be corrupted or incompatible.")
                       .arg(fromVersion().toString()));
    return QByteArray{};
  }

  auto snapshot_to = migrate(snapshot_from);

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

snap_v2_0_0::Snapshot
Migrator::migrate(const snap_v1_0_0::Snapshot &snapshot_from) const
{
  auto snapshot_to = snap_v2_0_0::Snapshot{};

  const auto &settings_from = snapshot_from.settings();
  auto settings_to = snapshot_to.mutable_settings();
  settings_to->set_version(settings_from.version());
  settings_to->set_author(settings_from.author());
  settings_to->set_timestamp(settings_from.timestamp());

  for (const auto &cell_from : snapshot_from.cells())
  {
    auto *cell_to = snapshot_to.add_cells();
    cell_to->set_id(cell_from.id());
    cell_to->set_name(cell_from.name());
    cell_to->set_width(cell_from.width());
    cell_to->set_height(cell_from.height());
    cell_to->set_type(cell_from.type());
  }

  for (const auto &component_from : snapshot_from.components())
  {
    auto *component_to = snapshot_to.add_components();
    component_to->set_id(component_from.id());
    component_to->set_name(component_from.name());
    component_to->set_part(component_from.part());
    component_to->set_placement(component_from.placement());

    for (const auto &pin_from : component_from.pins())
    {
      auto *pin_to = component_to->add_pins();
      pin_to->set_id(pin_from.id());
      pin_to->set_name(pin_from.name());
      pin_to->set_function(pin_from.function());
    }
  }

  for (const auto &part_from : snapshot_from.parts())
  {
    auto *part_to = snapshot_to.add_parts();
    part_to->set_id(part_from.id());
    part_to->set_name(part_from.name());
    part_to->set_category(part_from.category());
    part_to->set_material(part_from.material());
    part_to->set_weight(part_from.weight());
  }

  for (const auto &layer_from : snapshot_from.layers())
  {
    auto *layer_to = snapshot_to.add_layers();
    layer_to->set_id(layer_from.id());
    layer_to->set_name(layer_from.name());
    layer_to->set_order(layer_from.order());
    layer_to->set_material(layer_from.material());
    layer_to->set_thickness(layer_from.thickness());
  }

  return snapshot_to;
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
  if (!snapshot_from.ParseFromArray(from.data(), from.size()))
  {
    metis::LOG_ERROR(QString("Failed to parse Snapshot %1 from input data. The "
                             "data may be corrupted or incompatible.")
                       .arg(fromVersion().toString()));
    return QByteArray{};
  }

  auto snapshot_to = migrate(snapshot_from);

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

snap_v1_0_0::Snapshot
ReverseMigrator::migrate(const snap_v2_0_0::Snapshot &snapshot_from) const
{
  auto snapshot_to = snap_v1_0_0::Snapshot{};

  const auto &settings_from = snapshot_from.settings();
  auto settings_to = snapshot_to.mutable_settings();
  settings_to->set_version(settings_from.version());
  settings_to->set_author(settings_from.author());
  settings_to->set_timestamp(settings_from.timestamp());

  for (const auto &cell_from : snapshot_from.cells())
  {
    auto *cell_to = snapshot_to.add_cells();
    cell_to->set_id(cell_from.id());
    cell_to->set_name(cell_from.name());
    cell_to->set_width(cell_from.width());
    cell_to->set_height(cell_from.height());
    cell_to->set_type(cell_from.type());
  }

  for (const auto &component_from : snapshot_from.components())
  {
    auto *component_to = snapshot_to.add_components();
    component_to->set_id(component_from.id());
    component_to->set_name(component_from.name());
    component_to->set_part(component_from.part());
    component_to->set_placement(component_from.placement());

    for (const auto &pin_from : component_from.pins())
    {
      auto *pin_to = component_to->add_pins();
      pin_to->set_id(pin_from.id());
      pin_to->set_name(pin_from.name());
      pin_to->set_function(pin_from.function());
    }
  }

  for (const auto &part_from : snapshot_from.parts())
  {
    auto *part_to = snapshot_to.add_parts();
    part_to->set_id(part_from.id());
    part_to->set_name(part_from.name());
    part_to->set_category(part_from.category());
    part_to->set_material(part_from.material());
    part_to->set_weight(part_from.weight());
  }

  for (const auto &layer_from : snapshot_from.layers())
  {
    auto *layer_to = snapshot_to.add_layers();
    layer_to->set_id(layer_from.id());
    layer_to->set_name(layer_from.name());
    layer_to->set_order(layer_from.order());
    layer_to->set_material(layer_from.material());
    layer_to->set_thickness(layer_from.thickness());
  }

  return snapshot_to;
}
