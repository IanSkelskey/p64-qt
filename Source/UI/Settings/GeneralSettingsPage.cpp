#include "GeneralSettingsPage.h"
#include <Core/Settings/SettingsManager.h>
#include <Core/Settings/ApplicationSettings.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>

namespace QT_UI {

GeneralSettingsPage::GeneralSettingsPage(QWidget* parent)
    : BaseSettingsPage(parent)
{
    setupUI();
    loadSettings();
}

void GeneralSettingsPage::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Application behavior group
    QGroupBox* behaviorGroup = new QGroupBox(tr("Application Behavior"));
    QVBoxLayout* behaviorLayout = new QVBoxLayout(behaviorGroup);
    
    m_pauseWhenInactiveCheck = new QCheckBox(tr("Pause emulation when window is not active"));
    m_enterFullscreenCheck = new QCheckBox(tr("Enter fullscreen mode when loading a ROM"));
    m_disableScreensaverCheck = new QCheckBox(tr("Disable screensaver when running a ROM"));
    m_enableDiscordCheck = new QCheckBox(tr("Enable Discord Rich Presence"));
    m_hideAdvancedSettingsCheck = new QCheckBox(tr("Hide advanced settings"));
    
    behaviorLayout->addWidget(m_pauseWhenInactiveCheck);
    behaviorLayout->addWidget(m_enterFullscreenCheck);
    behaviorLayout->addWidget(m_disableScreensaverCheck);
    behaviorLayout->addWidget(m_enableDiscordCheck);
    behaviorLayout->addWidget(m_hideAdvancedSettingsCheck);
    
    // Connect setting changes
    connect(m_pauseWhenInactiveCheck, &QCheckBox::toggled, this, &GeneralSettingsPage::settingsChanged);
    connect(m_enterFullscreenCheck, &QCheckBox::toggled, this, &GeneralSettingsPage::settingsChanged);
    connect(m_disableScreensaverCheck, &QCheckBox::toggled, this, &GeneralSettingsPage::settingsChanged);
    connect(m_enableDiscordCheck, &QCheckBox::toggled, this, &GeneralSettingsPage::settingsChanged);
    connect(m_hideAdvancedSettingsCheck, &QCheckBox::toggled, this, &GeneralSettingsPage::updateUI);
    
    mainLayout->addWidget(behaviorGroup);
    
    // ROM History group
    QGroupBox* historyGroup = new QGroupBox(tr("ROM History"));
    QFormLayout* historyLayout = new QFormLayout(historyGroup);
    
    m_maxRomsSpinBox = new QSpinBox();
    m_maxRomsSpinBox->setMinimum(0);
    m_maxRomsSpinBox->setMaximum(50);
    m_maxRomDirsSpinBox = new QSpinBox();
    m_maxRomDirsSpinBox->setMinimum(0);
    m_maxRomDirsSpinBox->setMaximum(50);
    
    historyLayout->addRow(tr("Max # of ROMs remembered:"), m_maxRomsSpinBox);
    historyLayout->addRow(tr("Max # of ROM dirs remembered:"), m_maxRomDirsSpinBox);
    
    connect(m_maxRomsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &GeneralSettingsPage::settingsChanged);
    connect(m_maxRomDirsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &GeneralSettingsPage::settingsChanged);
    
    mainLayout->addWidget(historyGroup);
    
    // Advanced group
    m_advancedGroup = new QGroupBox(tr("Advanced Settings"));
    QVBoxLayout* advancedLayout = new QVBoxLayout(m_advancedGroup);
    
    // Add advanced settings here
    QCheckBox* autoSaveCheck = new QCheckBox(tr("Auto-save configuration"));
    QCheckBox* confirmOnResetCheck = new QCheckBox(tr("Confirm when resetting game"));
    QCheckBox* confirmOnExitCheck = new QCheckBox(tr("Confirm when exiting emulator"));
    
    advancedLayout->addWidget(autoSaveCheck);
    advancedLayout->addWidget(confirmOnResetCheck);
    advancedLayout->addWidget(confirmOnExitCheck);
    
    mainLayout->addWidget(m_advancedGroup);
    mainLayout->addStretch();
    
    // Connect hide advanced settings toggle
    connect(m_hideAdvancedSettingsCheck, &QCheckBox::toggled, m_advancedGroup, &QGroupBox::setHidden);
}

void GeneralSettingsPage::loadSettings()
{
    auto& settings = SettingsManager::instance();
    auto* appSettings = settings.application();
    
    m_pauseWhenInactiveCheck->setChecked(appSettings->pauseWhenInactive());
    m_enterFullscreenCheck->setChecked(appSettings->enterFullscreenWhenLoadingRom());
    m_disableScreensaverCheck->setChecked(appSettings->disableScreensaver());
    m_enableDiscordCheck->setChecked(appSettings->enableDiscord());
    m_hideAdvancedSettingsCheck->setChecked(appSettings->hideAdvancedSettings());
    m_maxRomsSpinBox->setValue(appSettings->maxRecentRoms());
    m_maxRomDirsSpinBox->setValue(appSettings->maxRecentRomDirs());
    
    updateUI();
}

void GeneralSettingsPage::saveSettings()
{
    auto& settings = SettingsManager::instance();
    auto* appSettings = settings.application();
    
    appSettings->setPauseWhenInactive(m_pauseWhenInactiveCheck->isChecked());
    appSettings->setEnterFullscreenWhenLoadingRom(m_enterFullscreenCheck->isChecked());
    appSettings->setDisableScreensaver(m_disableScreensaverCheck->isChecked());
    appSettings->setEnableDiscord(m_enableDiscordCheck->isChecked());
    appSettings->setHideAdvancedSettings(m_hideAdvancedSettingsCheck->isChecked());
    appSettings->setMaxRecentRoms(m_maxRomsSpinBox->value());
    appSettings->setMaxRecentRomDirs(m_maxRomDirsSpinBox->value());
}

void GeneralSettingsPage::resetSettings()
{
    m_pauseWhenInactiveCheck->setChecked(true);
    m_enterFullscreenCheck->setChecked(false);
    m_disableScreensaverCheck->setChecked(true);
    m_enableDiscordCheck->setChecked(true);
    m_hideAdvancedSettingsCheck->setChecked(false);
    m_maxRomsSpinBox->setValue(10);
    m_maxRomDirsSpinBox->setValue(10);
    
    updateUI();
    emit settingsChanged();
}

void GeneralSettingsPage::updateUI()
{
    m_advancedGroup->setVisible(!m_hideAdvancedSettingsCheck->isChecked());
}

} // namespace QT_UI
