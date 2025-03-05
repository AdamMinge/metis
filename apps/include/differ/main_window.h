#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QMainWindow>
#include <QScopedPointer>
#include <QStackedWidget>
/* -------------------------------------------------------------------------- */

namespace Ui {
class MainWindow;
}

class MainWindow final : public QMainWindow {
  Q_OBJECT

 private:
  struct Preferences;

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

 protected:
  void closeEvent(QCloseEvent *event) override;

 private:
  void registerActions();

  void initUi();
  void initConnections();

  void writeSettings();
  void readSettings();

 private:
  QScopedPointer<Ui::MainWindow> m_ui;
  QScopedPointer<Preferences> m_preferences;
};

#endif  // MAIN_WINDOW_H
