#pragma once

#include <QObject>
#include <QString>

namespace QT_UI {

class SettingsManager;

/**
 * @brief Settings related to N64 audio emulation
 * 
 * This class manages settings for the audio emulation,
 * including audio buffer, synchronization, and effects.
 */
class AudioSettings : public QObject
{
    Q_OBJECT

public:
    explicit AudioSettings(QObject* parent = nullptr);

    // Audio backend
    enum class Backend {
        Default,
        OpenAL,
        DirectSound,
        XAudio2,
        SDLAudio,
        ALSA,
        PulseAudio
    };
    Q_ENUM(Backend)

    // General audio settings
    bool audioEnabled() const;
    Backend audioBackend() const;
    bool synchronize() const;
    int bufferSize() const;
    int bufferCount() const;
    int volume() const;
    float balance() const;
    bool swapChannels() const;
    int sampleRate() const;

    // Audio processing
    bool resampling() const;
    bool timeStretch() const;
    bool enhancedAudio() const;
    bool enhancedStereo() const;
    float stereoSeparation() const;
    bool lowPassFilter() const;
    bool reverb() const;
    int reverbStrength() const;

    // Setters
    void setAudioEnabled(bool enabled);
    void setAudioBackend(Backend backend);
    void setSynchronize(bool sync);
    void setBufferSize(int size);
    void setBufferCount(int count);
    void setVolume(int volume);
    void setBalance(float balance);
    void setSwapChannels(bool swap);
    void setSampleRate(int rate);
    void setResampling(bool enable);
    void setTimeStretch(bool enable);
    void setEnhancedAudio(bool enable);
    void setEnhancedStereo(bool enable);
    void setStereoSeparation(float separation);
    void setLowPassFilter(bool enable);
    void setReverb(bool enable);
    void setReverbStrength(int strength);

signals:
    void settingsChanged();
    void volumeChanged();
    void backendChanged();

private:
    friend class SettingsManager;
    
    void loadSettings();
    void saveSettings();
};

} // namespace QT_UI
