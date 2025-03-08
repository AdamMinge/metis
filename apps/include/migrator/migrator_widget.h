#ifndef MIGRATOR_WIDGET_H
#define MIGRATOR_WIDGET_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QWidget>
/* -------------------------------------------------------------------------- */

namespace Ui
{
  class MigratorWidget;
}

class MigratorWidget final : public QWidget
{
  Q_OBJECT

private:
  struct Preferences;

public:
  explicit MigratorWidget(QWidget *parent = nullptr);
  ~MigratorWidget() override;

private:
  void initUi();
  void initConnections();

  void fillVersions();

private Q_SLOTS:
  void browseSnapshot();
  void browseOutput();

  void validate();
  [[nodiscard]] bool isSnapshotValid() const;
  [[nodiscard]] bool isOutputValid() const;
  [[nodiscard]] bool isVersionValid() const;

  void migrate();

private:
  QScopedPointer<Ui::MigratorWidget> m_ui;
  QScopedPointer<Preferences> m_preferences;
};

#endif// MIGRATOR_WIDGET_H
