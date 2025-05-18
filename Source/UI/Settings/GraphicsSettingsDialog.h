#pragma once

#include <QDialog>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QGroupBox>
#include <QLineEdit>

namespace QT_UI {

class GraphicsSettings;

/**
 * @brief Dialog for configuring graphics settings
 * 
 * This dialog allows users to configure all graphics-related settings
 * including resolution, texture filtering, and visual enhancements.
 */
class GraphicsSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GraphicsSettingsDialog(QWidget* parent = nullptr);
    ~GraphicsSettingsDialog() = default;

private slots:
    void applySettings();
    void resetSettings();
    void onResolutionChanged(int index);
    void onTextureFilterChanged(int index);
    void onAspectRatioChanged(int index);
    void updateCustomResolutionControls();
    void updateCustomAspectRatioControls();
    void updateAnisotropicControls();
    void updateBloomControls();
    void updateGammaControls();

private:
    void setupUI();
    void loadSettings();
    void saveSettings();
    void createGeneralTab();
    void createTextureTab();
    void createEnhancementsTab();
    void createAdvancedTab();

    // UI Elements
    QTabWidget* m_tabWidget;
    QDialogButtonBox* m_buttonBox;

    // General Tab
    QComboBox* m_resolutionCombo;
    QSpinBox* m_customWidthSpin;
    QSpinBox* m_customHeightSpin;
    QComboBox* m_aspectRatioCombo;
    QDoubleSpinBox* m_customAspectRatioSpin;
    QCheckBox* m_vsyncCheck;
    QCheckBox* m_fullscreenAACheck;
    QComboBox* m_antiAliasingCombo;
    QSpinBox* m_frameLimitSpin;
    QCheckBox* m_skipFrameCheck;

    // Texture Tab
    QComboBox* m_textureFilterCombo;
    QSpinBox* m_anisotropicLevelSpin;
    QCheckBox* m_mipmappingCheck;
    QCheckBox* m_highResTexturesCheck;
    QCheckBox* m_fastTextureCRCCheck;

    // Enhancements Tab
    QCheckBox* m_fogEnabledCheck;
    QCheckBox* m_enhancedFogCheck;
    QCheckBox* m_perspectiveCorrectionCheck;
    QCheckBox* m_forcePointSamplingCheck;
    QCheckBox* m_ditheredAlphaCheck;
    QCheckBox* m_postProcessingCheck;
    QComboBox* m_postProcessingEffectCombo;

    // Advanced Tab
    QCheckBox* m_disableVIProcessingCheck;
    QCheckBox* m_weaveDeinterlacingCheck;
    QCheckBox* m_enableOverlayCheck;
    QCheckBox* m_enableFPSCounterCheck;
    QCheckBox* m_gammaCorrectionCheck;
    QDoubleSpinBox* m_gammaValueSpin;
    QCheckBox* m_enableBloomCheck;
    QDoubleSpinBox* m_bloomIntensitySpin;
    QDoubleSpinBox* m_bloomBlendModeSpin;
    QCheckBox* m_enableColorMaskCheck;
    QCheckBox* m_enableCICheck;
    
    // Pointer to settings instance for convenience
    GraphicsSettings* m_settings;
};

} // namespace QT_UI
