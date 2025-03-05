/* ----------------------------------- Local -------------------------------- */
#include "differ_widget.h"
/* ---------------------------------- Metis -------------------------------- */
#include <metis/preferences_manager.h>
/* ------------------------------------ Ui ---------------------------------- */
#include "ui_differ_widget.h"
/* -------------------------------------------------------------------------- */

/* -------------------------------- Preferences ----------------------------- */

struct DifferWidget::Preferences {

};

/* -------------------------------- DifferWidget ---------------------------- */

DifferWidget::DifferWidget(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::DifferWidget),
      m_preferences(new Preferences) {
  initUi();
  initConnections();
}

DifferWidget::~DifferWidget() = default;

void DifferWidget::initUi() {
  m_ui->setupUi(this);
}

void DifferWidget::initConnections() {
  
}

