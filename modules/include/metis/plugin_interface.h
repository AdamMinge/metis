#ifndef METIS_PLUGIN_INTERFACE_H
#define METIS_PLUGIN_INTERFACE_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QList>
#include <QtPlugin>
/* ----------------------------------- Local -------------------------------- */
#include "metis/export.h"
/* -------------------------------------------------------------------------- */

namespace metis {

class LIB_METIS_API PluginInterface : public QObject {
  Q_OBJECT

 public:
  explicit PluginInterface();
  ~PluginInterface() override;

  virtual void init();

 protected:
  void addObject(QObject *object);
  void removeObject(QObject *object);

 private:
  QList<QObject *> m_added_objects;
};

}  // namespace metis

Q_DECLARE_INTERFACE(metis::PluginInterface, "org.metis.PluginInterface")

#endif  // METIS_PLUGIN_INTERFACE_H
