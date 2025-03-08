/* ----------------------------------- Local -------------------------------- */
#include "diff_v2.0.0/plugin.h"

#include "diff_v2.0.0/differ.h"
/* -------------------------------------------------------------------------- */

/* --------------------------------- DiffPlugin ----------------------------- */

DiffPlugin::DiffPlugin() = default;

DiffPlugin::~DiffPlugin() = default;

void DiffPlugin::init() { addObject(new Differ(this)); }
