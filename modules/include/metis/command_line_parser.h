#ifndef METIS_COMMAND_LINE_PARSER_H
#define METIS_COMMAND_LINE_PARSER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
/* ---------------------------------- Standard ------------------------------ */
#include <list>
/* -------------------------------------------------------------------------- */

namespace metis {

class CommandLineParser {
 private:
  struct Option {
    explicit Option(const QCommandLineOption &cmd_option,
                    std::function<void(const QString &)> callback = {});

    QCommandLineOption cmd_option;
    std::function<void(const QString &)> callback;
  };

 public:
  explicit CommandLineParser();
  virtual ~CommandLineParser();

  void process(const QCoreApplication &app);

 protected:
  void registerOption(const QStringList &names, const QString &description,
                      const std::function<void()> &callback);

  template <typename TYPE>
  void registerOption(const QStringList &names, const QString &description,
                      const std::function<void(const TYPE &)> &callback,
                      const QString &valueName);

  template <typename TYPE>
  [[nodiscard]] TYPE convertValue(const QString &value);

 private:
  void registerOptionImpl(const QStringList &names, const QString &description,
                          const std::function<void(const QString &)> &callback,
                          const QString &valueName = QString{});

 private:
  QList<Option> m_options;
};

template <typename TYPE>
void CommandLineParser::registerOption(
    const QStringList &names, const QString &description,
    const std::function<void(const TYPE &)> &callback,
    const QString &valueName) {
  registerOptionImpl(
      names, description,
      [callback, this](const QString &value) {
        callback(convertValue<TYPE>(value));
      },
      valueName);
}

/* -------------------------- Convert specializations  ---------------------- */

template <typename TYPE>
TYPE CommandLineParser::convertValue(const QString &value) {
  return static_cast<TYPE>(value);
}

#define CONVERTER_VALUE_SPEC(TYPE, METHOD)                                  \
  template <>                                                               \
  inline TYPE CommandLineParser::convertValue<TYPE>(const QString &value) { \
    return (METHOD);                                                        \
  }

CONVERTER_VALUE_SPEC(QString, value)
CONVERTER_VALUE_SPEC(short, value.toShort())
CONVERTER_VALUE_SPEC(ushort, value.toUShort())
CONVERTER_VALUE_SPEC(int, value.toInt())
CONVERTER_VALUE_SPEC(uint, value.toUInt())
CONVERTER_VALUE_SPEC(long, value.toLong())
CONVERTER_VALUE_SPEC(ulong, value.toULong())
CONVERTER_VALUE_SPEC(qlonglong, value.toLongLong())
CONVERTER_VALUE_SPEC(qulonglong, value.toULongLong())
CONVERTER_VALUE_SPEC(float, value.toFloat())
CONVERTER_VALUE_SPEC(double, value.toDouble())
#undef CONVERTER_VALUE_SPEC

}  // namespace metis

#endif  // METIS_COMMAND_LINE_PARSER_H
