/* ----------------------------------- Local -------------------------------- */
#include "migrate_v2.0.0_to_v3.0.0/plugin.h"
#include "migrate_v1.0.0_to_v2.0.0/migrator.h"
/* -------------------------------------------------------------------------- */

/* -------------------------------- MigratePlugin --------------------------- */

MigratePlugin::MigratePlugin() = default;

MigratePlugin::~MigratePlugin() = default;

void MigratePlugin::init()
{
  addObject(new Migrator(this));
  addObject(new ReverseMigrator(this));
}
