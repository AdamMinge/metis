#ifndef PLUGIN_H
#define PLUGIN_H

/* ---------------------------------- Metis -------------------------------- */
#include <metis/plugin_interface.h>
/* -------------------------------------------------------------------------- */

class DiffPlugin : public metis::PluginInterface
{
  Q_OBJECT
  Q_INTERFACES(metis::PluginInterface)
  Q_PLUGIN_METADATA(IID "org.metis.PluginInterface" FILE "plugin.json")

public:
  explicit DiffPlugin();
  ~DiffPlugin() override;

  void init() override;
};

#endif// PLUGIN_H
