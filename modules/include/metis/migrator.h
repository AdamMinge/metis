#ifndef METIS_MIGRATOR_H
#define METIS_MIGRATOR_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QObject>
#include <QVersionNumber>
/* ----------------------------------- Local -------------------------------- */
#include "metis/export.h"
/* -------------------------------------------------------------------------- */

namespace metis
{

  class LIB_METIS_API Migrator : public QObject
  {
    Q_OBJECT

  public:
    Migrator(QObject *parent = nullptr);
    ~Migrator() override;

    virtual QVersionNumber fromVersion() const = 0;
    virtual QVersionNumber toVersion() const = 0;

    virtual QByteArray migrate(const QByteArray &from) const = 0;
  };

}// namespace metis

#endif// METIS_MIGRATOR_H
