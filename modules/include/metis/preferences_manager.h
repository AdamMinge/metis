#ifndef METIS_PREFERENCES_MANAGER_H
#define METIS_PREFERENCES_MANAGER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QObject>
#include <QString>
/* --------------------------------- Standard ------------------------------- */
#include <memory>
/* ----------------------------------- Local -------------------------------- */
#include "metis/export.h"
/* -------------------------------------------------------------------------- */

namespace metis
{

  class LIB_METIS_API PreferencesSettings
  {
  public:
    enum class Type
    {
      Session,
      Temporary
    };

  public:
    [[nodiscard]] static std::unique_ptr<PreferencesSettings> create(Type type);

  public:
    virtual ~PreferencesSettings() = default;

    [[nodiscard]] virtual Type getType() const = 0;

    virtual void setValue(const QString &key, const QVariant &value) = 0;
    [[nodiscard]] virtual QVariant
    value(const QString &key, const QVariant &defaultValue) const = 0;

    virtual void remove(const QString &key) = 0;
    [[nodiscard]] virtual bool contains(const QString &key) const = 0;
    [[nodiscard]] virtual QStringList allKeys() const = 0;

    virtual void clear() = 0;

  protected:
    explicit PreferencesSettings() = default;
  };

  class LIB_METIS_API PreferencesManager : public QObject
  {
    Q_OBJECT

  public:
    [[nodiscard]] static PreferencesManager &getInstance();
    static void deleteInstance();

  public:
    ~PreferencesManager() override;

    void setSettingsType(PreferencesSettings::Type type);
    [[nodiscard]] PreferencesSettings::Type getSettingsType() const;

    void addRecentProjectFile(const QString &recent_file);
    void clearRecentProjectFiles();
    [[nodiscard]] QStringList getRecentProjectFiles() const;

    template<typename TYPE>
    void setValue(const QString &key, const TYPE &value);
    template<typename TYPE>
    [[nodiscard]] TYPE
    getValue(const QString &key, const TYPE &default_value = TYPE()) const;

    [[nodiscard]] bool contains(const QString &key);
    void remove(const QString &key);
    void clear();

    [[nodiscard]] QStringList getAllKeys() const;

  Q_SIGNALS:
    void recentProjectFilesChanged();

  protected:
    explicit PreferencesManager();

    void load();
    void save();

  private:
    static std::unique_ptr<PreferencesManager> m_instance;

    std::unique_ptr<PreferencesSettings> m_settings;
    QStringList m_recent_project_files;
  };

  template<typename TYPE>
  void PreferencesManager::setValue(const QString &key, const TYPE &value)
  {
    m_settings->setValue(key, value);
  }

  template<typename TYPE>
  TYPE PreferencesManager::getValue(
    const QString &key, const TYPE &default_value) const
  {
    return m_settings->value(key, default_value).template value<TYPE>();
  }

  template<typename TYPE>
  class Preference
  {
  public:
    explicit Preference(QString key, TYPE default_value = TYPE());

    Preference &operator=(const TYPE &value);

    [[nodiscard]] TYPE get() const;
    void set(const TYPE &value);

    [[nodiscard]] bool contains();
    [[nodiscard]] QString getKey() const;

    operator TYPE() const;// NOLINT(google-explicit-constructor)

  private:
    QString m_key;
    TYPE m_default_value;
  };

  template<typename TYPE>
  Preference<TYPE>::Preference(QString key, TYPE default_value)
      : m_key(std::move(key)), m_default_value(default_value)
  {}

  template<typename TYPE>
  Preference<TYPE> &Preference<TYPE>::operator=(const TYPE &value)
  {
    set(value);
    return *this;
  }

  template<typename TYPE>
  TYPE Preference<TYPE>::get() const
  {
    return PreferencesManager::getInstance().getValue<TYPE>(
      m_key, m_default_value);
  }

  template<typename TYPE>
  void Preference<TYPE>::set(const TYPE &value)
  {
    PreferencesManager::getInstance().setValue(m_key, value);
  }

  template<typename TYPE>
  bool Preference<TYPE>::contains()
  {
    return PreferencesManager::getInstance().contains(m_key);
  }

  template<typename TYPE>
  QString Preference<TYPE>::getKey() const
  {
    return m_key;
  }

  template<typename TYPE>
  Preference<TYPE>::operator TYPE() const
  {
    return get();
  }

  template<typename TYPE>
  class PreferenceContainer
  {
  public:
    explicit PreferenceContainer(QString key, TYPE default_value = TYPE());

    PreferenceContainer &operator=(const TYPE &value);

    [[nodiscard]] TYPE get(const QString &preference_key) const;
    void set(const QString &preference_key, const TYPE &value);

    [[nodiscard]] bool contains(const QString &preference_key);
    [[nodiscard]] QString getKey() const;
    [[nodiscard]] QString getPreferenceKey(const QString &preference_key) const;

  private:
    QString m_key;
    TYPE m_default_value;
  };

  template<typename TYPE>
  PreferenceContainer<TYPE>::PreferenceContainer(
    QString key, TYPE default_value)
      : m_key(std::move(key)), m_default_value(default_value)
  {}

  template<typename TYPE>
  PreferenceContainer<TYPE> &
  PreferenceContainer<TYPE>::operator=(const TYPE &value)
  {
    set(value);
    return *this;
  }

  template<typename TYPE>
  TYPE PreferenceContainer<TYPE>::get(const QString &preference_key) const
  {
    return Preference<TYPE>{getPreferenceKey(preference_key)}.get();
  }

  template<typename TYPE>
  void PreferenceContainer<TYPE>::set(
    const QString &preference_key, const TYPE &value)
  {
    return Preference<TYPE>{getPreferenceKey(preference_key)}.set(value);
  }

  template<typename TYPE>
  bool PreferenceContainer<TYPE>::contains(const QString &preference_key)
  {
    return Preference<TYPE>{getPreferenceKey(preference_key)}.contains();
  }

  template<typename TYPE>
  QString PreferenceContainer<TYPE>::getKey() const
  {
    return m_key;
  }

  template<typename TYPE>
  QString PreferenceContainer<TYPE>::getPreferenceKey(
    const QString &preference_key) const
  {
    return QLatin1String("%1/%2").arg(m_key, preference_key);
    ;
  }

}// namespace metis

#endif// METIS_PREFERENCES_MANAGER_H
