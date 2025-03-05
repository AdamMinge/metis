/* ----------------------------------- Local -------------------------------- */
#include "metis/preferences_manager.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QSettings>
/* -------------------------------------------------------------------------- */

namespace metis {

/* -------------------------- SessionPreferencesSettings -------------------- */

class SessionPreferencesSettings : public PreferencesSettings {
  friend PreferencesSettings;

 public:
  ~SessionPreferencesSettings() override = default;

  [[nodiscard]] Type getType() const override;

  void setValue(const QString &key, const QVariant &value) override;
  [[nodiscard]] QVariant value(const QString &key,
                               const QVariant &defaultValue) const override;

  void remove(const QString &key) override;
  [[nodiscard]] bool contains(const QString &key) const override;
  [[nodiscard]] QStringList allKeys() const override;

  void clear() override;

 protected:
  explicit SessionPreferencesSettings() = default;

 private:
  QSettings m_settings;
};

SessionPreferencesSettings::Type SessionPreferencesSettings::getType() const {
  return Type::Session;
}

void SessionPreferencesSettings::setValue(const QString &key,
                                          const QVariant &value) {
  m_settings.setValue(key, value);
}

QVariant SessionPreferencesSettings::value(const QString &key,
                                           const QVariant &defaultValue) const {
  return m_settings.value(key, defaultValue);
}

void SessionPreferencesSettings::remove(const QString &key) {
  m_settings.remove(key);
}

bool SessionPreferencesSettings::contains(const QString &key) const {
  return m_settings.contains(key);
}

QStringList SessionPreferencesSettings::allKeys() const {
  return m_settings.allKeys();
}

void SessionPreferencesSettings::clear() { m_settings.clear(); }

/* ------------------------- TemporaryPreferencesSettings ------------------- */

class TemporaryPreferencesSettings : public PreferencesSettings {
  friend PreferencesSettings;

 public:
  ~TemporaryPreferencesSettings() override = default;

  [[nodiscard]] Type getType() const override;

  void setValue(const QString &key, const QVariant &value) override;
  [[nodiscard]] QVariant value(const QString &key,
                               const QVariant &defaultValue) const override;

  void remove(const QString &key) override;
  [[nodiscard]] bool contains(const QString &key) const override;
  [[nodiscard]] QStringList allKeys() const override;

  void clear() override;

 protected:
  explicit TemporaryPreferencesSettings() = default;

 private:
  QMap<QString, QVariant> m_settings;
};

TemporaryPreferencesSettings::Type TemporaryPreferencesSettings::getType()
    const {
  return Type::Temporary;
}

void TemporaryPreferencesSettings::setValue(const QString &key,
                                            const QVariant &value) {
  m_settings[key] = value;
}

QVariant TemporaryPreferencesSettings::value(
    const QString &key, const QVariant &defaultValue) const {
  return m_settings.value(key, defaultValue);
}

void TemporaryPreferencesSettings::remove(const QString &key) {
  m_settings.remove(key);
}

bool TemporaryPreferencesSettings::contains(const QString &key) const {
  return m_settings.contains(key);
}

QStringList TemporaryPreferencesSettings::allKeys() const {
  return m_settings.keys();
}

void TemporaryPreferencesSettings::clear() { m_settings.clear(); }

/* ------------------------------ PreferencesSettings ----------------------- */

std::unique_ptr<PreferencesSettings> PreferencesSettings::create(Type type) {
  switch (type) {
    case Type::Session:
      return std::unique_ptr<SessionPreferencesSettings>(
          new SessionPreferencesSettings);

    case Type::Temporary:
      return std::unique_ptr<TemporaryPreferencesSettings>(
          new TemporaryPreferencesSettings);
  }

  return nullptr;
}

/* ----------------------------- PreferencesManager ------------------------- */

std::unique_ptr<PreferencesManager> PreferencesManager::m_instance =
    std::unique_ptr<PreferencesManager>(nullptr);

PreferencesManager &PreferencesManager::getInstance() {
  if (!m_instance) m_instance.reset(new PreferencesManager);

  return *m_instance;
}

void PreferencesManager::deleteInstance() { m_instance.reset(nullptr); }

PreferencesManager::PreferencesManager()
    : m_settings(
          PreferencesSettings::create(PreferencesSettings::Type::Session)) {
  load();
}

PreferencesManager::~PreferencesManager() { save(); }

void PreferencesManager::setSettingsType(PreferencesSettings::Type type) {
  if (getSettingsType() != type) {
    save();
    m_settings = PreferencesSettings::create(type);
    load();
  }
}

PreferencesSettings::Type PreferencesManager::getSettingsType() const {
  return m_settings->getType();
}

void PreferencesManager::load() {
  m_recent_project_files =
      m_settings->value("document/recent_document_files", QStringList{})
          .toStringList();
}

void PreferencesManager::save() {
  m_settings->setValue("document/recent_document_files",
                       m_recent_project_files);
}

void PreferencesManager::addRecentProjectFile(const QString &recent_file) {
  if (m_recent_project_files.contains(recent_file)) return;

  m_recent_project_files << recent_file;
  Q_EMIT recentProjectFilesChanged();
}

void PreferencesManager::clearRecentProjectFiles() {
  if (m_recent_project_files.isEmpty()) return;

  m_recent_project_files.clear();
  Q_EMIT recentProjectFilesChanged();
}

QStringList PreferencesManager::getRecentProjectFiles() const {
  return m_recent_project_files;
}

bool PreferencesManager::contains(const QString &key) {
  return m_settings->contains(key);
}

void PreferencesManager::remove(const QString &key) { m_settings->remove(key); }

void PreferencesManager::clear() { m_settings->clear(); }

QStringList PreferencesManager::getAllKeys() const {
  return m_settings->allKeys();
}

}  // namespace metis
