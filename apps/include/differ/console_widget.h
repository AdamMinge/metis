#ifndef CONSOLE_WIDGET_H
#define CONSOLE_WIDGET_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QPlainTextEdit>
#include <QPushButton>
#include <QWidget>
/* -------------------------------------------------------------------------- */

class ConsoleOutputWidget : public QPlainTextEdit
{
  Q_OBJECT

public:
  using QPlainTextEdit::QPlainTextEdit;

protected:
  void contextMenuEvent(QContextMenuEvent *event) override;
};

class ConsoleWidget final : public QWidget
{
  Q_OBJECT

public:
  explicit ConsoleWidget(QWidget *parent = nullptr);
  ~ConsoleWidget() override;

private Q_SLOTS:
  void onReport(const QString &str, const QColor &color = QColor{});

  void onInfoLog(const QString &message);
  void onWarningLog(const QString &message);
  void onErrorLog(const QString &message);

private:
  void initUi();
  void initConnections();

private:
  QPlainTextEdit *m_plain_text_edit;
  QPushButton *m_clear_button;
};

#endif// CONSOLE_WIDGET_H
