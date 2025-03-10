/* ----------------------------------- Local -------------------------------- */
#include "metis/plugin.h"

#include "metis/plugin_interface.h"
#include "metis/plugin_manager.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QPluginLoader>
#include <memory>
/* -------------------------------------------------------------------------- */

namespace metis
{

  /* -------------------------------- Plugin Impl ----------------------------- */

  class PluginImpl
  {
  public:
    explicit PluginImpl() = default;
    virtual ~PluginImpl() = default;

    [[nodiscard]] virtual QString getName() const = 0;
    [[nodiscard]] virtual QString getDescription() const = 0;
    [[nodiscard]] virtual QString getVersion() const = 0;
    [[nodiscard]] virtual QString getFileName() const = 0;
    [[nodiscard]] virtual QIcon getIcon() const = 0;

    [[nodiscard]] virtual bool isStatic() const = 0;
    [[nodiscard]] bool isDynamic() const;

    virtual bool enable() = 0;
    virtual bool disable() = 0;

    [[nodiscard]] virtual bool isEnabled() const = 0;
  };

  bool PluginImpl::isDynamic() const { return !isStatic(); }

  /* ---------------------------- Dynamic Plugin Impl ------------------------- */

  class DynamicPluginImpl : public PluginImpl
  {
  public:
    explicit DynamicPluginImpl(std::unique_ptr<QPluginLoader> loader);
    ~DynamicPluginImpl() override;

    [[nodiscard]] QString getName() const override;
    [[nodiscard]] QString getDescription() const override;
    [[nodiscard]] QString getVersion() const override;
    [[nodiscard]] QString getFileName() const override;
    [[nodiscard]] QIcon getIcon() const override;

    [[nodiscard]] bool isStatic() const override;

    bool enable() override;
    bool disable() override;

    [[nodiscard]] bool isEnabled() const override;

  private:
    std::unique_ptr<QPluginLoader> m_loader;
    QString m_name;
    QString m_description;
    QString m_version;
    QIcon m_icon;
    QObject *m_instance;
  };

  DynamicPluginImpl::DynamicPluginImpl(std::unique_ptr<QPluginLoader> loader)
      : m_loader(std::move(loader)), m_instance(nullptr)
  {
    const auto metaData =
      m_loader->metaData().value(QStringLiteral("MetaData")).toObject();

    m_name = metaData.value(QStringLiteral("Name")).toString();
    m_description = metaData.value(QStringLiteral("Description")).toString();
    m_version = metaData.value(QStringLiteral("Version")).toString();

    QLibrary lib(m_loader->fileName());
    if (auto getPluginIcon =
          reinterpret_cast<QIcon *(*) ()>(lib.resolve("getPluginIcon"));
        getPluginIcon)
    {
      auto plugin_icon = std::unique_ptr<QIcon>(getPluginIcon());
      m_icon = QIcon(*plugin_icon);
    } else
    {
      m_icon = QIcon();
    }
    lib.unload();
  }

  DynamicPluginImpl::~DynamicPluginImpl() = default;

  QString DynamicPluginImpl::getName() const { return m_name; }

  QString DynamicPluginImpl::getDescription() const { return m_description; }

  QString DynamicPluginImpl::getVersion() const { return m_version; }

  QString DynamicPluginImpl::getFileName() const
  {
    return m_loader->fileName();
  }

  QIcon DynamicPluginImpl::getIcon() const { return m_icon; }

  bool DynamicPluginImpl::isStatic() const { return false; }

  bool DynamicPluginImpl::enable()
  {
    if (isEnabled()) return true;

    m_instance = m_loader->instance();
    if (!m_instance) return false;

    if (auto plugin_interface = qobject_cast<PluginInterface *>(m_instance);
        plugin_interface)
    {
      plugin_interface->init();
    } else
    {
      PluginManager::getInstance().addObject(m_instance);
    }

    return true;
  }

  bool DynamicPluginImpl::disable()
  {
    if (!isEnabled()) return true;

    if (auto plugin_interface = qobject_cast<PluginInterface *>(m_instance);
        !plugin_interface)
    {
      PluginManager::getInstance().removeObject(m_instance);
    }

    auto unloaded = m_loader->unload();
    if (unloaded) m_instance = nullptr;

    return unloaded;
  }

  bool DynamicPluginImpl::isEnabled() const { return m_instance != nullptr; }

  /* ----------------------------- Static Plugin Impl ------------------------- */

  class StaticPluginImpl : public PluginImpl
  {
  public:
    explicit StaticPluginImpl(QObject *instance);
    ~StaticPluginImpl() override;

    [[nodiscard]] QString getName() const override;
    [[nodiscard]] QString getDescription() const override;
    [[nodiscard]] QString getVersion() const override;
    [[nodiscard]] QString getFileName() const override;
    [[nodiscard]] QIcon getIcon() const override;

    [[nodiscard]] bool isStatic() const override;

    bool enable() override;
    bool disable() override;

    [[nodiscard]] bool isEnabled() const override;

  private:
    QObject *m_instance;
  };

  StaticPluginImpl::StaticPluginImpl(QObject *instance) : m_instance(instance)
  {}

  StaticPluginImpl::~StaticPluginImpl() = default;

  QString StaticPluginImpl::getName() const { return QLatin1String{}; }

  QString StaticPluginImpl::getDescription() const { return QLatin1String{}; }

  QString StaticPluginImpl::getVersion() const { return QLatin1String{}; }

  QString StaticPluginImpl::getFileName() const { return QLatin1String{}; }

  QIcon StaticPluginImpl::getIcon() const { return QIcon{}; }

  bool StaticPluginImpl::isStatic() const { return true; }

  bool StaticPluginImpl::enable()
  {
    if (isEnabled()) return true;

    if (auto plugin_interface = qobject_cast<PluginInterface *>(m_instance);
        plugin_interface)
      plugin_interface->init();
    else
      PluginManager::getInstance().addObject(m_instance);

    return true;
  }

  bool StaticPluginImpl::disable()
  {
    if (isEnabled()) PluginManager::getInstance().removeObject(m_instance);
    return true;
  }

  bool StaticPluginImpl::isEnabled() const
  {
    return PluginManager::getInstance().getObjects<QObject>().contains(
      m_instance);
  }

  /* ----------------------------------- Plugin ------------------------------- */

  Plugin Plugin::create(const QString &plugin_file)
  {
    auto loader = std::make_unique<QPluginLoader>(plugin_file);
    auto impl = std::make_unique<DynamicPluginImpl>(std::move(loader));
    return Plugin(std::move(impl));
  }

  Plugin Plugin::create(QObject *instance)
  {
    auto impl = std::make_unique<StaticPluginImpl>(instance);
    return Plugin(std::move(impl));
  }

  Plugin::Plugin(std::unique_ptr<PluginImpl> impl) : m_impl(std::move(impl)) {}

  Plugin::~Plugin()
  {
    if (isEnabled()) disable();
  }

  QString Plugin::getName() const
  {
    return m_impl ? m_impl->getName() : QString{};
  }

  QString Plugin::getDescription() const
  {
    return m_impl ? m_impl->getDescription() : QString{};
  }

  QString Plugin::getVersion() const
  {
    return m_impl ? m_impl->getVersion() : QString{};
  }

  QString Plugin::getFileName() const
  {
    return m_impl ? m_impl->getFileName() : QString{};
  }

  QIcon Plugin::getIcon() const { return m_impl ? m_impl->getIcon() : QIcon{}; }

  bool Plugin::isStatic() const { return m_impl && m_impl->isStatic(); }

  bool Plugin::isDynamic() const { return m_impl && m_impl->isDynamic(); }

  Plugin::Plugin(Plugin &&other) noexcept = default;

  Plugin &Plugin::operator=(Plugin &&other) noexcept = default;

  bool Plugin::enable() { return m_impl && m_impl->enable(); }

  bool Plugin::disable() { return m_impl && m_impl->disable(); }

  bool Plugin::isEnabled() const { return m_impl && m_impl->isEnabled(); }

}// namespace metis
