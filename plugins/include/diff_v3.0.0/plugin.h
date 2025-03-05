#ifndef PLUGIN_H
#define PLUGIN_H

/* ---------------------------------- Metis -------------------------------- */
#include <metis/plugin_interface.h>
/* ----------------------------------- Local -------------------------------- */
#include "diff_v3.0.0/export.h"
/* -------------------------------------------------------------------------- */

class LIB_PLUGIN_API DiffPlugin : public metis::PluginInterface {
  Q_OBJECT
  Q_INTERFACES(metis::PluginInterface)
  Q_PLUGIN_METADATA(IID "org.flow.PluginInterface" FILE "plugin.json")

 public:
  explicit DiffPlugin();
  ~DiffPlugin() override;

  void init() override;
};

#endif  // PLUGIN_H
