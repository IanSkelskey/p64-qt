#include "DirectoriesSettingsPage.h"
#include <Core/Settings/SettingsManager.h>
#include <Core/Settings/PluginSettings.h>
#include <Core/Settings/SaveSettings.h>
#include <Core/Settings/RomBrowserSettings.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QApplication>
#include <QDir>

namespace QT_UI {

DirectoriesSettingsPage::DirectoriesSettingsPage(QWidget* parent)
    : BaseSettingsPage(parent)
{
    setupUI();
    loadSettings();
}

void DirectoriesSettingsPage::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Plugin directory group
    QGroupBox* pluginDirGroup = new QGroupBox(tr("Plugin directory:"));
    QVBoxLayout* pluginDirLayout = new QVBoxLayout(pluginDirGroup);
    
    m_pluginDirEdit = new QLineEdit();
    pluginDirLayout->addWidget(m_pluginDirEdit);
    
    m_browsePluginButton = new QPushButton(tr("Browse..."));
    connect(m_browsePluginButton, &QPushButton::clicked, this, &DirectoriesSettingsPage::browsePluginDir);
    pluginDirLayout->addWidget(m_browsePluginButton);
    
    m_useDefaultPluginDirCheck = new QCheckBox(tr("Use Default: $AppPath\\Plugin\\"));
    connect(m_useDefaultPluginDirCheck, &QCheckBox::toggled, this, &DirectoriesSettingsPage::toggleUseDefaultPluginDir);
    pluginDirLayout->addWidget(m_useDefaultPluginDirCheck);
    
    mainLayout->addWidget(pluginDirGroup);
    
    // Saves directory group
    QGroupBox* savesDirGroup = new QGroupBox(tr("N64 native saves directory:"));
    QVBoxLayout* savesDirLayout = new QVBoxLayout(savesDirGroup);
    
    m_savesDirEdit = new QLineEdit();
    savesDirLayout->addWidget(m_savesDirEdit);
    
    m_browseSavesButton = new QPushButton(tr("Browse..."));
    connect(m_browseSavesButton, &QPushButton::clicked, this, &DirectoriesSettingsPage::browseSavesDir);
    savesDirLayout->addWidget(m_browseSavesButton);
    
    m_useDefaultSavesDirCheck = new QCheckBox(tr("Use Default: $AppPath\\Save\\"));
    connect(m_useDefaultSavesDirCheck, &QCheckBox::toggled, this, &DirectoriesSettingsPage::toggleUseDefaultSavesDir);
    savesDirLayout->addWidget(m_useDefaultSavesDirCheck);
    
    mainLayout->addWidget(savesDirGroup);
    
    // Screenshots directory group
    QGroupBox* screenshotsDirGroup = new QGroupBox(tr("Screenshots directory:"));
    QVBoxLayout* screenshotsDirLayout = new QVBoxLayout(screenshotsDirGroup);
    
    m_screenshotsDirEdit = new QLineEdit();
    screenshotsDirLayout->addWidget(m_screenshotsDirEdit);
    
    m_browseScreenshotsButton = new QPushButton(tr("Browse..."));
    connect(m_browseScreenshotsButton, &QPushButton::clicked, this, &DirectoriesSettingsPage::browseScreenshotsDir);
    screenshotsDirLayout->addWidget(m_browseScreenshotsButton);
    
    m_useDefaultScreenshotsDirCheck = new QCheckBox(tr("Use Default: $AppPath\\Screenshots\\"));
    connect(m_useDefaultScreenshotsDirCheck, &QCheckBox::toggled, 
            [this](bool checked) { m_screenshotsDirEdit->setEnabled(!checked); });
    screenshotsDirLayout->addWidget(m_useDefaultScreenshotsDirCheck);
    
    mainLayout->addWidget(screenshotsDirGroup);
    
    // Covers directory group
    QGroupBox* coversDirGroup = new QGroupBox(tr("Cover images directory:"));
    QVBoxLayout* coversDirLayout = new QVBoxLayout(coversDirGroup);
    
    m_coversDirEdit = new QLineEdit();
    coversDirLayout->addWidget(m_coversDirEdit);
    
    m_browseCoversButton = new QPushButton(tr("Browse..."));
    connect(m_browseCoversButton, &QPushButton::clicked, this, &DirectoriesSettingsPage::browseCoversDir);
    coversDirLayout->addWidget(m_browseCoversButton);
    
    m_useDefaultCoversDirCheck = new QCheckBox(tr("Use Default: $AppPath\\covers\\"));
    connect(m_useDefaultCoversDirCheck, &QCheckBox::toggled, 
            [this](bool checked) { m_coversDirEdit->setEnabled(!checked); });
    coversDirLayout->addWidget(m_useDefaultCoversDirCheck);
    
    mainLayout->addWidget(coversDirGroup);
    mainLayout->addStretch();
    
    // Connect settings changed signals
    connect(m_pluginDirEdit, &QLineEdit::textChanged, this, &BaseSettingsPage::settingsChanged);
    connect(m_savesDirEdit, &QLineEdit::textChanged, this, &BaseSettingsPage::settingsChanged);
    connect(m_screenshotsDirEdit, &QLineEdit::textChanged, this, &BaseSettingsPage::settingsChanged);
    connect(m_coversDirEdit, &QLineEdit::textChanged, this, &BaseSettingsPage::settingsChanged);
    connect(m_useDefaultPluginDirCheck, &QCheckBox::toggled, this, &BaseSettingsPage::settingsChanged);
    connect(m_useDefaultSavesDirCheck, &QCheckBox::toggled, this, &BaseSettingsPage::settingsChanged);
    connect(m_useDefaultScreenshotsDirCheck, &QCheckBox::toggled, this, &BaseSettingsPage::settingsChanged);
    connect(m_useDefaultCoversDirCheck, &QCheckBox::toggled, this, &BaseSettingsPage::settingsChanged);
}

void DirectoriesSettingsPage::loadSettings()
{
    auto& settings = SettingsManager::instance();
    
    // Plugin directory
    bool useDefaultPlugin = settings.plugins()->useDefaultPluginDirectory();
    m_useDefaultPluginDirCheck->setChecked(useDefaultPlugin);
    m_pluginDirEdit->setText(settings.plugins()->pluginDirectory());
    m_pluginDirEdit->setEnabled(!useDefaultPlugin);
    
    // Saves directory
    bool useDefaultSaves = settings.saves()->useDefaultSavesDirectory();
    m_useDefaultSavesDirCheck->setChecked(useDefaultSaves);
    m_savesDirEdit->setText(settings.saves()->getSavesDirectory());
    m_savesDirEdit->setEnabled(!useDefaultSaves);
    
    // Screenshots directory (using QSettings directly as this is not in the settings classes yet)
    QSettings qSettings("Project64", "QtUI");
    bool useDefaultScreenshots = qSettings.value("Directories/UseDefaultScreenshots", true).toBool();
    QString screenshotsDir = qSettings.value("Directories/ScreenshotsDir", 
                                           QDir(QApplication::applicationDirPath()).filePath("Screenshots")).toString();
    
    m_useDefaultScreenshotsDirCheck->setChecked(useDefaultScreenshots);
    m_screenshotsDirEdit->setText(screenshotsDir);
    m_screenshotsDirEdit->setEnabled(!useDefaultScreenshots);
    
    // Covers directory
    QString coversDir = settings.romBrowser()->coverDirectory();
    bool useDefaultCovers = qSettings.value("RomBrowser/UseDefaultCoverDir", true).toBool();
    
    m_useDefaultCoversDirCheck->setChecked(useDefaultCovers);
    m_coversDirEdit->setText(coversDir);
    m_coversDirEdit->setEnabled(!useDefaultCovers);
}

void DirectoriesSettingsPage::saveSettings()
{
    auto& settings = SettingsManager::instance();
    
    // Plugin directory
    settings.plugins()->setUseDefaultPluginDirectory(m_useDefaultPluginDirCheck->isChecked());
    settings.plugins()->setPluginDirectory(m_pluginDirEdit->text());
    
    // Saves directory
    settings.saves()->setUseDefaultSavesDirectory(m_useDefaultSavesDirCheck->isChecked());
    settings.saves()->setSavesDirectory(m_savesDirEdit->text());
    
    // Screenshots directory (using QSettings directly)
    QSettings qSettings("Project64", "QtUI");
    qSettings.setValue("Directories/UseDefaultScreenshots", m_useDefaultScreenshotsDirCheck->isChecked());
    qSettings.setValue("Directories/ScreenshotsDir", m_screenshotsDirEdit->text());
    
    // Covers directory
    qSettings.setValue("RomBrowser/UseDefaultCoverDir", m_useDefaultCoversDirCheck->isChecked());
    settings.romBrowser()->setCoverDirectory(m_coversDirEdit->text());
}

void DirectoriesSettingsPage::resetSettings()
{
    m_useDefaultPluginDirCheck->setChecked(true);
    m_pluginDirEdit->setText(QDir(QApplication::applicationDirPath()).filePath("Plugin"));
    m_pluginDirEdit->setEnabled(false);
    
    m_useDefaultSavesDirCheck->setChecked(true);
    m_savesDirEdit->setText(QDir(QApplication::applicationDirPath()).filePath("Save"));
    m_savesDirEdit->setEnabled(false);
    
    m_useDefaultScreenshotsDirCheck->setChecked(true);
    m_screenshotsDirEdit->setText(QDir(QApplication::applicationDirPath()).filePath("Screenshots"));
    m_screenshotsDirEdit->setEnabled(false);
    
    m_useDefaultCoversDirCheck->setChecked(true);
    m_coversDirEdit->setText(QDir(QApplication::applicationDirPath()).filePath("covers"));
    m_coversDirEdit->setEnabled(false);
    
    emit settingsChanged();
}

void DirectoriesSettingsPage::browsePluginDir()
{
    QString dir = QFileDialog::getExistingDirectory(
        this, 
        tr("Select Plugin Directory"),
        m_pluginDirEdit->text(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    if (!dir.isEmpty()) {
        m_pluginDirEdit->setText(dir);
    }
}

void DirectoriesSettingsPage::browseSavesDir()
{
    QString dir = QFileDialog::getExistingDirectory(
        this, 
        tr("Select Saves Directory"),
        m_savesDirEdit->text(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    if (!dir.isEmpty()) {
        m_savesDirEdit->setText(dir);
    }
}

void DirectoriesSettingsPage::browseScreenshotsDir()
{
    QString dir = QFileDialog::getExistingDirectory(
        this, 
        tr("Select Screenshots Directory"),
        m_screenshotsDirEdit->text(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    if (!dir.isEmpty()) {
        m_screenshotsDirEdit->setText(dir);
    }
}

void DirectoriesSettingsPage::browseCoversDir()
{
    QString dir = QFileDialog::getExistingDirectory(
        this, 
        tr("Select Covers Directory"),
        m_coversDirEdit->text(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    if (!dir.isEmpty()) {
        m_coversDirEdit->setText(dir);
    }
}

void DirectoriesSettingsPage::toggleUseDefaultPluginDir(bool checked)
{
    m_pluginDirEdit->setEnabled(!checked);
    if (checked) {
        m_pluginDirEdit->setText(QDir(QApplication::applicationDirPath()).filePath("Plugin"));
    }
}

void DirectoriesSettingsPage::toggleUseDefaultSavesDir(bool checked)
{
    m_savesDirEdit->setEnabled(!checked);
    if (checked) {
        m_savesDirEdit->setText(QDir(QApplication::applicationDirPath()).filePath("Save"));
    }
}

} // namespace QT_UI
