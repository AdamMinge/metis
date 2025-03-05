/* ----------------------------------- Local -------------------------------- */
#include "main_window.h"
/* ---------------------------------- Metis -------------------------------- */
#include <metis/application.h>
#include <metis/plugin_manager.h>
/* -------------------------------------------------------------------------- */

/* ----------------------------- messagesToConsole -------------------------- */

static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER =
  qInstallMessageHandler(nullptr);

static void messagesToConsole(
  QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  switch (type)
  {
    case QtInfoMsg:
    case QtDebugMsg:
      // TODO : implementation //
      break;

    case QtWarningMsg:
      // TODO : implementation //
      break;

    case QtCriticalMsg:
      // TODO : implementation //
      break;

    default:
      break;
  }

  (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg);
}

/* ----------------------------- enableAllPlugins --------------------------- */

void enableAllPlugins()
{
  auto plugins = metis::PluginManager::getInstance().getPlugins();
  for (auto &plugin : plugins) plugin->enable();
}

/* ----------------------------------- main --------------------------------- */

int main(int argc, char **argv)
{
  metis::Application app(argc, argv);

  qInstallMessageHandler(messagesToConsole);
  enableAllPlugins();

  MainWindow mainWindow;
  mainWindow.show();

  return app.exec();
}
