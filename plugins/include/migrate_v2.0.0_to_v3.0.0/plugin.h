#ifndef PLUGIN_H
#define PLUGIN_H

/* ---------------------------------- Metis -------------------------------- */
#include <metis/plugin_interface.h>
/* -------------------------------------------------------------------------- */

class MigratePlugin : public metis::PluginInterface
{
  Q_OBJECT
  Q_INTERFACES(metis::PluginInterface)
  Q_PLUGIN_METADATA(IID "org.flow.PluginInterface" FILE "plugin.json")

public:
  explicit MigratePlugin();
  ~MigratePlugin() override;

  void init() override;
};

#endif// PLUGIN_H
