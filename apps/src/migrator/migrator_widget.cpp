/* ----------------------------------- Local -------------------------------- */
#include "migrator_widget.h"

#include "migration_manager.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QFileDialog>
/* ---------------------------------- Metis -------------------------------- */
#include <metis/logging_manager.h>
#include <metis/migrator.h>
#include <metis/plugin_manager.h>
#include <metis/preferences_manager.h>
/* ------------------------------------ Ui ---------------------------------- */
#include "ui_migrator_widget.h"
/* -------------------------------------------------------------------------- */

/* -------------------------------- Preferences ----------------------------- */

struct MigratorWidget::Preferences {
};

/* ------------------------------- MigratorWidget --------------------------- */

MigratorWidget::MigratorWidget(QWidget *parent)
    : QWidget(parent), m_ui(new Ui::MigratorWidget),
      m_preferences(new Preferences)
{
  initUi();
  initConnections();
  validate();
}

MigratorWidget::~MigratorWidget() = default;

void MigratorWidget::initUi()
{
  m_ui->setupUi(this);
  fillVersions();
}

void MigratorWidget::initConnections()
{
  connect(
    m_ui->snapshot_browser_button, &QToolButton::clicked, this,
    &MigratorWidget::browseSnapshot);
  connect(
    m_ui->output_browser_button, &QToolButton::clicked, this,
    &MigratorWidget::browseOutput);

  connect(
    m_ui->snapshot_edit, &QLineEdit::textChanged, this,
    &MigratorWidget::validate);
  connect(
    m_ui->output_edit, &QLineEdit::textChanged, this,
    &MigratorWidget::validate);
  connect(
    m_ui->snapshot_version_edit, &QComboBox::currentIndexChanged, this,
    &MigratorWidget::validate);
  connect(
    m_ui->output_version_edit, &QComboBox::currentIndexChanged, this,
    &MigratorWidget::validate);

  connect(
    m_ui->migrate_button, &QPushButton::clicked, this,
    &MigratorWidget::migrate);
}

void MigratorWidget::fillVersions()
{
  m_ui->snapshot_version_edit->clear();
  m_ui->output_version_edit->clear();

  auto versions = QSet<QVersionNumber>{};
  auto migrators =
    metis::PluginManager::getInstance().getObjects<metis::Migrator>();
  for (const auto &migrator : migrators)
  {
    versions.insert(migrator->fromVersion());
    versions.insert(migrator->toVersion());
  }

  auto ordered_versions = versions.values();
  std::sort(ordered_versions.begin(), ordered_versions.end());

  for (const auto &ordered_version : ordered_versions)
  {
    m_ui->snapshot_version_edit->addItem(ordered_version.toString());
    m_ui->output_version_edit->addItem(ordered_version.toString());
  }

  m_ui->snapshot_version_edit->setCurrentIndex(0);
  m_ui->output_version_edit->setCurrentIndex(
    m_ui->output_version_edit->count() - 1);
}

void MigratorWidget::browseSnapshot()
{
  const auto file_path = QFileDialog::getOpenFileName(
    this, tr("Select Snapshot File"), "", tr("Snapshot Files (*)"));
  if (!file_path.isEmpty()) { m_ui->snapshot_edit->setText(file_path); }
}

void MigratorWidget::browseOutput()
{
  const auto file_path = QFileDialog::getSaveFileName(
    this, tr("Select Output Snapshot File"), "", tr("Snapshot Files (*)"));
  if (!file_path.isEmpty()) { m_ui->output_edit->setText(file_path); }
}

void MigratorWidget::validate()
{
  bool valid = true;

  if (valid) valid &= isSnapshotValid();
  if (valid) valid &= isOutputValid();
  if (valid) valid &= isVersionValid();

  m_ui->migrate_button->setEnabled(valid);
}

bool MigratorWidget::isSnapshotValid() const
{
  const auto snapshot_path = m_ui->snapshot_edit->text();
  const auto snapshot_info = QFileInfo(snapshot_path);

  return snapshot_info.exists();
}

bool MigratorWidget::isOutputValid() const
{
  const auto output_path = m_ui->output_edit->text();
  return !output_path.isEmpty();
}

bool MigratorWidget::isVersionValid() const
{
  if (m_ui->snapshot_version_edit->currentIndex() < 0) return false;
  if (m_ui->output_version_edit->currentIndex() < 0) return false;

  return true;
}

void MigratorWidget::migrate()
{
  metis::LOG_INFO("Migration process started.");

  auto migration_manager = MigrationManager{};

  auto snapshot_file = QFile(m_ui->snapshot_edit->text());
  if (!snapshot_file.open(QIODevice::ReadOnly))
  {
    metis::LOG_ERROR(
      "Failed to open snapshot file: " + snapshot_file.fileName());
    return;
  }

  const auto snapshot_data = snapshot_file.readAll();
  if (snapshot_data.isEmpty())
  {
    metis::LOG_ERROR("Snapshot file is empty: " + snapshot_file.fileName());
    return;
  }

  const auto from_version =
    QVersionNumber::fromString(m_ui->snapshot_version_edit->currentText());
  const auto to_version =
    QVersionNumber::fromString(m_ui->output_version_edit->currentText());

  auto output_data =
    migration_manager.migrate(snapshot_data, from_version, to_version);
  if (output_data.isEmpty())
  {
    metis::LOG_ERROR(
      "Migration failed: No output data generated for version migration from " +
      from_version.toString() + " to " + to_version.toString());
    return;
  }

  auto output_file = QFile(m_ui->output_edit->text());
  if (!output_file.open(QIODevice::WriteOnly))
  {
    metis::LOG_ERROR(
      "Failed to open output file for writing: " + output_file.fileName());
    return;
  }

  auto bytes_written = output_file.write(output_data);
  if (bytes_written != output_data.size())
  {
    metis::LOG_ERROR(
      "Failed to write full output data to file: " + output_file.fileName() +
      " (expected " + QString::number(output_data.size()) + " bytes, wrote " +
      QString::number(bytes_written) + " bytes)");
    return;
  }

  metis::LOG_INFO(
    "Migration successful: " + from_version.toString() + " -> " +
    to_version.toString());
}