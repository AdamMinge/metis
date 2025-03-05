#ifndef METIS_EXPORT_H
#define METIS_EXPORT_H

/* ------------------------------------ Qt ---------------------------------- */
#include "metis/config.h"
/* -------------------------------------------------------------------------- */

#if defined(LIB_METIS_EXPORTS)
#define LIB_METIS_API METIS_API_EXPORT
#else
#define LIB_METIS_API METIS_API_IMPORT
#endif

#endif  // METIS_EXPORT_H
