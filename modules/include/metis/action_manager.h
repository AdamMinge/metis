#ifndef METIS_ACTION_MANAGER_H
#define METIS_ACTION_MANAGER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QAction>
#include <QHash>
#include <QMenu>
#include <QObject>
/* --------------------------------- Standard ------------------------------- */
#include <memory>
/* ----------------------------------- Local -------------------------------- */
#include "metis/export.h"
/* -------------------------------------------------------------------------- */

namespace metis {

class LIB_METIS_API ActionManager : public QObject {
  Q_OBJECT

 public:
  static ActionManager &getInstance();
  static void deleteInstance();

 public:
  ~ActionManager() override;

  void registerAction(QAction *action, const QString &id);
  void unregisterAction(QAction *action, const QString &id);

  void registerMenu(QMenu *menu, const QString &id);
  void unregisterMenu(const QString &id);

  [[nodiscard]] QAction *findAction(const QString &id) const;
  [[nodiscard]] QMenu *findMenu(const QString &id) const;

  [[nodiscard]] QString getActionId(QAction *action) const;
  [[nodiscard]] QString getMenuId(QMenu *action) const;

  [[nodiscard]] QKeySequence getDefaultShortcut(const QString &id) const;

  [[nodiscard]] QList<QString> getActionsId() const;
  [[nodiscard]] QList<QString> getMenusId() const;

  void setCustomShortcut(const QString &id, const QKeySequence &keySequence);
  [[nodiscard]] bool hasCustomShortcut(const QString &id) const;

  void resetCustomShortcut(const QString &id);
  void resetAllCustomShortcuts();

 Q_SIGNALS:
  void changedAction(const QString &id);
  void registeredAction(const QString &id);
  void unregisteredAction(const QString &id);

 private:
  void applyShortcut(QAction *action, const QKeySequence &shortcut);
  void updateToolTipWithShortcut(QAction *action);

 private:
  explicit ActionManager();

 private:
  static std::unique_ptr<ActionManager> m_instance;

  QMultiHash<QString, QAction *> m_actions;
  QHash<QString, QMenu *> m_menus;

  QHash<QString, QKeySequence> m_default_shortcuts;
  QHash<QString, QKeySequence> m_custom_shortcuts;

  bool m_applying_shortcut;
  bool m_applying_tooltip_with_shortcut;
};

}  // namespace metis

#endif  // METIS_ACTION_MANAGER_H
