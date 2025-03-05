/* ----------------------------------- Local -------------------------------- */
#include "console_widget.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QHBoxLayout>
#include <QMenu>
#include <QVBoxLayout>
/* ----------------------------------- Metis -------------------------------- */
#include <metis/logging_manager.h>
/* -------------------------------------------------------------------------- */

/* ----------------------------------- Utils -------------------------------- */

namespace
{

  QString getColorName(const QColor &color)
  {
    static const auto color_names = QColor::colorNames();
    for (const auto &color_name : color_names)
      if (QColor(color_name) == color) return color_name;

    return QString{};
  }


}// namespace

/* ---------------------------- ConsoleOutputWidget ------------------------- */

void ConsoleOutputWidget::contextMenuEvent(QContextMenuEvent *event)
{
  std::unique_ptr<QMenu> menu{createStandardContextMenu(event->pos())};

  menu->addSeparator();
  menu->addAction(tr("Clear Console"), this, &QPlainTextEdit::clear);

  menu->exec(event->globalPos());
}

/* ------------------------------- ConsoleWidget ---------------------------- */

ConsoleWidget::ConsoleWidget(QWidget *parent)
    : QWidget(parent), m_plain_text_edit(new ConsoleOutputWidget()),
      m_clear_button(new QPushButton(tr("Clear Console")))
{
  initUi();
  initConnections();
}

ConsoleWidget::~ConsoleWidget() = default;

void ConsoleWidget::onReport(const QString &str, const QColor &color)
{
  m_plain_text_edit->appendHtml(
    QLatin1String("<pre style='color:%1'>").arg(getColorName(color)) +
    str.toHtmlEscaped() + QLatin1String("</pre>"));
}

void ConsoleWidget::onInfoLog(const QString &message) { onReport(message); }

void ConsoleWidget::onWarningLog(const QString &message)
{
  onReport(message, QColor("orange"));
}

void ConsoleWidget::onErrorLog(const QString &message)
{
  onReport(message, QColor("red"));
}

void ConsoleWidget::initUi()
{
  m_plain_text_edit->setReadOnly(true);
  auto palette = m_plain_text_edit->palette();
  palette.setColor(QPalette::Base, Qt::black);
  palette.setColor(QPalette::Text, Qt::lightGray);
  m_plain_text_edit->setPalette(palette);

  auto layout = new QVBoxLayout(this);
  auto bottomBar = new QHBoxLayout();

  bottomBar->addWidget(m_clear_button);
  bottomBar->setSpacing(2);

  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(m_plain_text_edit);
  layout->addLayout(bottomBar);

  setLayout(layout);
}

void ConsoleWidget::initConnections()
{
  connect(
    m_clear_button, &QPushButton::clicked, m_plain_text_edit,
    &QPlainTextEdit::clear);

  auto &logging_manager = metis::LoggingManager::getInstance();
  connect(
    &logging_manager, &metis::LoggingManager::onInfoLog, this,
    &ConsoleWidget::onInfoLog);
  connect(
    &logging_manager, &metis::LoggingManager::onWarningLog, this,
    &ConsoleWidget::onWarningLog);
  connect(
    &logging_manager, &metis::LoggingManager::onErrorLog, this,
    &ConsoleWidget::onErrorLog);
}
