#include "GraphicsSettingsDialog.h"
#include <Core/Settings/SettingsManager.h>
#include <Core/Settings/GraphicsSettings.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFormLayout>
#include <QMessageBox>
#include <QDebug>
#include <QPushButton>

namespace QT_UI {

GraphicsSettingsDialog::GraphicsSettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    // Set window properties
    setWindowTitle(tr("Graphics Settings"));
    setWindowIcon(QIcon(":/icons/graphics.png"));
    resize(650, 550);
    
    // Get settings reference
    m_settings = SettingsManager::instance().graphics();
    
    // Setup the UI
    setupUI();
    
    // Load current settings
    loadSettings();
    
    // Connect signals
    connect(m_resolutionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GraphicsSettingsDialog::onResolutionChanged);
    connect(m_aspectRatioCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GraphicsSettingsDialog::onAspectRatioChanged);
    connect(m_textureFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GraphicsSettingsDialog::onTextureFilterChanged);
    connect(m_enableBloomCheck, &QCheckBox::toggled,
            this, &GraphicsSettingsDialog::updateBloomControls);
    connect(m_gammaCorrectionCheck, &QCheckBox::toggled,
            this, &GraphicsSettingsDialog::updateGammaControls);
}

void GraphicsSettingsDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Create tab widget
    m_tabWidget = new QTabWidget(this);
    
    // Create tabs
    createGeneralTab();
    createTextureTab();
    createEnhancementsTab();
    createAdvancedTab();
    
    // Add button box
    m_buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply | QDialogButtonBox::Reset,
        this);
    
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        saveSettings();
        accept();
    });
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &GraphicsSettingsDialog::applySettings);
    connect(m_buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked,
            this, &GraphicsSettingsDialog::resetSettings);
    
    // Add widgets to main layout
    mainLayout->addWidget(m_tabWidget);
    mainLayout->addWidget(m_buttonBox);
}

void GraphicsSettingsDialog::createGeneralTab()
{
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    
    // Resolution group
    QGroupBox* resolutionGroup = new QGroupBox(tr("Resolution"), tab);
    QVBoxLayout* resolutionLayout = new QVBoxLayout(resolutionGroup);
    
    // Resolution combo
    QHBoxLayout* resolutionComboLayout = new QHBoxLayout();
    QLabel* resolutionLabel = new QLabel(tr("Resolution:"), resolutionGroup);
    m_resolutionCombo = new QComboBox(resolutionGroup);
    
    m_resolutionCombo->addItem(tr("Native (320x240)"), static_cast<int>(GraphicsSettings::Resolution::Native));
    m_resolutionCombo->addItem(tr("640x480"), static_cast<int>(GraphicsSettings::Resolution::Resolution640x480));
    m_resolutionCombo->addItem(tr("1024x768"), static_cast<int>(GraphicsSettings::Resolution::Resolution1024x768));
    m_resolutionCombo->addItem(tr("1280x960"), static_cast<int>(GraphicsSettings::Resolution::Resolution1280x960));
    m_resolutionCombo->addItem(tr("1440x1080"), static_cast<int>(GraphicsSettings::Resolution::Resolution1440x1080));
    m_resolutionCombo->addItem(tr("1600x1200"), static_cast<int>(GraphicsSettings::Resolution::Resolution1600x1200));
    m_resolutionCombo->addItem(tr("1920x1440"), static_cast<int>(GraphicsSettings::Resolution::Resolution1920x1440));
    m_resolutionCombo->addItem(tr("2048x1536"), static_cast<int>(GraphicsSettings::Resolution::Resolution2048x1536));
    m_resolutionCombo->addItem(tr("2880x2160"), static_cast<int>(GraphicsSettings::Resolution::Resolution2880x2160));
    m_resolutionCombo->addItem(tr("3840x2880"), static_cast<int>(GraphicsSettings::Resolution::Resolution3840x2880));
    m_resolutionCombo->addItem(tr("Custom..."), static_cast<int>(GraphicsSettings::Resolution::Custom));
    
    resolutionComboLayout->addWidget(resolutionLabel);
    resolutionComboLayout->addWidget(m_resolutionCombo);
    resolutionLayout->addLayout(resolutionComboLayout);
    
    // Custom resolution controls
    QHBoxLayout* customResLayout = new QHBoxLayout();
    QLabel* customResLabel = new QLabel(tr("Custom Resolution:"), resolutionGroup);
    m_customWidthSpin = new QSpinBox(resolutionGroup);
    m_customWidthSpin->setMinimum(320);
    m_customWidthSpin->setMaximum(7680);
    m_customWidthSpin->setSingleStep(8);
    
    QLabel* xLabel = new QLabel("x", resolutionGroup);
    
    m_customHeightSpin = new QSpinBox(resolutionGroup);
    m_customHeightSpin->setMinimum(240);
    m_customHeightSpin->setMaximum(4320);
    m_customHeightSpin->setSingleStep(8);
    
    customResLayout->addWidget(customResLabel);
    customResLayout->addWidget(m_customWidthSpin);
    customResLayout->addWidget(xLabel);
    customResLayout->addWidget(m_customHeightSpin);
    customResLayout->addStretch();
    
    resolutionLayout->addLayout(customResLayout);
    
    // Aspect ratio
    QHBoxLayout* aspectRatioLayout = new QHBoxLayout();
    QLabel* aspectRatioLabel = new QLabel(tr("Aspect Ratio:"), resolutionGroup);
    m_aspectRatioCombo = new QComboBox(resolutionGroup);
    
    m_aspectRatioCombo->addItem(tr("Native (4:3)"), static_cast<int>(GraphicsSettings::AspectRatio::Native));
    m_aspectRatioCombo->addItem(tr("Widescreen (16:9)"), static_cast<int>(GraphicsSettings::AspectRatio::SixteenToNine));
    m_aspectRatioCombo->addItem(tr("Stretch"), static_cast<int>(GraphicsSettings::AspectRatio::Stretched));
    m_aspectRatioCombo->addItem(tr("Custom..."), static_cast<int>(GraphicsSettings::AspectRatio::Custom));
    
    aspectRatioLayout->addWidget(aspectRatioLabel);
    aspectRatioLayout->addWidget(m_aspectRatioCombo);
    aspectRatioLayout->addStretch();
    
    resolutionLayout->addLayout(aspectRatioLayout);
    
    // Custom aspect ratio
    QHBoxLayout* customAspectLayout = new QHBoxLayout();
    QLabel* customAspectLabel = new QLabel(tr("Custom Aspect Ratio:"), resolutionGroup);
    m_customAspectRatioSpin = new QDoubleSpinBox(resolutionGroup);
    m_customAspectRatioSpin->setMinimum(0.5);
    m_customAspectRatioSpin->setMaximum(3.0);
    m_customAspectRatioSpin->setSingleStep(0.01);
    m_customAspectRatioSpin->setValue(1.33333);
    
    customAspectLayout->addWidget(customAspectLabel);
    customAspectLayout->addWidget(m_customAspectRatioSpin);
    customAspectLayout->addStretch();
    
    resolutionLayout->addLayout(customAspectLayout);
    layout->addWidget(resolutionGroup);
    
    // Display options group
    QGroupBox* displayGroup = new QGroupBox(tr("Display Options"), tab);
    QVBoxLayout* displayLayout = new QVBoxLayout(displayGroup);
    
    // VSync
    m_vsyncCheck = new QCheckBox(tr("Enable VSync"), displayGroup);
    displayLayout->addWidget(m_vsyncCheck);
    
    // Anti-aliasing options
    QHBoxLayout* aaLayout = new QHBoxLayout();
    QLabel* aaLabel = new QLabel(tr("Anti-aliasing:"), displayGroup);
    m_antiAliasingCombo = new QComboBox(displayGroup);
    
    m_antiAliasingCombo->addItem(tr("None"), static_cast<int>(GraphicsSettings::AntiAliasing::None));
    m_antiAliasingCombo->addItem(tr("MSAA 2x"), static_cast<int>(GraphicsSettings::AntiAliasing::MSAA2x));
    m_antiAliasingCombo->addItem(tr("MSAA 4x"), static_cast<int>(GraphicsSettings::AntiAliasing::MSAA4x));
    m_antiAliasingCombo->addItem(tr("MSAA 8x"), static_cast<int>(GraphicsSettings::AntiAliasing::MSAA8x));
    m_antiAliasingCombo->addItem(tr("MSAA 16x"), static_cast<int>(GraphicsSettings::AntiAliasing::MSAA16x));
    
    aaLayout->addWidget(aaLabel);
    aaLayout->addWidget(m_antiAliasingCombo);
    aaLayout->addStretch();
    
    displayLayout->addLayout(aaLayout);
    
    // Fullscreen anti-aliasing
    m_fullscreenAACheck = new QCheckBox(tr("Enable fullscreen anti-aliasing"), displayGroup);
    displayLayout->addWidget(m_fullscreenAACheck);
    
    // Frame limiting
    QHBoxLayout* frameLimitLayout = new QHBoxLayout();
    QLabel* frameLimitLabel = new QLabel(tr("Frame limit (FPS):"), displayGroup);
    m_frameLimitSpin = new QSpinBox(displayGroup);
    m_frameLimitSpin->setMinimum(10);
    m_frameLimitSpin->setMaximum(240);
    m_frameLimitSpin->setValue(60);
    
    frameLimitLayout->addWidget(frameLimitLabel);
    frameLimitLayout->addWidget(m_frameLimitSpin);
    frameLimitLayout->addStretch();
    
    displayLayout->addLayout(frameLimitLayout);
    
    // Skip frame
    m_skipFrameCheck = new QCheckBox(tr("Skip frame (faster but less smooth)"), displayGroup);
    displayLayout->addWidget(m_skipFrameCheck);
    
    layout->addWidget(displayGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(tab, tr("General"));
}

void GraphicsSettingsDialog::createTextureTab()
{
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    
    // Texture filtering group
    QGroupBox* filteringGroup = new QGroupBox(tr("Texture Filtering"), tab);
    QVBoxLayout* filteringLayout = new QVBoxLayout(filteringGroup);
    
    // Texture filter type
    QHBoxLayout* filterTypeLayout = new QHBoxLayout();
    QLabel* filterTypeLabel = new QLabel(tr("Filter:"), filteringGroup);
    m_textureFilterCombo = new QComboBox(filteringGroup);
    
    m_textureFilterCombo->addItem(tr("None (Nearest)"), static_cast<int>(GraphicsSettings::TextureFilter::None));
    m_textureFilterCombo->addItem(tr("Bilinear"), static_cast<int>(GraphicsSettings::TextureFilter::Bilinear));
    m_textureFilterCombo->addItem(tr("Trilinear"), static_cast<int>(GraphicsSettings::TextureFilter::Trilinear));
    m_textureFilterCombo->addItem(tr("Anisotropic Filtering"), static_cast<int>(GraphicsSettings::TextureFilter::AnisotropicFiltering));
    
    filterTypeLayout->addWidget(filterTypeLabel);
    filterTypeLayout->addWidget(m_textureFilterCombo);
    filterTypeLayout->addStretch();
    
    filteringLayout->addLayout(filterTypeLayout);
    
    // Anisotropic level
    QHBoxLayout* anisoLayout = new QHBoxLayout();
    QLabel* anisoLabel = new QLabel(tr("Anisotropic Level:"), filteringGroup);
    m_anisotropicLevelSpin = new QSpinBox(filteringGroup);
    m_anisotropicLevelSpin->setMinimum(2);
    m_anisotropicLevelSpin->setMaximum(16);
    m_anisotropicLevelSpin->setValue(4);
    
    anisoLayout->addWidget(anisoLabel);
    anisoLayout->addWidget(m_anisotropicLevelSpin);
    anisoLayout->addStretch();
    
    filteringLayout->addLayout(anisoLayout);
    
    // Mipmapping
    m_mipmappingCheck = new QCheckBox(tr("Enable mipmapping"), filteringGroup);
    filteringLayout->addWidget(m_mipmappingCheck);
    
    layout->addWidget(filteringGroup);
    
    // Texture enhancement group
    QGroupBox* enhanceGroup = new QGroupBox(tr("Texture Enhancement"), tab);
    QVBoxLayout* enhanceLayout = new QVBoxLayout(enhanceGroup);
    
    // High resolution textures
    m_highResTexturesCheck = new QCheckBox(tr("Use high resolution texture packs if available"), enhanceGroup);
    enhanceLayout->addWidget(m_highResTexturesCheck);
    
    // Fast texture CRC
    m_fastTextureCRCCheck = new QCheckBox(tr("Fast texture CRC (less accurate)"), enhanceGroup);
    enhanceLayout->addWidget(m_fastTextureCRCCheck);
    
    layout->addWidget(enhanceGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(tab, tr("Textures"));
}

void GraphicsSettingsDialog::createEnhancementsTab()
{
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    
    // Visual enhancements group
    QGroupBox* enhancementsGroup = new QGroupBox(tr("Visual Enhancements"), tab);
    QVBoxLayout* enhancementsLayout = new QVBoxLayout(enhancementsGroup);
    
    // Fog controls
    m_fogEnabledCheck = new QCheckBox(tr("Enable fog emulation"), enhancementsGroup);
    m_enhancedFogCheck = new QCheckBox(tr("Use enhanced fog"), enhancementsGroup);
    
    enhancementsLayout->addWidget(m_fogEnabledCheck);
    enhancementsLayout->addWidget(m_enhancedFogCheck);
    
    // Perspective correction
    m_perspectiveCorrectionCheck = new QCheckBox(tr("Enable perspective texture correction"), enhancementsGroup);
    enhancementsLayout->addWidget(m_perspectiveCorrectionCheck);
    
    // Point sampling
    m_forcePointSamplingCheck = new QCheckBox(tr("Force point sampling (less blurry, N64-like)"), enhancementsGroup);
    enhancementsLayout->addWidget(m_forcePointSamplingCheck);
    
    // Dithered alpha
    m_ditheredAlphaCheck = new QCheckBox(tr("Use dithered alpha emulation"), enhancementsGroup);
    enhancementsLayout->addWidget(m_ditheredAlphaCheck);
    
    layout->addWidget(enhancementsGroup);
    
    // Post-processing group
    QGroupBox* postProcessGroup = new QGroupBox(tr("Post-processing Effects"), tab);
    QVBoxLayout* postProcessLayout = new QVBoxLayout(postProcessGroup);
    
    // Enable post-processing
    m_postProcessingCheck = new QCheckBox(tr("Enable post-processing effects"), postProcessGroup);
    postProcessLayout->addWidget(m_postProcessingCheck);
    
    // Effect selection
    QHBoxLayout* effectLayout = new QHBoxLayout();
    QLabel* effectLabel = new QLabel(tr("Effect:"), postProcessGroup);
    m_postProcessingEffectCombo = new QComboBox(postProcessGroup);
    
    m_postProcessingEffectCombo->addItem(tr("None"));
    m_postProcessingEffectCombo->addItem(tr("Bloom"));
    m_postProcessingEffectCombo->addItem(tr("CRT"));
    m_postProcessingEffectCombo->addItem(tr("Sharpen"));
    m_postProcessingEffectCombo->addItem(tr("Scanlines"));
    
    effectLayout->addWidget(effectLabel);
    effectLayout->addWidget(m_postProcessingEffectCombo);
    effectLayout->addStretch();
    
    postProcessLayout->addLayout(effectLayout);
    
    layout->addWidget(postProcessGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(tab, tr("Enhancements"));
}

void GraphicsSettingsDialog::createAdvancedTab()
{
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    
    // Advanced display options
    QGroupBox* advancedGroup = new QGroupBox(tr("Advanced Display Options"), tab);
    QVBoxLayout* advancedLayout = new QVBoxLayout(advancedGroup);
    
    // VI processing
    m_disableVIProcessingCheck = new QCheckBox(tr("Disable video interface processing"), advancedGroup);
    advancedLayout->addWidget(m_disableVIProcessingCheck);
    
    // Deinterlacing
    m_weaveDeinterlacingCheck = new QCheckBox(tr("Use weave deinterlacing"), advancedGroup);
    advancedLayout->addWidget(m_weaveDeinterlacingCheck);
    
    // Overlay
    m_enableOverlayCheck = new QCheckBox(tr("Enable on-screen display"), advancedGroup);
    advancedLayout->addWidget(m_enableOverlayCheck);
    
    // FPS counter
    m_enableFPSCounterCheck = new QCheckBox(tr("Show FPS counter"), advancedGroup);
    advancedLayout->addWidget(m_enableFPSCounterCheck);
    
    // Color interface options
    m_enableColorMaskCheck = new QCheckBox(tr("Enable color mask"), advancedGroup);
    advancedLayout->addWidget(m_enableColorMaskCheck);
    
    m_enableCICheck = new QCheckBox(tr("Enable color interface"), advancedGroup);
    advancedLayout->addWidget(m_enableCICheck);
    
    layout->addWidget(advancedGroup);
    
    // Gamma correction group
    QGroupBox* gammaGroup = new QGroupBox(tr("Gamma Correction"), tab);
    QVBoxLayout* gammaLayout = new QVBoxLayout(gammaGroup);
    
    // Gamma correction checkbox
    m_gammaCorrectionCheck = new QCheckBox(tr("Enable gamma correction"), gammaGroup);
    gammaLayout->addWidget(m_gammaCorrectionCheck);
    
    // Gamma value slider
    QHBoxLayout* gammaValueLayout = new QHBoxLayout();
    QLabel* gammaValueLabel = new QLabel(tr("Gamma value:"), gammaGroup);
    m_gammaValueSpin = new QDoubleSpinBox(gammaGroup);
    m_gammaValueSpin->setMinimum(0.1);
    m_gammaValueSpin->setMaximum(3.0);
    m_gammaValueSpin->setSingleStep(0.1);
    m_gammaValueSpin->setValue(1.0);
    
    gammaValueLayout->addWidget(gammaValueLabel);
    gammaValueLayout->addWidget(m_gammaValueSpin);
    gammaValueLayout->addStretch();
    
    gammaLayout->addLayout(gammaValueLayout);
    
    layout->addWidget(gammaGroup);
    
    // Bloom group
    QGroupBox* bloomGroup = new QGroupBox(tr("Bloom Effect"), tab);
    QVBoxLayout* bloomLayout = new QVBoxLayout(bloomGroup);
    
    // Bloom checkbox
    m_enableBloomCheck = new QCheckBox(tr("Enable bloom effect"), bloomGroup);
    bloomLayout->addWidget(m_enableBloomCheck);
    
    // Bloom intensity
    QHBoxLayout* bloomIntensityLayout = new QHBoxLayout();
    QLabel* bloomIntensityLabel = new QLabel(tr("Intensity:"), bloomGroup);
    m_bloomIntensitySpin = new QDoubleSpinBox(bloomGroup);
    m_bloomIntensitySpin->setMinimum(0.1);
    m_bloomIntensitySpin->setMaximum(2.0);
    m_bloomIntensitySpin->setSingleStep(0.1);
    m_bloomIntensitySpin->setValue(0.5);
    
    bloomIntensityLayout->addWidget(bloomIntensityLabel);
    bloomIntensityLayout->addWidget(m_bloomIntensitySpin);
    bloomIntensityLayout->addStretch();
    
    bloomLayout->addLayout(bloomIntensityLayout);
    
    // Bloom blend mode
    QHBoxLayout* bloomBlendLayout = new QHBoxLayout();
    QLabel* bloomBlendLabel = new QLabel(tr("Blend mode:"), bloomGroup);
    m_bloomBlendModeSpin = new QDoubleSpinBox(bloomGroup);
    m_bloomBlendModeSpin->setMinimum(0.0);
    m_bloomBlendModeSpin->setMaximum(1.0);
    m_bloomBlendModeSpin->setSingleStep(0.1);
    m_bloomBlendModeSpin->setValue(0.0);
    
    bloomBlendLayout->addWidget(bloomBlendLabel);
    bloomBlendLayout->addWidget(m_bloomBlendModeSpin);
    bloomBlendLayout->addStretch();
    
    bloomLayout->addLayout(bloomBlendLayout);
    
    layout->addWidget(bloomGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(tab, tr("Advanced"));
}

void GraphicsSettingsDialog::loadSettings()
{
    // Resolution settings
    m_resolutionCombo->setCurrentIndex(static_cast<int>(m_settings->resolution()));
    
    QSize customRes = m_settings->customResolution();
    m_customWidthSpin->setValue(customRes.width());
    m_customHeightSpin->setValue(customRes.height());
    
    m_aspectRatioCombo->setCurrentIndex(static_cast<int>(m_settings->aspectRatio()));
    m_customAspectRatioSpin->setValue(m_settings->customAspectRatio());
    
    // Display settings
    m_vsyncCheck->setChecked(m_settings->vsync());
    m_antiAliasingCombo->setCurrentIndex(static_cast<int>(m_settings->antiAliasing()));
    m_fullscreenAACheck->setChecked(m_settings->fullscreenAntiAliasing());
    m_frameLimitSpin->setValue(m_settings->frameLimit());
    m_skipFrameCheck->setChecked(m_settings->skipFrame());
    
    // Texture settings
    m_textureFilterCombo->setCurrentIndex(static_cast<int>(m_settings->textureFilter()));
    m_anisotropicLevelSpin->setValue(m_settings->anisotropicLevel());
    m_mipmappingCheck->setChecked(m_settings->mipmapping());
    m_highResTexturesCheck->setChecked(m_settings->highResTextures());
    m_fastTextureCRCCheck->setChecked(m_settings->fastTextureCRC());
    
    // Enhancement settings
    m_fogEnabledCheck->setChecked(m_settings->fogEnabled());
    m_enhancedFogCheck->setChecked(m_settings->enhancedFog());
    m_perspectiveCorrectionCheck->setChecked(m_settings->perspectiveCorrection());
    m_forcePointSamplingCheck->setChecked(m_settings->forcePointSampling());
    m_ditheredAlphaCheck->setChecked(m_settings->ditheredAlpha());
    
    // Post-processing
    m_postProcessingCheck->setChecked(m_settings->postProcessing());
    m_postProcessingEffectCombo->setCurrentText(m_settings->postProcessingEffect());
    
    // Advanced settings
    m_disableVIProcessingCheck->setChecked(m_settings->disableVideoInterfaceProcessing());
    m_weaveDeinterlacingCheck->setChecked(m_settings->weaveDeinterlacing());
    m_enableOverlayCheck->setChecked(m_settings->enableOverlay());
    m_enableFPSCounterCheck->setChecked(m_settings->enableFPSCounter());
    m_gammaCorrectionCheck->setChecked(m_settings->gammaCorrection());
    m_gammaValueSpin->setValue(m_settings->gammaValue());
    m_enableBloomCheck->setChecked(m_settings->enableBloom());
    m_bloomIntensitySpin->setValue(m_settings->bloomIntensity());
    m_bloomBlendModeSpin->setValue(m_settings->bloomBlendMode());
    m_enableColorMaskCheck->setChecked(m_settings->enableColorMask());
    m_enableCICheck->setChecked(m_settings->enableCI());
    
    // Update UI state
    updateCustomResolutionControls();
    updateCustomAspectRatioControls();
    updateAnisotropicControls();
    updateGammaControls();
    updateBloomControls();
}

void GraphicsSettingsDialog::saveSettings()
{
    // Resolution settings
    m_settings->setResolution(static_cast<GraphicsSettings::Resolution>(m_resolutionCombo->currentIndex()));
    m_settings->setCustomResolution(QSize(m_customWidthSpin->value(), m_customHeightSpin->value()));
    m_settings->setAspectRatio(static_cast<GraphicsSettings::AspectRatio>(m_aspectRatioCombo->currentIndex()));
    m_settings->setCustomAspectRatio(m_customAspectRatioSpin->value());
    
    // Display settings
    m_settings->setVsync(m_vsyncCheck->isChecked());
    m_settings->setAntiAliasing(static_cast<GraphicsSettings::AntiAliasing>(m_antiAliasingCombo->currentIndex()));
    m_settings->setFullscreenAntiAliasing(m_fullscreenAACheck->isChecked());
    m_settings->setFrameLimit(m_frameLimitSpin->value());
    m_settings->setSkipFrame(m_skipFrameCheck->isChecked());
    
    // Texture settings
    m_settings->setTextureFilter(static_cast<GraphicsSettings::TextureFilter>(m_textureFilterCombo->currentIndex()));
    m_settings->setAnisotropicLevel(m_anisotropicLevelSpin->value());
    m_settings->setMipmapping(m_mipmappingCheck->isChecked());
    m_settings->setHighResTextures(m_highResTexturesCheck->isChecked());
    m_settings->setFastTextureCRC(m_fastTextureCRCCheck->isChecked());
    
    // Enhancement settings
    m_settings->setFogEnabled(m_fogEnabledCheck->isChecked());
    m_settings->setEnhancedFog(m_enhancedFogCheck->isChecked());
    m_settings->setPerspectiveCorrection(m_perspectiveCorrectionCheck->isChecked());
    m_settings->setForcePointSampling(m_forcePointSamplingCheck->isChecked());
    m_settings->setDitheredAlpha(m_ditheredAlphaCheck->isChecked());
    
    // Post-processing
    m_settings->setPostProcessing(m_postProcessingCheck->isChecked());
    m_settings->setPostProcessingEffect(m_postProcessingEffectCombo->currentText());
    
    // Advanced settings
    m_settings->setDisableVideoInterfaceProcessing(m_disableVIProcessingCheck->isChecked());
    m_settings->setWeaveDeinterlacing(m_weaveDeinterlacingCheck->isChecked());
    m_settings->setEnableOverlay(m_enableOverlayCheck->isChecked());
    m_settings->setEnableFPSCounter(m_enableFPSCounterCheck->isChecked());
    m_settings->setGammaCorrection(m_gammaCorrectionCheck->isChecked());
    m_settings->setGammaValue(m_gammaValueSpin->value());
    m_settings->setEnableBloom(m_enableBloomCheck->isChecked());
    m_settings->setBloomIntensity(m_bloomIntensitySpin->value());
    m_settings->setBloomBlendMode(m_bloomBlendModeSpin->value());
    m_settings->setEnableColorMask(m_enableColorMaskCheck->isChecked());
    m_settings->setEnableCI(m_enableCICheck->isChecked());
}

void GraphicsSettingsDialog::applySettings()
{
    saveSettings();
    QMessageBox::information(this, tr("Settings Applied"), tr("Graphics settings have been applied."));
}

void GraphicsSettingsDialog::resetSettings()
{
    // Reset to default values - hardcoded here for simplicity
    m_resolutionCombo->setCurrentIndex(static_cast<int>(GraphicsSettings::Resolution::Resolution640x480));
    m_customWidthSpin->setValue(640);
    m_customHeightSpin->setValue(480);
    m_aspectRatioCombo->setCurrentIndex(static_cast<int>(GraphicsSettings::AspectRatio::Native));
    m_customAspectRatioSpin->setValue(1.33333);
    
    m_vsyncCheck->setChecked(true);
    m_antiAliasingCombo->setCurrentIndex(static_cast<int>(GraphicsSettings::AntiAliasing::None));
    m_fullscreenAACheck->setChecked(false);
    m_frameLimitSpin->setValue(60);
    m_skipFrameCheck->setChecked(false);
    
    m_textureFilterCombo->setCurrentIndex(static_cast<int>(GraphicsSettings::TextureFilter::None));
    m_anisotropicLevelSpin->setValue(2);
    m_mipmappingCheck->setChecked(true);
    m_highResTexturesCheck->setChecked(false);
    m_fastTextureCRCCheck->setChecked(true);
    
    m_fogEnabledCheck->setChecked(true);
    m_enhancedFogCheck->setChecked(false);
    m_perspectiveCorrectionCheck->setChecked(true);
    m_forcePointSamplingCheck->setChecked(false);
    m_ditheredAlphaCheck->setChecked(false);
    
    m_postProcessingCheck->setChecked(false);
    m_postProcessingEffectCombo->setCurrentIndex(0);
    
    m_disableVIProcessingCheck->setChecked(false);
    m_weaveDeinterlacingCheck->setChecked(false);
    m_enableOverlayCheck->setChecked(true);
    m_enableFPSCounterCheck->setChecked(false);
    m_gammaCorrectionCheck->setChecked(false);
    m_gammaValueSpin->setValue(1.0);
    m_enableBloomCheck->setChecked(false);
    m_bloomIntensitySpin->setValue(0.5);
    m_bloomBlendModeSpin->setValue(0.0);
    m_enableColorMaskCheck->setChecked(true);
    m_enableCICheck->setChecked(true);
    
    // Update UI state
    updateCustomResolutionControls();
    updateCustomAspectRatioControls();
    updateAnisotropicControls();
    updateGammaControls();
    updateBloomControls();
}

void GraphicsSettingsDialog::onResolutionChanged(int index)
{
    updateCustomResolutionControls();
}

void GraphicsSettingsDialog::onTextureFilterChanged(int index)
{
    updateAnisotropicControls();
}

void GraphicsSettingsDialog::onAspectRatioChanged(int index)
{
    updateCustomAspectRatioControls();
}

void GraphicsSettingsDialog::updateCustomResolutionControls()
{
    bool isCustom = m_resolutionCombo->currentIndex() == static_cast<int>(GraphicsSettings::Resolution::Custom);
    m_customWidthSpin->setEnabled(isCustom);
    m_customHeightSpin->setEnabled(isCustom);
}

void GraphicsSettingsDialog::updateCustomAspectRatioControls()
{
    bool isCustom = m_aspectRatioCombo->currentIndex() == static_cast<int>(GraphicsSettings::AspectRatio::Custom);
    m_customAspectRatioSpin->setEnabled(isCustom);
}

void GraphicsSettingsDialog::updateAnisotropicControls()
{
    bool isAnisotropic = m_textureFilterCombo->currentIndex() == static_cast<int>(GraphicsSettings::TextureFilter::AnisotropicFiltering);
    m_anisotropicLevelSpin->setEnabled(isAnisotropic);
}

void GraphicsSettingsDialog::updateBloomControls()
{
    bool enabled = m_enableBloomCheck->isChecked();
    m_bloomIntensitySpin->setEnabled(enabled);
    m_bloomBlendModeSpin->setEnabled(enabled);
}

void GraphicsSettingsDialog::updateGammaControls()
{
    m_gammaValueSpin->setEnabled(m_gammaCorrectionCheck->isChecked());
}

} // namespace QT_UI
