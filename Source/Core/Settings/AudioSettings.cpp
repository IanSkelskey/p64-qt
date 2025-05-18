#include "AudioSettings.h"
#include "SettingsManager.h"

namespace QT_UI {

AudioSettings::AudioSettings(QObject* parent)
    : QObject(parent)
{
    loadSettings();
}

// General audio settings
bool AudioSettings::audioEnabled() const
{
    return SettingsManager::instance().value("Audio/Enabled", true).toBool();
}

AudioSettings::Backend AudioSettings::audioBackend() const
{
    return static_cast<Backend>(SettingsManager::instance().value(
        "Audio/Backend", 
        static_cast<int>(Backend::Default)).toInt());
}

bool AudioSettings::synchronize() const
{
    return SettingsManager::instance().value("Audio/Synchronize", true).toBool();
}

int AudioSettings::bufferSize() const
{
    return SettingsManager::instance().value("Audio/BufferSize", 2048).toInt();
}

int AudioSettings::bufferCount() const
{
    return SettingsManager::instance().value("Audio/BufferCount", 4).toInt();
}

int AudioSettings::volume() const
{
    return SettingsManager::instance().value("Audio/Volume", 100).toInt();
}

float AudioSettings::balance() const
{
    return SettingsManager::instance().value("Audio/Balance", 0.0f).toFloat();
}

bool AudioSettings::swapChannels() const
{
    return SettingsManager::instance().value("Audio/SwapChannels", false).toBool();
}

int AudioSettings::sampleRate() const
{
    return SettingsManager::instance().value("Audio/SampleRate", 44100).toInt();
}

// Audio processing
bool AudioSettings::resampling() const
{
    return SettingsManager::instance().value("Audio/Resampling", true).toBool();
}

bool AudioSettings::timeStretch() const
{
    return SettingsManager::instance().value("Audio/TimeStretch", false).toBool();
}

bool AudioSettings::enhancedAudio() const
{
    return SettingsManager::instance().value("Audio/EnhancedAudio", false).toBool();
}

bool AudioSettings::enhancedStereo() const
{
    return SettingsManager::instance().value("Audio/EnhancedStereo", false).toBool();
}

float AudioSettings::stereoSeparation() const
{
    return SettingsManager::instance().value("Audio/StereoSeparation", 1.0f).toFloat();
}

bool AudioSettings::lowPassFilter() const
{
    return SettingsManager::instance().value("Audio/LowPassFilter", false).toBool();
}

bool AudioSettings::reverb() const
{
    return SettingsManager::instance().value("Audio/Reverb", false).toBool();
}

int AudioSettings::reverbStrength() const
{
    return SettingsManager::instance().value("Audio/ReverbStrength", 5).toInt();
}

// Setters
void AudioSettings::setAudioEnabled(bool enabled)
{
    if (this->audioEnabled() != enabled) {
        SettingsManager::instance().setValue("Audio/Enabled", enabled);
        emit settingsChanged();
    }
}

void AudioSettings::setAudioBackend(Backend backend)
{
    if (this->audioBackend() != backend) {
        SettingsManager::instance().setValue("Audio/Backend", static_cast<int>(backend));
        emit backendChanged();
        emit settingsChanged();
    }
}

void AudioSettings::setSynchronize(bool sync)
{
    if (this->synchronize() != sync) {
        SettingsManager::instance().setValue("Audio/Synchronize", sync);
        emit settingsChanged();
    }
}

void AudioSettings::setBufferSize(int size)
{
    if (this->bufferSize() != size) {
        SettingsManager::instance().setValue("Audio/BufferSize", size);
        emit settingsChanged();
    }
}

void AudioSettings::setBufferCount(int count)
{
    if (this->bufferCount() != count) {
        SettingsManager::instance().setValue("Audio/BufferCount", count);
        emit settingsChanged();
    }
}

void AudioSettings::setVolume(int volume)
{
    if (this->volume() != volume) {
        SettingsManager::instance().setValue("Audio/Volume", volume);
        emit volumeChanged();
        emit settingsChanged();
    }
}

void AudioSettings::setBalance(float balance)
{
    if (this->balance() != balance) {
        SettingsManager::instance().setValue("Audio/Balance", balance);
        emit volumeChanged();
        emit settingsChanged();
    }
}

void AudioSettings::setSwapChannels(bool swap)
{
    if (this->swapChannels() != swap) {
        SettingsManager::instance().setValue("Audio/SwapChannels", swap);
        emit settingsChanged();
    }
}

void AudioSettings::setSampleRate(int rate)
{
    if (this->sampleRate() != rate) {
        SettingsManager::instance().setValue("Audio/SampleRate", rate);
        emit settingsChanged();
    }
}

void AudioSettings::setResampling(bool enable)
{
    if (this->resampling() != enable) {
        SettingsManager::instance().setValue("Audio/Resampling", enable);
        emit settingsChanged();
    }
}

void AudioSettings::setTimeStretch(bool enable)
{
    if (this->timeStretch() != enable) {
        SettingsManager::instance().setValue("Audio/TimeStretch", enable);
        emit settingsChanged();
    }
}

void AudioSettings::setEnhancedAudio(bool enable)
{
    if (this->enhancedAudio() != enable) {
        SettingsManager::instance().setValue("Audio/EnhancedAudio", enable);
        emit settingsChanged();
    }
}

void AudioSettings::setEnhancedStereo(bool enable)
{
    if (this->enhancedStereo() != enable) {
        SettingsManager::instance().setValue("Audio/EnhancedStereo", enable);
        emit settingsChanged();
    }
}

void AudioSettings::setStereoSeparation(float separation)
{
    if (this->stereoSeparation() != separation) {
        SettingsManager::instance().setValue("Audio/StereoSeparation", separation);
        emit settingsChanged();
    }
}

void AudioSettings::setLowPassFilter(bool enable)
{
    if (this->lowPassFilter() != enable) {
        SettingsManager::instance().setValue("Audio/LowPassFilter", enable);
        emit settingsChanged();
    }
}

void AudioSettings::setReverb(bool enable)
{
    if (this->reverb() != enable) {
        SettingsManager::instance().setValue("Audio/Reverb", enable);
        emit settingsChanged();
    }
}

void AudioSettings::setReverbStrength(int strength)
{
    if (this->reverbStrength() != strength) {
        SettingsManager::instance().setValue("Audio/ReverbStrength", strength);
        emit settingsChanged();
    }
}

void AudioSettings::loadSettings()
{
    // Load settings from SettingsManager
    // All getters will call SettingsManager::instance().value() directly
}

void AudioSettings::saveSettings()
{
    // Save settings to SettingsManager
    // All settings are saved when they are changed via the setters
    SettingsManager::instance().sync();
}

} // namespace QT_UI
