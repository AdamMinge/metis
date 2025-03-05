#ifndef METIS_LOGGING_MANAGER_H
#define METIS_LOGGING_MANAGER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QObject>
/* --------------------------------- Standard ------------------------------- */
#include <memory>
/* ----------------------------------- Local -------------------------------- */
#include "metis/export.h"
/* -------------------------------------------------------------------------- */

namespace metis {

class LIB_METIS_API LoggingManager : public QObject {
  Q_OBJECT

 public:
  enum class Severity { Info, Warning, Error };

 public:
  [[nodiscard]] static LoggingManager &getInstance();
  static void deleteInstance();

 public:
  ~LoggingManager() override;

  void log(Severity severity, const QString &message);

 Q_SIGNALS:
  void onInfoLog(const QString &message);
  void onWarningLog(const QString &message);
  void onErrorLog(const QString &message);

 protected:
  explicit LoggingManager();

 private:
  static std::unique_ptr<LoggingManager> m_instance;
};

inline void LOG_INFO(const QString &message) {
  LoggingManager::getInstance().log(LoggingManager::Severity::Info, message);
}

inline void LOG_WARNING(const QString &message) {
  LoggingManager::getInstance().log(LoggingManager::Severity::Warning, message);
}

inline void LOG_ERROR(const QString &message) {
  LoggingManager::getInstance().log(LoggingManager::Severity::Error, message);
}

}  // namespace metis

#endif  // METIS_LOGGING_MANAGER_H
