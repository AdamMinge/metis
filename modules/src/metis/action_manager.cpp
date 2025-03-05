/* ----------------------------------- Local -------------------------------- */
#include "metis/action_manager.h"
/* -------------------------------------------------------------------------- */

namespace metis {

std::unique_ptr<ActionManager> ActionManager::m_instance =
    std::unique_ptr<ActionManager>(nullptr);

ActionManager &ActionManager::getInstance() {
  if (!m_instance) m_instance.reset(new ActionManager);

  return *m_instance;
}

void ActionManager::deleteInstance() { m_instance.reset(nullptr); }

ActionManager::ActionManager()
    : m_applying_shortcut(false), m_applying_tooltip_with_shortcut(false) {}

ActionManager::~ActionManager() = default;

void ActionManager::registerAction(QAction *action, const QString &id) {
  Q_ASSERT_X(!m_actions.contains(id, action), "ActionManager::registerAction",
             "duplicate action");

  m_actions.insert(id, action);
  m_default_shortcuts.insert(id, action->shortcut());
  m_custom_shortcuts.insert(id, action->shortcut());

  connect(action, &QAction::changed, [this, id, action]() {
    if (m_applying_tooltip_with_shortcut) return;

    if (!m_applying_shortcut && m_custom_shortcuts.contains(id)) {
      applyShortcut(action, m_custom_shortcuts.value(id));
      return;
    }

    updateToolTipWithShortcut(action);
    Q_EMIT changedAction(id);
  });

  updateToolTipWithShortcut(action);
  Q_EMIT registeredAction(id);
}

void ActionManager::unregisterAction(QAction *action, const QString &id) {
  Q_ASSERT_X(m_actions.contains(id, action), "ActionManager::unregisterAction",
             "unknown action");

  m_actions.remove(id, action);
  action->disconnect(this);
  m_default_shortcuts.remove(id);
  Q_EMIT unregisteredAction(id);
}

void ActionManager::registerMenu(QMenu *menu, const QString &id) {
  Q_ASSERT_X(!m_menus.contains(id), "ActionManager::registerMenu",
             "duplicate id");
  m_menus.insert(id, menu);
}

void ActionManager::unregisterMenu(const QString &id) {
  Q_ASSERT_X(m_menus.contains(id), "ActionManager::unregisterMenu",
             "unknown id");
  m_menus.remove(id);
}

QAction *ActionManager::findAction(const QString &id) const {
  auto action = m_actions.value(id, nullptr);
  return action;
}

QMenu *ActionManager::findMenu(const QString &id) const {
  auto menu = m_menus.value(id, nullptr);
  return menu;
}

QString ActionManager::getActionId(QAction *action) const {
  return m_actions.key(action);
}

QString ActionManager::getMenuId(QMenu *action) const {
  return m_menus.key(action);
}

QKeySequence ActionManager::getDefaultShortcut(const QString &id) const {
  return m_default_shortcuts.value(id);
}

QList<QString> ActionManager::getActionsId() const {
  return m_actions.uniqueKeys();
}

QList<QString> ActionManager::getMenusId() const { return m_menus.keys(); }

void ActionManager::setCustomShortcut(const QString &id,
                                      const QKeySequence &keySequence) {
  auto actions = m_actions.values(id);
  Q_ASSERT_X(!actions.isEmpty(), "ActionManager::setCustomShortcut",
             "unknown id");

  if (!hasCustomShortcut(id))
    m_default_shortcuts.insert(id, actions.first()->shortcut());

  m_custom_shortcuts.insert(id, keySequence);

  for (auto action : actions) applyShortcut(action, keySequence);
}

bool ActionManager::hasCustomShortcut(const QString &id) const {
  return m_custom_shortcuts.contains(id);
}

void ActionManager::resetCustomShortcut(const QString &id) {
  if (!hasCustomShortcut(id)) return;

  auto actions = m_actions.values(id);
  Q_ASSERT_X(!actions.isEmpty(), "ActionManager::resetCustomShortcut",
             "unknown id");

  auto defaultShortcut = m_default_shortcuts.take(id);
  for (auto action : actions) applyShortcut(action, defaultShortcut);
  m_custom_shortcuts.remove(id);
}

void ActionManager::resetAllCustomShortcuts() {
  QHashIterator<QString, QKeySequence> iterator(m_default_shortcuts);
  while (iterator.hasNext()) {
    iterator.next();
    auto actions = m_actions.values(iterator.key());
    for (auto action : actions) applyShortcut(action, iterator.value());
  }

  m_default_shortcuts.clear();
  m_custom_shortcuts.clear();
}

void ActionManager::applyShortcut(QAction *action,
                                  const QKeySequence &shortcut) {
  m_applying_shortcut = true;
  action->setShortcut(shortcut);
  m_applying_shortcut = false;
}

void ActionManager::updateToolTipWithShortcut(QAction *action) {
  m_applying_tooltip_with_shortcut = true;

  QString toolTip = action->toolTip();

  if (toolTip.contains(QLatin1String(" <span "))) {
    action->setToolTip(QString());
    toolTip = action->toolTip();
  }

  if (!action->shortcut().isEmpty()) {
    toolTip.append(
        QStringLiteral(" <span style=\"color: gray;\">(%1)<span>")
            .arg(action->shortcut().toString(QKeySequence::NativeText)));
  }

  action->setToolTip(toolTip);

  m_applying_tooltip_with_shortcut = false;
}

}  // namespace metis
