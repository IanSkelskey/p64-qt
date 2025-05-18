#include "SettingsManager.h"
#include "GraphicsSettings.h"
#include "EmulationSettings.h"
#include "AudioSettings.h"
#include "InputSettings.h"
#include "PluginSettings.h"
#include "DebugSettings.h"
#include "SaveSettings.h"
#include "ApplicationSettings.h"
#include "RomBrowserSettings.h"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QApplication>

namespace QT_UI {

// Pointer to singleton instance
static GraphicsSettings* g_graphicsSettings = nullptr;
static EmulationSettings* g_emulationSettings = nullptr;
static AudioSettings* g_audioSettings = nullptr;
static InputSettings* g_inputSettings = nullptr;
static PluginSettings* g_pluginSettings = nullptr;
static DebugSettings* g_debugSettings = nullptr;
static SaveSettings* g_saveSettings = nullptr;
static ApplicationSettings* g_applicationSettings = nullptr;
static RomBrowserSettings* g_romBrowserSettings = nullptr;

SettingsManager& SettingsManager::instance()
{
    static SettingsManager instance;
    return instance;
}

SettingsManager::SettingsManager() 
    : QObject(nullptr)
    , m_settings("Project64", "QtUI")
{
    // Create settings objects
    g_graphicsSettings = new GraphicsSettings(this);
    g_emulationSettings = new EmulationSettings(this);
    g_audioSettings = new AudioSettings(this);
    g_inputSettings = new InputSettings(this);
    g_pluginSettings = new PluginSettings(this);
    g_debugSettings = new DebugSettings(this);
    g_saveSettings = new SaveSettings(this);
    g_applicationSettings = new ApplicationSettings(this);
    g_romBrowserSettings = new RomBrowserSettings(this);
    
    initializeDefaults();
}

SettingsManager::~SettingsManager()
{
    sync();
}

GraphicsSettings* SettingsManager::graphics() const
{
    return g_graphicsSettings;
}

EmulationSettings* SettingsManager::emulation() const
{
    return g_emulationSettings;
}

AudioSettings* SettingsManager::audio() const
{
    return g_audioSettings;
}

InputSettings* SettingsManager::input() const
{
    return g_inputSettings;
}

PluginSettings* SettingsManager::plugins() const
{
    return g_pluginSettings;
}

DebugSettings* SettingsManager::debug() const
{
    return g_debugSettings;
}

SaveSettings* SettingsManager::saves() const
{
    return g_saveSettings;
}

ApplicationSettings* SettingsManager::application() const
{
    return g_applicationSettings;
}

RomBrowserSettings* SettingsManager::romBrowser() const
{
    return g_romBrowserSettings;
}

void SettingsManager::initializeDefaults()
{
    // Only set defaults for settings that don't exist yet
    if (!m_settings.contains("General/PauseWhenInactive"))
        m_settings.setValue("General/PauseWhenInactive", true);
    
    if (!m_settings.contains("General/EnterFullscreen"))
        m_settings.setValue("General/EnterFullscreen", false);
    
    if (!m_settings.contains("General/DisableScreensaver"))
        m_settings.setValue("General/DisableScreensaver", true);
    
    if (!m_settings.contains("General/EnableDiscord"))
        m_settings.setValue("General/EnableDiscord", true);
    
    if (!m_settings.contains("General/HideAdvancedSettings"))
        m_settings.setValue("General/HideAdvancedSettings", false);
    
    if (!m_settings.contains("General/MaxRecentRoms"))
        m_settings.setValue("General/MaxRecentRoms", 10);
    
    if (!m_settings.contains("General/MaxRecentRomDirs"))
        m_settings.setValue("General/MaxRecentRomDirs", 10);
    
    // Directory defaults
    if (!m_settings.contains("Directories/UseDefaultPlugin"))
        m_settings.setValue("Directories/UseDefaultPlugin", true);
    
    if (!m_settings.contains("Directories/PluginDir"))
        m_settings.setValue("Directories/PluginDir", QDir(QApplication::applicationDirPath()).filePath("Plugin"));
    
    if (!m_settings.contains("Directories/UseDefaultSaves"))
        m_settings.setValue("Directories/UseDefaultSaves", true);
    
    if (!m_settings.contains("Directories/SavesDir"))
        m_settings.setValue("Directories/SavesDir", QDir(QApplication::applicationDirPath()).filePath("Save"));
    
    if (!m_settings.contains("RomBrowser/CoverDirectory"))
        m_settings.setValue("RomBrowser/CoverDirectory", QDir(QApplication::applicationDirPath()).filePath("covers"));
    
    // ROM Browser defaults
    if (!m_settings.contains("RomBrowser/Enabled"))
        m_settings.setValue("RomBrowser/Enabled", true);
    
    if (!m_settings.contains("RomList/RecursiveScan"))
        m_settings.setValue("RomList/RecursiveScan", true);
    
    if (!m_settings.contains("RomList/ShowFileExtensions"))
        m_settings.setValue("RomList/ShowFileExtensions", true);
    
    // View mode defaults
    if (!m_settings.contains("RomBrowser/ViewMode"))
        m_settings.setValue("RomBrowser/ViewMode", 0); // DetailView
    
    if (!m_settings.contains("RomBrowser/ShowTitles"))
        m_settings.setValue("RomBrowser/ShowTitles", true);
    
    if (!m_settings.contains("RomBrowser/CoverScale"))
        m_settings.setValue("RomBrowser/CoverScale", 1.0f);
    
    // Theme default
    if (!m_settings.contains("Theme/CurrentTheme"))
        m_settings.setValue("Theme/CurrentTheme", 0); // SystemTheme
    
    // Cover Downloader defaults
    if (!m_settings.contains("Cover/UrlTemplates"))
        m_settings.setValue("Cover/UrlTemplates", "https://raw.githubusercontent.com/IanSkelskey/n64-covers/main/${rom_id}.png");
    
    if (!m_settings.contains("Cover/UseTitleNames"))
        m_settings.setValue("Cover/UseTitleNames", false);
    
    if (!m_settings.contains("Cover/OverwriteExisting"))
        m_settings.setValue("Cover/OverwriteExisting", false);
    
    // Input defaults
    if (!m_settings.contains("Input/DefaultControllerType"))
        m_settings.setValue("Input/DefaultControllerType", 0); // Standard controller
    
    // Plugin defaults
    if (!m_settings.contains("Plugins/AutoDetect"))
        m_settings.setValue("Plugins/AutoDetect", true);
    
    if (!m_settings.contains("Plugins/RSP/Type"))
        m_settings.setValue("Plugins/RSP/Type", 1); // Dynamic Recompiler
        
    // Debug defaults
    if (!m_settings.contains("Debug/EnableDebugger"))
        m_settings.setValue("Debug/EnableDebugger", false);
    
    if (!m_settings.contains("Debug/DebuggerPort"))
        m_settings.setValue("Debug/DebuggerPort", 9999);
        
    // Save defaults
    if (!m_settings.contains("SaveStates/Enabled"))
        m_settings.setValue("SaveStates/Enabled", true);
    
    if (!m_settings.contains("SaveStates/Compress"))
        m_settings.setValue("SaveStates/Compress", true);
    
    if (!m_settings.contains("SaveStates/CompressionLevel"))
        m_settings.setValue("SaveStates/CompressionLevel", 6);
        
    if (!m_settings.contains("SaveStates/MaxSlots"))
        m_settings.setValue("SaveStates/MaxSlots", 10);
}

void SettingsManager::sync()
{
    m_settings.sync();
}

// Generic setting access
QVariant SettingsManager::value(const QString& key, const QVariant& defaultValue) const
{
    return m_settings.value(key, defaultValue);
}

void SettingsManager::setValue(const QString& key, const QVariant& value)
{
    if (m_settings.value(key) != value) {
        m_settings.setValue(key, value);
        emit settingChanged(key);
    }
}

} // namespace QT_UI
