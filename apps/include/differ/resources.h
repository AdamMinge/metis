#ifndef RESOURCES_H
#define RESOURCES_H

/* ------------------------------------- Qt --------------------------------- */
#include <QLatin1String>
/* -------------------------------------------------------------------------- */

/* ----------------------------------- Icons -------------------------------- */

namespace icons {

#define _DEFINE_ICON(NAME, SIZE, ICON) \
  constexpr QLatin1String NAME{":/editor/images/" SIZE "/" ICON};

}  // namespace icons


#endif  // RESOURCES_H
