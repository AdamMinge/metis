#ifndef PLUGIN_H
#define PLUGIN_H

/* ---------------------------------- Metis -------------------------------- */
#include <metis/plugin_interface.h>
/* ----------------------------------- Local -------------------------------- */
#include "migrate_v2.0.0_to_v3.0.0/export.h"
/* -------------------------------------------------------------------------- */

class LIB_PLUGIN_API MigratePlugin : public metis::PluginInterface {
  Q_OBJECT
  Q_INTERFACES(metis::PluginInterface)
  Q_PLUGIN_METADATA(IID "org.flow.PluginInterface" FILE "plugin.json")

 public:
  explicit MigratePlugin();
  ~MigratePlugin() override;

  void init() override;
};

#endif  // PLUGIN_H
