/* ----------------------------------- Local -------------------------------- */
#include "metis/application.h"

#include "metis/action_manager.h"
#include "metis/command_line_parser.h"
#include "metis/config.h"
#include "metis/logging_manager.h"
#include "metis/plugin_manager.h"
#include "metis/preferences_manager.h"
/* -------------------------------------------------------------------------- */

namespace metis
{

  /* -------------------------------- Preferences ----------------------------- */

  struct Application::Preferences {
    metis::Preference<QLocale> application_language =
      metis::Preference<QLocale>("application/language");
    metis::Preference<QString> application_style =
      metis::Preference<QString>("application/style");
    metis::Preference<QStringList> application_enabled_plugins =
      metis::Preference<QStringList>("application/enabled_plugins");
    metis::PreferenceContainer<QKeySequence> application_shortcuts =
      metis::PreferenceContainer<QKeySequence>("application/shortcuts");
  };

  /* -------------------------- BaseCommandLineParser ------------------------- */

  class BaseCommandLineParser : public CommandLineParser
  {
  public:
    explicit BaseCommandLineParser();
    ~BaseCommandLineParser() override;

    [[nodiscard]] bool isWithoutSettings() const { return m_without_settings; };
    [[nodiscard]] QStringList getPluginsPaths() const
    {
      return m_plugins_paths;
    };

  private:
    bool m_without_settings;
    QStringList m_plugins_paths;
  };

  BaseCommandLineParser::BaseCommandLineParser() : m_without_settings(false)
  {
    registerOption(
      {"without-preferences"},
      QObject::tr("Execute application without loading/saving preferences"),
      [this]() { m_without_settings = true; });

    registerOption<QString>(
      {"plugins-paths"}, QObject::tr("Specified locations of plugins to load"),
      [this](auto &plugin_path) { m_plugins_paths = plugin_path.split(';'); },
      QLatin1String("paths"));
  }

  BaseCommandLineParser::~BaseCommandLineParser() = default;

  /* -------------------------------- Application ----------------------------- */

  Application::Application(int &argc, char **argv)
      : QApplication(argc, argv), m_preferences(new Preferences)
  {
    setApplicationName(QStringLiteral("Metis"));
    setApplicationVersion(QLatin1String(METIS_VERSION_STR));
    setApplicationDisplayName(QStringLiteral("Metis"));
    setOrganizationName(QStringLiteral("Metis"));

    parseCommandLine();
    readSettings();
  }

  Application::~Application() = default;

  bool Application::event(QEvent *event)
  {
    if (event->type() == QEvent::Quit) { writeSettings(); }

    return QApplication::event(event);
  }

  void Application::parseCommandLine()
  {
    BaseCommandLineParser parser;
    parser.process(*this);

    if (parser.isWithoutSettings())
      metis::PreferencesManager::getInstance().setSettingsType(
        metis::PreferencesSettings::Type::Temporary);

    if (auto paths = libraryPaths() + parser.getPluginsPaths();
        !paths.isEmpty())
      metis::PluginManager::getInstance().setDefaultPluginsPaths(paths);
  }

  void Application::writeSettings()
  {
    writePlugins();
    writeShortcuts();
  }

  void Application::writePlugins()
  {
    auto enabled_plugins = QStringList{};
    for (const auto plugin :
         metis::PluginManager::getInstance().getDynamicPlugins())
    {
      if (plugin->isEnabled()) enabled_plugins << plugin->getFileName();
    }

    m_preferences->application_enabled_plugins = enabled_plugins;
  }

  void Application::writeShortcuts()
  {
    const auto actions_id = metis::ActionManager::getInstance().getActionsId();
    for (const auto &action_id : actions_id)
    {
      auto action = metis::ActionManager::getInstance().findAction(action_id);
      m_preferences->application_shortcuts.set(action_id, action->shortcut());
    }
  }

  void Application::readSettings()
  {
    readPlugins();
    readShortcuts();
  }

  void Application::readPlugins()
  {
    const auto enabled_plugins =
      m_preferences->application_enabled_plugins.get();

    metis::PluginManager::getInstance().loadPlugins();
    for (auto plugin : metis::PluginManager::getInstance().getPlugins())
    {
      if (enabled_plugins.contains(plugin->getFileName())) plugin->enable();
    }
  }

  void Application::readShortcuts()
  {
    const auto actions_id = metis::ActionManager::getInstance().getActionsId();
    for (const auto &action_id : actions_id)
    {
      if (m_preferences->application_shortcuts.contains(action_id))
      {
        auto shortcut = m_preferences->application_shortcuts.get(action_id);
        metis::ActionManager::getInstance().setCustomShortcut(
          action_id, shortcut);
      }
    }
  }

}// namespace metis
