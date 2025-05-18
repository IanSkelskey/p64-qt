#pragma once

#include <QObject>
#include <QString>
#include <QSettings>
#include <QVariant>

namespace QT_UI {

class GraphicsSettings;
class EmulationSettings;
class AudioSettings;
class InputSettings;
class PluginSettings;
class DebugSettings;
class SaveSettings;
class ApplicationSettings;
class RomBrowserSettings;

/**
 * @brief Central manager for application settings
 * 
 * This class provides a centralized interface for accessing and modifying
 * all application settings. It follows the singleton pattern to ensure
 * consistent access throughout the application.
 */
class SettingsManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the singleton instance
     */
    static SettingsManager& instance();

    /**
     * @brief Initialize settings with default values if not already set
     */
    void initializeDefaults();

    /**
     * @brief Save all settings to disk
     */
    void sync();

    // Access to specific settings modules
    GraphicsSettings* graphics() const;
    EmulationSettings* emulation() const;
    AudioSettings* audio() const;
    InputSettings* input() const;
    PluginSettings* plugins() const;
    DebugSettings* debug() const;
    SaveSettings* saves() const;
    ApplicationSettings* application() const;
    RomBrowserSettings* romBrowser() const;

    // Generic setting access
    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
    void setValue(const QString& key, const QVariant& value);

signals:
    void settingChanged(const QString& key);

private:
    SettingsManager();
    ~SettingsManager();

    // Prevent copying
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    QSettings m_settings;
};

} // namespace QT_UI
