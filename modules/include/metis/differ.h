#ifndef METIS_DIFFER_H
#define METIS_DIFFER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QAbstractItemModel>
#include <QObject>
#include <QVersionNumber>
/* ----------------------------------- Local -------------------------------- */
#include "metis/export.h"
/* -------------------------------------------------------------------------- */

namespace metis
{
  class LIB_METIS_API Differ : public QObject
  {
    Q_OBJECT

  public:
    Differ(QObject *parent = nullptr);
    ~Differ() override;

    virtual QVersionNumber version() const = 0;
    virtual QAbstractItemModel *diff(
      const QByteArray &src, const QByteArray &dest,
      QObject *parent = nullptr) const = 0;
  };

}// namespace metis

#endif// METIS_DIFFER_H
