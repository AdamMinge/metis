#ifndef DIFFER_WIDGET_H
#define DIFFER_WIDGET_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QWidget>
/* -------------------------------------------------------------------------- */

namespace Ui
{
  class DifferWidget;
}

class DifferWidget final : public QWidget
{
  Q_OBJECT

private:
  struct Preferences;

public:
  explicit DifferWidget(QWidget *parent = nullptr);
  ~DifferWidget() override;

private:
  void initUi();
  void initConnections();

  void fillVersions();

private Q_SLOTS:
  void browseFirstSnapshot();
  void browseSecondSnapshot();

  void validate();
  [[nodiscard]] bool isFirstSnapshotValid() const;
  [[nodiscard]] bool isSecondSnapshotValid() const;
  [[nodiscard]] bool isVersionValid() const;

  void diff();
  [[nodiscard]] QByteArray readSnapshot(const QString &path) const;

private:
  QScopedPointer<Ui::DifferWidget> m_ui;
  QScopedPointer<Preferences> m_preferences;
};

#endif// DIFFER_WIDGET_H
