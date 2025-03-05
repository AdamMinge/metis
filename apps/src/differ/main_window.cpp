/* ----------------------------------- Local -------------------------------- */
#include "main_window.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QCloseEvent>
#include <QMessageBox>
/* ---------------------------------- Metis -------------------------------- */
#include <metis/action_manager.h>
#include <metis/preferences_manager.h>
/* ------------------------------------ Ui ---------------------------------- */
#include "ui_main_window.h"
/* -------------------------------------------------------------------------- */

/* -------------------------------- Preferences ----------------------------- */

struct MainWindow::Preferences {
  metis::Preference<QByteArray> main_window_geometry =
      metis::Preference<QByteArray>("main_window/geometry");
  metis::Preference<QByteArray> main_window_state =
      metis::Preference<QByteArray>("main_window/state");
};

/* -------------------------------- MainWindow ------------------------------ */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_ui(new Ui::MainWindow),
      m_preferences(new Preferences) {
  registerActions();

  initUi();
  initConnections();
  readSettings();
}

MainWindow::~MainWindow() = default;

void MainWindow::closeEvent(QCloseEvent *event) {
  writeSettings();
  event->accept();
}

void MainWindow::registerActions() {
  
}

void MainWindow::initUi() {
  m_ui->setupUi(this);
}

void MainWindow::initConnections() {
  
}

void MainWindow::writeSettings() {
  m_preferences->main_window_geometry = saveGeometry();
  m_preferences->main_window_state = saveState();
}

void MainWindow::readSettings() {
  auto window_geometry = m_preferences->main_window_geometry.get();
  auto window_state = m_preferences->main_window_state.get();

  if (!window_geometry.isNull()) restoreGeometry(window_geometry);
  if (!window_state.isNull()) restoreState(window_state);
}
