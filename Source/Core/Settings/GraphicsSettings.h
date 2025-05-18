#pragma once

#include <QObject>
#include <QString>
#include <QSize>

namespace QT_UI {

class SettingsManager;

/**
 * @brief Settings related to N64 graphics emulation
 * 
 * This class manages all settings related to graphics rendering
 * for N64 emulation, including resolution, texture filtering,
 * and other visual enhancements.
 */
class GraphicsSettings : public QObject
{
    Q_OBJECT

public:
    explicit GraphicsSettings(QObject* parent = nullptr);

    // Resolution settings
    enum class Resolution {
        Native,     // 320x240
        Resolution640x480,
        Resolution1024x768,
        Resolution1280x960,
        Resolution1440x1080,
        Resolution1600x1200,
        Resolution1920x1440,
        Resolution2048x1536,
        Resolution2880x2160,
        Resolution3840x2880,
        Custom
    };
    Q_ENUM(Resolution)

    // Texture filtering options
    enum class TextureFilter {
        None,
        Bilinear,
        Trilinear,
        AnisotropicFiltering
    };
    Q_ENUM(TextureFilter)

    // Anti-aliasing options
    enum class AntiAliasing {
        None,
        MSAA2x,
        MSAA4x,
        MSAA8x,
        MSAA16x
    };
    Q_ENUM(AntiAliasing)

    // Aspect ratio options
    enum class AspectRatio {
        Native,         // 4:3
        SixteenToNine,  // 16:9
        Stretched,      // Stretch to window size
        Custom          // Custom defined ratio
    };
    Q_ENUM(AspectRatio)

    // Basic rendering settings
    Resolution resolution() const;
    QSize customResolution() const;
    TextureFilter textureFilter() const;
    int anisotropicLevel() const;
    AntiAliasing antiAliasing() const;
    AspectRatio aspectRatio() const;
    float customAspectRatio() const;
    bool vsync() const;
    bool fullscreenAntiAliasing() const;
    int frameLimit() const;
    bool skipFrame() const;
    bool multisampling() const;

    // Enhancement settings
    bool highResTextures() const;
    bool fastTextureCRC() const;
    bool mipmapping() const;
    bool fogEnabled() const;
    bool enhancedFog() const;
    bool perspectiveCorrection() const;
    bool forcePointSampling() const;
    bool ditheredAlpha() const;
    bool postProcessing() const;
    QString postProcessingEffect() const;

    // N64 specific settings
    bool disableVideoInterfaceProcessing() const;
    bool weaveDeinterlacing() const;
    bool enableOverlay() const;
    bool enableFPSCounter() const;
    bool gammaCorrection() const;
    float gammaValue() const;
    bool enableBloom() const;
    float bloomIntensity() const;
    float bloomBlendMode() const;
    bool enableColorMask() const;
    bool enableCI() const;

    // Setters
    void setResolution(Resolution resolution);
    void setCustomResolution(const QSize& size);
    void setTextureFilter(TextureFilter filter);
    void setAnisotropicLevel(int level);
    void setAntiAliasing(AntiAliasing aa);
    void setAspectRatio(AspectRatio ratio);
    void setCustomAspectRatio(float ratio);
    void setVsync(bool enable);
    void setFullscreenAntiAliasing(bool enable);
    void setFrameLimit(int limit);
    void setSkipFrame(bool skip);
    void setMultisampling(bool enable);
    void setHighResTextures(bool enable);
    void setFastTextureCRC(bool enable);
    void setMipmapping(bool enable);
    void setFogEnabled(bool enable);
    void setEnhancedFog(bool enable);
    void setPerspectiveCorrection(bool enable);
    void setForcePointSampling(bool enable);
    void setDitheredAlpha(bool enable);
    void setPostProcessing(bool enable);
    void setPostProcessingEffect(const QString& effect);
    void setDisableVideoInterfaceProcessing(bool disable);
    void setWeaveDeinterlacing(bool enable);
    void setEnableOverlay(bool enable);
    void setEnableFPSCounter(bool enable);
    void setGammaCorrection(bool enable);
    void setGammaValue(float value);
    void setEnableBloom(bool enable);
    void setBloomIntensity(float intensity);
    void setBloomBlendMode(float mode);
    void setEnableColorMask(bool enable);
    void setEnableCI(bool enable);

signals:
    void settingsChanged();

private:
    friend class SettingsManager;
    
    void loadSettings();
    void saveSettings();
};

} // namespace QT_UI
