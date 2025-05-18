#pragma once

#include "BaseSettingsPage.h"
#include <QCheckBox>
#include <QSpinBox>
#include <QGroupBox>

namespace QT_UI {

class GeneralSettingsPage : public BaseSettingsPage
{
    Q_OBJECT

public:
    explicit GeneralSettingsPage(QWidget* parent = nullptr);
    ~GeneralSettingsPage() override = default;

    void loadSettings() override;
    void saveSettings() override;
    void resetSettings() override;
    QString pageTitle() const override { return tr("General"); }

private slots:
    void updateUI();

private:
    void setupUI();

    // Application behavior settings
    QCheckBox* m_pauseWhenInactiveCheck;
    QCheckBox* m_enterFullscreenCheck;
    QCheckBox* m_disableScreensaverCheck;
    QCheckBox* m_enableDiscordCheck;
    QCheckBox* m_hideAdvancedSettingsCheck;
    
    // ROM history settings
    QSpinBox* m_maxRomsSpinBox;
    QSpinBox* m_maxRomDirsSpinBox;
    
    // Advanced settings group
    QGroupBox* m_advancedGroup;
};

} // namespace QT_UI
