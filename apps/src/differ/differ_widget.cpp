/* ----------------------------------- Local -------------------------------- */
#include "differ_widget.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QFileDialog>
/* ---------------------------------- Metis -------------------------------- */
#include <metis/differ.h>
#include <metis/logging_manager.h>
#include <metis/plugin_manager.h>
#include <metis/preferences_manager.h>
/* ------------------------------------ Ui ---------------------------------- */
#include "ui_differ_widget.h"
/* -------------------------------------------------------------------------- */

/* -------------------------------- Preferences ----------------------------- */

struct DifferWidget::Preferences {
};

/* -------------------------------- DifferWidget ---------------------------- */

DifferWidget::DifferWidget(QWidget *parent)
    : QWidget(parent), m_ui(new Ui::DifferWidget),
      m_preferences(new Preferences)
{
  initUi();
  initConnections();
  validate();
}

DifferWidget::~DifferWidget() = default;

void DifferWidget::initUi()
{
  m_ui->setupUi(this);
  fillVersions();
}

void DifferWidget::initConnections()
{
  connect(
    m_ui->first_snapshot_browser_button, &QToolButton::clicked, this,
    &DifferWidget::browseFirstSnapshot);
  connect(
    m_ui->second_snapshot_browser_button, &QToolButton::clicked, this,
    &DifferWidget::browseSecondSnapshot);

  connect(
    m_ui->first_snapshot_edit, &QLineEdit::textChanged, this,
    &DifferWidget::validate);
  connect(
    m_ui->second_snapshot_edit, &QLineEdit::textChanged, this,
    &DifferWidget::validate);
  connect(
    m_ui->version_edit, &QComboBox::currentIndexChanged, this,
    &DifferWidget::validate);

  connect(m_ui->diff_button, &QPushButton::clicked, this, &DifferWidget::diff);
}

void DifferWidget::fillVersions()
{
  m_ui->version_edit->clear();

  auto versions = QSet<QVersionNumber>{};
  auto differs =
    metis::PluginManager::getInstance().getObjects<metis::Differ>();
  for (const auto &differ : differs) { versions.insert(differ->version()); }

  auto ordered_versions = versions.values();
  std::sort(ordered_versions.begin(), ordered_versions.end());

  for (const auto &ordered_version : ordered_versions)
  {
    m_ui->version_edit->addItem(ordered_version.toString());
  }

  m_ui->version_edit->setCurrentIndex(0);
}

void DifferWidget::browseFirstSnapshot()
{
  const auto file_path = QFileDialog::getOpenFileName(
    this, tr("Select First Snapshot File"), "", tr("Snapshot Files (*)"));
  if (!file_path.isEmpty()) { m_ui->first_snapshot_edit->setText(file_path); }
}

void DifferWidget::browseSecondSnapshot()
{
  const auto file_path = QFileDialog::getOpenFileName(
    this, tr("Select Second Snapshot File"), "", tr("Snapshot Files (*)"));
  if (!file_path.isEmpty()) { m_ui->second_snapshot_edit->setText(file_path); }
}

void DifferWidget::validate()
{
  bool valid = true;

  if (valid) valid &= isFirstSnapshotValid();
  if (valid) valid &= isSecondSnapshotValid();
  if (valid) valid &= isVersionValid();

  m_ui->diff_button->setEnabled(valid);
}

bool DifferWidget::isFirstSnapshotValid() const
{
  const auto snapshot_path = m_ui->first_snapshot_edit->text();
  const auto snapshot_info = QFileInfo(snapshot_path);

  return snapshot_info.exists();
}

bool DifferWidget::isSecondSnapshotValid() const
{
  const auto snapshot_path = m_ui->second_snapshot_edit->text();
  const auto snapshot_info = QFileInfo(snapshot_path);

  return snapshot_info.exists();
}

bool DifferWidget::isVersionValid() const
{
  if (m_ui->version_edit->currentIndex() < 0) return false;
  return true;
}

void DifferWidget::diff()
{
  const auto first_snapshot_data =
    readSnapshot(m_ui->first_snapshot_edit->text());
  if (first_snapshot_data.isEmpty()) return;

  const auto second_snapshot_data =
    readSnapshot(m_ui->second_snapshot_edit->text());
  if (second_snapshot_data.isEmpty()) return;

  const auto version =
    QVersionNumber::fromString(m_ui->version_edit->currentText());

  auto differs =
    metis::PluginManager::getInstance().getObjects<metis::Differ>();
  auto differ =
    std::find_if(differs.begin(), differs.end(), [&version](auto differ) {
      return differ->version() == version;
    });

  if (differ == differs.end())
  {
    metis::LOG_ERROR(
      "Failed to find differ for version: " + version.toString());
    return;
  }

  auto model = (*differ)->diff(
    first_snapshot_data, second_snapshot_data, m_ui->diff_view_tree);
  if (!model)
  {
    metis::LOG_ERROR(
      "Failed to make diff between snapshot for differ version: " +
      version.toString());
    return;
  }

  m_ui->diff_view_tree->setModel(model);
}

QByteArray DifferWidget::readSnapshot(const QString &path) const
{
  auto snapshot_file = QFile(path);
  if (!snapshot_file.open(QIODevice::ReadOnly))
  {
    metis::LOG_ERROR(
      "Failed to open first snapshot file: " + snapshot_file.fileName());
    return QByteArray{};
  }

  const auto snapshot_data = snapshot_file.readAll();
  if (snapshot_data.isEmpty())
  {
    metis::LOG_ERROR(
      "First snapshot file is empty: " + snapshot_file.fileName());
    return QByteArray{};
  }

  return snapshot_data;
}