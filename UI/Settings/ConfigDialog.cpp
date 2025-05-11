#include "ConfigDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>
#include <QFileDialog>
#include <QSettings>
#include <QSpacerItem>

namespace QT_UI {

ConfigDialog::ConfigDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    loadSettings();
    
    // Connect the dialog buttons
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        saveSettings();
        accept();
    });
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

ConfigDialog::~ConfigDialog()
{
    // All widgets are parented and automatically cleaned up
}

void ConfigDialog::setupUi()
{
    // Set window properties
    setWindowTitle(tr("Configuration"));
    setWindowIcon(QIcon(":/assets/icons/Resources/icons/pj64.ico"));
    resize(600, 550);
    
    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Create tab widget
    m_tabWidget = new QTabWidget(this);
    
    // Create General tab
    QWidget *generalTab = new QWidget();
    QVBoxLayout *generalLayout = new QVBoxLayout(generalTab);
    
    // Create toolbox for general settings
    m_generalToolBox = new QToolBox(generalTab);
    
    // General section
    QWidget *generalSection = new QWidget();
    QVBoxLayout *generalSectionLayout = new QVBoxLayout(generalSection);
    
    m_pauseWhenInactiveCheck = new QCheckBox(tr("Pause emulation when window is not active"), generalSection);
    m_enterFullscreenCheck = new QCheckBox(tr("Enter fullscreen mode when loading a ROM"), generalSection);
    m_disableScreensaverCheck = new QCheckBox(tr("Disable screensaver when running a ROM"), generalSection);
    m_enableDiscordCheck = new QCheckBox(tr("Enable Discord Rich Presence"), generalSection);
    m_hideAdvancedSettingsCheck = new QCheckBox(tr("Hide advanced settings"), generalSection);
    
    generalSectionLayout->addWidget(m_pauseWhenInactiveCheck);
    generalSectionLayout->addWidget(m_enterFullscreenCheck);
    generalSectionLayout->addWidget(m_disableScreensaverCheck);
    generalSectionLayout->addWidget(m_enableDiscordCheck);
    generalSectionLayout->addWidget(m_hideAdvancedSettingsCheck);
    
    generalSectionLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    
    QFormLayout *romsFormLayout = new QFormLayout();
    QLabel *maxRomsLabel = new QLabel(tr("Max # of ROMs remembered (0-10):"), generalSection);
    m_maxRomsSpinBox = new QSpinBox(generalSection);
    m_maxRomsSpinBox->setMinimum(0);
    m_maxRomsSpinBox->setMaximum(10);
    m_maxRomsSpinBox->setValue(4);
    romsFormLayout->addRow(maxRomsLabel, m_maxRomsSpinBox);
    
    QLabel *maxRomDirsLabel = new QLabel(tr("Max # of ROM dirs remembered (0-10):"), generalSection);
    m_maxRomDirsSpinBox = new QSpinBox(generalSection);
    m_maxRomDirsSpinBox->setMinimum(0);
    m_maxRomDirsSpinBox->setMaximum(10);
    m_maxRomDirsSpinBox->setValue(4);
    romsFormLayout->addRow(maxRomDirsLabel, m_maxRomDirsSpinBox);
    
    generalSectionLayout->addLayout(romsFormLayout);
    generalSectionLayout->addStretch();
    
    m_generalToolBox->addItem(generalSection, tr("General"));
    
    // Advanced section
    QWidget *advancedSection = new QWidget();
    m_generalToolBox->addItem(advancedSection, tr("Advanced"));
    
    // Defaults section
    QWidget *defaultsSection = new QWidget();
    m_generalToolBox->addItem(defaultsSection, tr("Defaults"));
    
    // Directories section
    QWidget *directoriesSection = new QWidget();
    QVBoxLayout *directoriesLayout = new QVBoxLayout(directoriesSection);
    
    // Plugin directory group
    QGroupBox *pluginDirGroup = new QGroupBox(tr("Plugin directory:"), directoriesSection);
    QVBoxLayout *pluginDirLayout = new QVBoxLayout(pluginDirGroup);
    
    m_pluginDirEdit = new QLineEdit(pluginDirGroup);
    pluginDirLayout->addWidget(m_pluginDirEdit);
    
    m_browsePluginButton = new QPushButton(tr("Browse..."), pluginDirGroup);
    m_browsePluginButton->setMaximumWidth(378);
    connect(m_browsePluginButton, &QPushButton::clicked, this, &ConfigDialog::browsePluginDir);
    pluginDirLayout->addWidget(m_browsePluginButton);
    
    m_useDefaultPluginDirCheck = new QCheckBox(tr("Use Default: $AppPath\\Plugin\\"), pluginDirGroup);
    pluginDirLayout->addWidget(m_useDefaultPluginDirCheck);
    
    directoriesLayout->addWidget(pluginDirGroup);
    
    // Saves directory group
    QGroupBox *savesDirGroup = new QGroupBox(tr("N64 native saves directory:"), directoriesSection);
    QVBoxLayout *savesDirLayout = new QVBoxLayout(savesDirGroup);
    
    m_savesDirEdit = new QLineEdit(savesDirGroup);
    savesDirLayout->addWidget(m_savesDirEdit);
    
    m_browseSavesButton = new QPushButton(tr("Browse..."), savesDirGroup);
    m_browseSavesButton->setMaximumWidth(378);
    connect(m_browseSavesButton, &QPushButton::clicked, this, &ConfigDialog::browseSavesDir);
    savesDirLayout->addWidget(m_browseSavesButton);
    
    m_useDefaultSavesDirCheck = new QCheckBox(tr("Use Default: $AppPath\\Save\\"), savesDirGroup);
    savesDirLayout->addWidget(m_useDefaultSavesDirCheck);
    
    directoriesLayout->addWidget(savesDirGroup);
    directoriesLayout->addStretch();
    
    m_generalToolBox->addItem(directoriesSection, tr("Directories"));
    
    // 64DD section
    QWidget *ddSection = new QWidget();
    m_generalToolBox->addItem(ddSection, tr("64DD"));
    
    generalLayout->addWidget(m_generalToolBox);
    
    // Set the current toolbox page
    m_generalToolBox->setCurrentIndex(0); // General page
    
    m_tabWidget->addTab(generalTab, tr("General"));
    
    // ROM Browser tab
    QWidget *romBrowserTab = new QWidget();
    m_tabWidget->addTab(romBrowserTab, tr("ROM Browser"));
    
    // Hotkeys tab
    QWidget *hotkeysTab = new QWidget();
    m_tabWidget->addTab(hotkeysTab, tr("Hotkeys"));
    
    // Plugins tab
    QWidget *pluginsTab = new QWidget();
    m_tabWidget->addTab(pluginsTab, tr("Plugins"));
    
    mainLayout->addWidget(m_tabWidget);
    
    // Add button box
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply | QDialogButtonBox::Reset, this);
    connect(m_buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &ConfigDialog::applySettings);
    connect(m_buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, this, &ConfigDialog::resetSettings);
    mainLayout->addWidget(m_buttonBox);
    
    setLayout(mainLayout);
}

void ConfigDialog::loadSettings()
{
    QSettings settings;
    
    // Load general settings
    m_pauseWhenInactiveCheck->setChecked(settings.value("general/pauseWhenInactive", true).toBool());
    m_enterFullscreenCheck->setChecked(settings.value("general/enterFullscreen", false).toBool());
    m_disableScreensaverCheck->setChecked(settings.value("general/disableScreensaver", true).toBool());
    m_enableDiscordCheck->setChecked(settings.value("general/enableDiscord", true).toBool());
    m_hideAdvancedSettingsCheck->setChecked(settings.value("general/hideAdvancedSettings", false).toBool());
    m_maxRomsSpinBox->setValue(settings.value("general/maxRoms", 4).toInt());
    m_maxRomDirsSpinBox->setValue(settings.value("general/maxRomDirs", 4).toInt());
    
    // Load directory settings
    bool useDefaultPlugin = settings.value("directories/useDefaultPlugin", true).toBool();
    m_useDefaultPluginDirCheck->setChecked(useDefaultPlugin);
    m_pluginDirEdit->setText(settings.value("directories/pluginDir", "Plugin").toString());
    m_pluginDirEdit->setEnabled(!useDefaultPlugin);
    
    bool useDefaultSaves = settings.value("directories/useDefaultSaves", true).toBool();
    m_useDefaultSavesDirCheck->setChecked(useDefaultSaves);
    m_savesDirEdit->setText(settings.value("directories/savesDir", "Save").toString());
    m_savesDirEdit->setEnabled(!useDefaultSaves);
    
    // Connect checkboxes to enable/disable edit fields
    connect(m_useDefaultPluginDirCheck, &QCheckBox::toggled, this, [this](bool checked) {
        m_pluginDirEdit->setEnabled(!checked);
    });
    
    connect(m_useDefaultSavesDirCheck, &QCheckBox::toggled, this, [this](bool checked) {
        m_savesDirEdit->setEnabled(!checked);
    });
}

void ConfigDialog::saveSettings()
{
    QSettings settings;
    
    // Save general settings
    settings.setValue("general/pauseWhenInactive", m_pauseWhenInactiveCheck->isChecked());
    settings.setValue("general/enterFullscreen", m_enterFullscreenCheck->isChecked());
    settings.setValue("general/disableScreensaver", m_disableScreensaverCheck->isChecked());
    settings.setValue("general/enableDiscord", m_enableDiscordCheck->isChecked());
    settings.setValue("general/hideAdvancedSettings", m_hideAdvancedSettingsCheck->isChecked());
    settings.setValue("general/maxRoms", m_maxRomsSpinBox->value());
    settings.setValue("general/maxRomDirs", m_maxRomDirsSpinBox->value());
    
    // Save directory settings
    settings.setValue("directories/useDefaultPlugin", m_useDefaultPluginDirCheck->isChecked());
    settings.setValue("directories/pluginDir", m_pluginDirEdit->text());
    settings.setValue("directories/useDefaultSaves", m_useDefaultSavesDirCheck->isChecked());
    settings.setValue("directories/savesDir", m_savesDirEdit->text());
}

void ConfigDialog::browsePluginDir()
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

void ConfigDialog::browseSavesDir()
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

void ConfigDialog::applySettings()
{
    saveSettings();
}

void ConfigDialog::resetSettings()
{
    // Reset to default values
    m_pauseWhenInactiveCheck->setChecked(true);
    m_enterFullscreenCheck->setChecked(false);
    m_disableScreensaverCheck->setChecked(true);
    m_enableDiscordCheck->setChecked(true);
    m_hideAdvancedSettingsCheck->setChecked(false);
    m_maxRomsSpinBox->setValue(4);
    m_maxRomDirsSpinBox->setValue(4);
    
    m_useDefaultPluginDirCheck->setChecked(true);
    m_pluginDirEdit->setText("Plugin");
    m_pluginDirEdit->setEnabled(false);
    
    m_useDefaultSavesDirCheck->setChecked(true);
    m_savesDirEdit->setText("Save");
    m_savesDirEdit->setEnabled(false);
}

} // namespace QT_UI
