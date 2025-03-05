/* ----------------------------------- Local -------------------------------- */
#include "metis/logging_manager.h"
/* -------------------------------------------------------------------------- */

namespace metis {

std::unique_ptr<LoggingManager> LoggingManager::m_instance =
    std::unique_ptr<LoggingManager>(nullptr);

LoggingManager &LoggingManager::getInstance() {
  if (!m_instance) m_instance.reset(new LoggingManager);

  return *m_instance;
}

void LoggingManager::deleteInstance() { m_instance.reset(nullptr); }

LoggingManager::LoggingManager() = default;

LoggingManager::~LoggingManager() = default;

void LoggingManager::log(Severity severity, const QString &message) {
  switch (severity) {
    case LoggingManager::Severity::Info:
      Q_EMIT onInfoLog(message);
      break;
    case LoggingManager::Severity::Warning:
      Q_EMIT onWarningLog(message);
      break;
    case LoggingManager::Severity::Error:
      Q_EMIT onErrorLog(message);
      break;
  }
}

}  // namespace metis
