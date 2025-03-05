/* ----------------------------------- Local -------------------------------- */
#include "metis/command_line_parser.h"
/* -------------------------------------------------------------------------- */

namespace metis {

/* ----------------------------------- Option ------------------------------- */

CommandLineParser::Option::Option(const QCommandLineOption &cmd_option,
                                  std::function<void(const QString &)> callback)
    : cmd_option(cmd_option), callback(std::move(callback)) {}

/* ----------------------------- CommandLineParser -------------------------- */

CommandLineParser::CommandLineParser() = default;

CommandLineParser::~CommandLineParser() = default;

void CommandLineParser::process(const QCoreApplication &app) {
  QCommandLineParser parser;
  parser.setApplicationDescription("Metis Editor");
  parser.addHelpOption();
  parser.addVersionOption();

  for (auto &option : m_options) parser.addOption(option.cmd_option);

  parser.process(app);

  for (auto &option : m_options) {
    if (parser.isSet(option.cmd_option)) {
      auto value = QString{};
      if (!option.cmd_option.valueName().isEmpty())
        value = parser.value(option.cmd_option);

      option.callback(value);
    }
  }
}

void CommandLineParser::registerOption(const QStringList &names,
                                       const QString &description,
                                       const std::function<void()> &callback) {
  registerOptionImpl(names, description, [callback](const QString &value) {
    Q_UNUSED(value);
    callback();
  });
}

void CommandLineParser::registerOptionImpl(
    const QStringList &names, const QString &description,
    const std::function<void(const QString &)> &callback,
    const QString &valueName) {
  Q_ASSERT(!names.isEmpty());
  Q_ASSERT(!description.isEmpty());

  auto cmd_option = QCommandLineOption(names, description, valueName);

  m_options.emplace_back(Option(cmd_option, callback));
}

}  // namespace metis
