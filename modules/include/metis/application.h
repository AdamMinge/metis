#ifndef METIS_APPLICATION_H
#define METIS_APPLICATION_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QApplication>
/* ----------------------------------- Local -------------------------------- */
#include "metis/export.h"
/* -------------------------------------------------------------------------- */

namespace metis {

/* -------------------------------- Application ----------------------------- */

class LIB_METIS_API Application : public QApplication {
  Q_OBJECT

 private:
  struct Preferences;

 public:
  Application(int &argc, char **argv);
  ~Application() override;

  bool event(QEvent *event) override;

 private:
  void parseCommandLine();

  void writeSettings();
  void writePlugins();
  void writeShortcuts();

  void readSettings();
  void readPlugins();
  void readShortcuts();

 private:
  QScopedPointer<Preferences> m_preferences;
};

}  // namespace metis

#endif  // METIS_APPLICATION_H
