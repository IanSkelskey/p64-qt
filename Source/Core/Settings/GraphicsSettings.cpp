#include "GraphicsSettings.h"
#include "SettingsManager.h"
#include <QDebug>

namespace QT_UI {

GraphicsSettings::GraphicsSettings(QObject* parent)
    : QObject(parent)
{
    loadSettings();
}

// Resolution settings
GraphicsSettings::Resolution GraphicsSettings::resolution() const
{
    return static_cast<Resolution>(SettingsManager::instance().value(
        "Graphics/Resolution", 
        static_cast<int>(Resolution::Native)).toInt());
}

QSize GraphicsSettings::customResolution() const
{
    return SettingsManager::instance().value(
        "Graphics/CustomResolution", 
        QSize(640, 480)).toSize();
}

GraphicsSettings::TextureFilter GraphicsSettings::textureFilter() const
{
    return static_cast<TextureFilter>(SettingsManager::instance().value(
        "Graphics/TextureFilter", 
        static_cast<int>(TextureFilter::None)).toInt());
}

int GraphicsSettings::anisotropicLevel() const
{
    return SettingsManager::instance().value("Graphics/AnisotropicLevel", 0).toInt();
}

GraphicsSettings::AntiAliasing GraphicsSettings::antiAliasing() const
{
    return static_cast<AntiAliasing>(SettingsManager::instance().value(
        "Graphics/AntiAliasing", 
        static_cast<int>(AntiAliasing::None)).toInt());
}

GraphicsSettings::AspectRatio GraphicsSettings::aspectRatio() const
{
    return static_cast<AspectRatio>(SettingsManager::instance().value(
        "Graphics/AspectRatio", 
        static_cast<int>(AspectRatio::Native)).toInt());
}

float GraphicsSettings::customAspectRatio() const
{
    return SettingsManager::instance().value("Graphics/CustomAspectRatio", 1.33333f).toFloat();
}

bool GraphicsSettings::vsync() const
{
    return SettingsManager::instance().value("Graphics/VSync", true).toBool();
}

bool GraphicsSettings::fullscreenAntiAliasing() const
{
    return SettingsManager::instance().value("Graphics/FullscreenAA", false).toBool();
}

int GraphicsSettings::frameLimit() const
{
    return SettingsManager::instance().value("Graphics/FrameLimit", 60).toInt();
}

bool GraphicsSettings::skipFrame() const
{
    return SettingsManager::instance().value("Graphics/SkipFrame", false).toBool();
}

bool GraphicsSettings::multisampling() const
{
    return SettingsManager::instance().value("Graphics/Multisampling", true).toBool();
}

// Enhancement settings
bool GraphicsSettings::highResTextures() const
{
    return SettingsManager::instance().value("Graphics/HighResTextures", false).toBool();
}

bool GraphicsSettings::fastTextureCRC() const
{
    return SettingsManager::instance().value("Graphics/FastTextureCRC", true).toBool();
}

bool GraphicsSettings::mipmapping() const
{
    return SettingsManager::instance().value("Graphics/Mipmapping", true).toBool();
}

bool GraphicsSettings::fogEnabled() const
{
    return SettingsManager::instance().value("Graphics/FogEnabled", true).toBool();
}

bool GraphicsSettings::enhancedFog() const
{
    return SettingsManager::instance().value("Graphics/EnhancedFog", false).toBool();
}

bool GraphicsSettings::perspectiveCorrection() const
{
    return SettingsManager::instance().value("Graphics/PerspectiveCorrection", true).toBool();
}

bool GraphicsSettings::forcePointSampling() const
{
    return SettingsManager::instance().value("Graphics/ForcePointSampling", false).toBool();
}

bool GraphicsSettings::ditheredAlpha() const
{
    return SettingsManager::instance().value("Graphics/DitheredAlpha", false).toBool();
}

bool GraphicsSettings::postProcessing() const
{
    return SettingsManager::instance().value("Graphics/PostProcessing", false).toBool();
}

QString GraphicsSettings::postProcessingEffect() const
{
    return SettingsManager::instance().value("Graphics/PostProcessingEffect", "None").toString();
}

// N64 specific settings
bool GraphicsSettings::disableVideoInterfaceProcessing() const
{
    return SettingsManager::instance().value("Graphics/DisableVideoInterfaceProcessing", false).toBool();
}

bool GraphicsSettings::weaveDeinterlacing() const
{
    return SettingsManager::instance().value("Graphics/WeaveDeinterlacing", false).toBool();
}

bool GraphicsSettings::enableOverlay() const
{
    return SettingsManager::instance().value("Graphics/EnableOverlay", true).toBool();
}

bool GraphicsSettings::enableFPSCounter() const
{
    return SettingsManager::instance().value("Graphics/EnableFPSCounter", false).toBool();
}

bool GraphicsSettings::gammaCorrection() const
{
    return SettingsManager::instance().value("Graphics/GammaCorrection", false).toBool();
}

float GraphicsSettings::gammaValue() const
{
    return SettingsManager::instance().value("Graphics/GammaValue", 1.0f).toFloat();
}

bool GraphicsSettings::enableBloom() const
{
    return SettingsManager::instance().value("Graphics/EnableBloom", false).toBool();
}

float GraphicsSettings::bloomIntensity() const
{
    return SettingsManager::instance().value("Graphics/BloomIntensity", 0.5f).toFloat();
}

float GraphicsSettings::bloomBlendMode() const
{
    return SettingsManager::instance().value("Graphics/BloomBlendMode", 0.0f).toFloat();
}

bool GraphicsSettings::enableColorMask() const
{
    return SettingsManager::instance().value("Graphics/EnableColorMask", true).toBool();
}

bool GraphicsSettings::enableCI() const
{
    return SettingsManager::instance().value("Graphics/EnableCI", true).toBool();
}

// Setters
void GraphicsSettings::setResolution(Resolution resolution)
{
    if (this->resolution() != resolution) {
        SettingsManager::instance().setValue("Graphics/Resolution", static_cast<int>(resolution));
        emit settingsChanged();
    }
}

void GraphicsSettings::setCustomResolution(const QSize& size)
{
    if (this->customResolution() != size) {
        SettingsManager::instance().setValue("Graphics/CustomResolution", size);
        emit settingsChanged();
    }
}

void GraphicsSettings::setTextureFilter(TextureFilter filter)
{
    if (this->textureFilter() != filter) {
        SettingsManager::instance().setValue("Graphics/TextureFilter", static_cast<int>(filter));
        emit settingsChanged();
    }
}

void GraphicsSettings::setAnisotropicLevel(int level)
{
    if (this->anisotropicLevel() != level) {
        SettingsManager::instance().setValue("Graphics/AnisotropicLevel", level);
        emit settingsChanged();
    }
}

void GraphicsSettings::setAntiAliasing(AntiAliasing aa)
{
    if (this->antiAliasing() != aa) {
        SettingsManager::instance().setValue("Graphics/AntiAliasing", static_cast<int>(aa));
        emit settingsChanged();
    }
}

void GraphicsSettings::setAspectRatio(AspectRatio ratio)
{
    if (this->aspectRatio() != ratio) {
        SettingsManager::instance().setValue("Graphics/AspectRatio", static_cast<int>(ratio));
        emit settingsChanged();
    }
}

void GraphicsSettings::setCustomAspectRatio(float ratio)
{
    if (this->customAspectRatio() != ratio) {
        SettingsManager::instance().setValue("Graphics/CustomAspectRatio", ratio);
        emit settingsChanged();
    }
}

void GraphicsSettings::setVsync(bool enable)
{
    if (this->vsync() != enable) {
        SettingsManager::instance().setValue("Graphics/VSync", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setFullscreenAntiAliasing(bool enable)
{
    if (this->fullscreenAntiAliasing() != enable) {
        SettingsManager::instance().setValue("Graphics/FullscreenAA", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setFrameLimit(int limit)
{
    if (this->frameLimit() != limit) {
        SettingsManager::instance().setValue("Graphics/FrameLimit", limit);
        emit settingsChanged();
    }
}

void GraphicsSettings::setSkipFrame(bool skip)
{
    if (this->skipFrame() != skip) {
        SettingsManager::instance().setValue("Graphics/SkipFrame", skip);
        emit settingsChanged();
    }
}

void GraphicsSettings::setMultisampling(bool enable)
{
    if (this->multisampling() != enable) {
        SettingsManager::instance().setValue("Graphics/Multisampling", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setHighResTextures(bool enable)
{
    if (this->highResTextures() != enable) {
        SettingsManager::instance().setValue("Graphics/HighResTextures", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setFastTextureCRC(bool enable)
{
    if (this->fastTextureCRC() != enable) {
        SettingsManager::instance().setValue("Graphics/FastTextureCRC", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setMipmapping(bool enable)
{
    if (this->mipmapping() != enable) {
        SettingsManager::instance().setValue("Graphics/Mipmapping", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setFogEnabled(bool enable)
{
    if (this->fogEnabled() != enable) {
        SettingsManager::instance().setValue("Graphics/FogEnabled", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setEnhancedFog(bool enable)
{
    if (this->enhancedFog() != enable) {
        SettingsManager::instance().setValue("Graphics/EnhancedFog", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setPerspectiveCorrection(bool enable)
{
    if (this->perspectiveCorrection() != enable) {
        SettingsManager::instance().setValue("Graphics/PerspectiveCorrection", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setForcePointSampling(bool enable)
{
    if (this->forcePointSampling() != enable) {
        SettingsManager::instance().setValue("Graphics/ForcePointSampling", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setDitheredAlpha(bool enable)
{
    if (this->ditheredAlpha() != enable) {
        SettingsManager::instance().setValue("Graphics/DitheredAlpha", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setPostProcessing(bool enable)
{
    if (this->postProcessing() != enable) {
        SettingsManager::instance().setValue("Graphics/PostProcessing", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setPostProcessingEffect(const QString& effect)
{
    if (this->postProcessingEffect() != effect) {
        SettingsManager::instance().setValue("Graphics/PostProcessingEffect", effect);
        emit settingsChanged();
    }
}

void GraphicsSettings::setDisableVideoInterfaceProcessing(bool disable)
{
    if (this->disableVideoInterfaceProcessing() != disable) {
        SettingsManager::instance().setValue("Graphics/DisableVideoInterfaceProcessing", disable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setWeaveDeinterlacing(bool enable)
{
    if (this->weaveDeinterlacing() != enable) {
        SettingsManager::instance().setValue("Graphics/WeaveDeinterlacing", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setEnableOverlay(bool enable)
{
    if (this->enableOverlay() != enable) {
        SettingsManager::instance().setValue("Graphics/EnableOverlay", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setEnableFPSCounter(bool enable)
{
    if (this->enableFPSCounter() != enable) {
        SettingsManager::instance().setValue("Graphics/EnableFPSCounter", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setGammaCorrection(bool enable)
{
    if (this->gammaCorrection() != enable) {
        SettingsManager::instance().setValue("Graphics/GammaCorrection", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setGammaValue(float value)
{
    if (this->gammaValue() != value) {
        SettingsManager::instance().setValue("Graphics/GammaValue", value);
        emit settingsChanged();
    }
}

void GraphicsSettings::setEnableBloom(bool enable)
{
    if (this->enableBloom() != enable) {
        SettingsManager::instance().setValue("Graphics/EnableBloom", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setBloomIntensity(float intensity)
{
    if (this->bloomIntensity() != intensity) {
        SettingsManager::instance().setValue("Graphics/BloomIntensity", intensity);
        emit settingsChanged();
    }
}

void GraphicsSettings::setBloomBlendMode(float mode)
{
    if (this->bloomBlendMode() != mode) {
        SettingsManager::instance().setValue("Graphics/BloomBlendMode", mode);
        emit settingsChanged();
    }
}

void GraphicsSettings::setEnableColorMask(bool enable)
{
    if (this->enableColorMask() != enable) {
        SettingsManager::instance().setValue("Graphics/EnableColorMask", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::setEnableCI(bool enable)
{
    if (this->enableCI() != enable) {
        SettingsManager::instance().setValue("Graphics/EnableCI", enable);
        emit settingsChanged();
    }
}

void GraphicsSettings::loadSettings()
{
    // Load settings from SettingsManager
    // All getters will call SettingsManager::instance().value() directly
}

void GraphicsSettings::saveSettings()
{
    // Save settings to SettingsManager
    // All settings are saved when they are changed via the setters
    SettingsManager::instance().sync();
}

} // namespace QT_UI
