#ifndef EXPORT_H
#define EXPORT_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QtCore/QtGlobal>
/* -------------------------------------------------------------------------- */

#if defined(DIFF_V1_0_0_EXPORTS)
#define LIB_PLUGIN_API Q_DECL_EXPORT
#else
#define LIB_PLUGIN_API Q_DECL_IMPORT
#endif

#endif  // EXPORT_H
