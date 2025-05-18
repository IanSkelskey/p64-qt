#include "ApplicationSettings.h"
#include "SettingsManager.h"

namespace QT_UI {

ApplicationSettings::ApplicationSettings(QObject* parent)
    : QObject(parent)
{
    loadSettings();
}

// General behavior settings
bool ApplicationSettings::pauseWhenInactive() const
{
    return SettingsManager::instance().value("General/PauseWhenInactive", true).toBool();
}

bool ApplicationSettings::enterFullscreenWhenLoadingRom() const
{
    return SettingsManager::instance().value("General/EnterFullscreen", false).toBool();
}

bool ApplicationSettings::disableScreensaver() const
{
    return SettingsManager::instance().value("General/DisableScreensaver", true).toBool();
}

bool ApplicationSettings::enableDiscord() const
{
    return SettingsManager::instance().value("General/EnableDiscord", true).toBool();
}

bool ApplicationSettings::hideAdvancedSettings() const
{
    return SettingsManager::instance().value("General/HideAdvancedSettings", false).toBool();
}

int ApplicationSettings::maxRecentRoms() const
{
    return SettingsManager::instance().value("General/MaxRecentRoms", 10).toInt();
}

int ApplicationSettings::maxRecentRomDirs() const
{
    return SettingsManager::instance().value("General/MaxRecentRomDirs", 10).toInt();
}

// Theme settings
ApplicationSettings::Theme ApplicationSettings::theme() const
{
    return static_cast<Theme>(SettingsManager::instance().value(
        "Theme/CurrentTheme", 
        static_cast<int>(Theme::SystemTheme)).toInt());
}

// Window settings
QByteArray ApplicationSettings::mainWindowGeometry() const
{
    return SettingsManager::instance().value("UI/MainWindowGeometry").toByteArray();
}

QByteArray ApplicationSettings::mainWindowState() const
{
    return SettingsManager::instance().value("UI/MainWindowState").toByteArray();
}

// Recent items
QStringList ApplicationSettings::recentRoms() const
{
    return SettingsManager::instance().value("Recent/Roms", QStringList()).toStringList();
}

QStringList ApplicationSettings::recentRomDirectories() const
{
    return SettingsManager::instance().value("Recent/RomDirs", QStringList()).toStringList();
}

// Setters
void ApplicationSettings::setPauseWhenInactive(bool pause)
{
    if (pauseWhenInactive() != pause) {
        SettingsManager::instance().setValue("General/PauseWhenInactive", pause);
        emit generalSettingsChanged();
    }
}

void ApplicationSettings::setEnterFullscreenWhenLoadingRom(bool enter)
{
    if (enterFullscreenWhenLoadingRom() != enter) {
        SettingsManager::instance().setValue("General/EnterFullscreen", enter);
        emit generalSettingsChanged();
    }
}

void ApplicationSettings::setDisableScreensaver(bool disable)
{
    if (disableScreensaver() != disable) {
        SettingsManager::instance().setValue("General/DisableScreensaver", disable);
        emit generalSettingsChanged();
    }
}

void ApplicationSettings::setEnableDiscord(bool enable)
{
    if (enableDiscord() != enable) {
        SettingsManager::instance().setValue("General/EnableDiscord", enable);
        emit generalSettingsChanged();
    }
}

void ApplicationSettings::setHideAdvancedSettings(bool hide)
{
    if (hideAdvancedSettings() != hide) {
        SettingsManager::instance().setValue("General/HideAdvancedSettings", hide);
        emit generalSettingsChanged();
    }
}

void ApplicationSettings::setMaxRecentRoms(int max)
{
    if (maxRecentRoms() != max) {
        SettingsManager::instance().setValue("General/MaxRecentRoms", max);
        emit generalSettingsChanged();
    }
}

void ApplicationSettings::setMaxRecentRomDirs(int max)
{
    if (maxRecentRomDirs() != max) {
        SettingsManager::instance().setValue("General/MaxRecentRomDirs", max);
        emit generalSettingsChanged();
    }
}

void ApplicationSettings::setTheme(Theme theme)
{
    if (this->theme() != theme) {
        SettingsManager::instance().setValue("Theme/CurrentTheme", static_cast<int>(theme));
        emit themeChanged();
    }
}

void ApplicationSettings::setMainWindowGeometry(const QByteArray& geometry)
{
    SettingsManager::instance().setValue("UI/MainWindowGeometry", geometry);
}

void ApplicationSettings::setMainWindowState(const QByteArray& state)
{
    SettingsManager::instance().setValue("UI/MainWindowState", state);
}

// Recent items management
void ApplicationSettings::addRecentRom(const QString& path)
{
    QStringList roms = recentRoms();
    
    // Remove if already in list
    roms.removeAll(path);
    
    // Add to front
    roms.prepend(path);
    
    // Trim list to max size
    while (roms.size() > maxRecentRoms()) {
        roms.removeLast();
    }
    
    SettingsManager::instance().setValue("Recent/Roms", roms);
    emit recentItemsChanged();
}

void ApplicationSettings::addRecentRomDirectory(const QString& path)
{
    QStringList dirs = recentRomDirectories();
    
    // Remove if already in list
    dirs.removeAll(path);
    
    // Add to front
    dirs.prepend(path);
    
    // Trim list to max size
    while (dirs.size() > maxRecentRomDirs()) {
        dirs.removeLast();
    }
    
    SettingsManager::instance().setValue("Recent/RomDirs", dirs);
    emit recentItemsChanged();
}

void ApplicationSettings::clearRecentRoms()
{
    SettingsManager::instance().setValue("Recent/Roms", QStringList());
    emit recentItemsChanged();
}

void ApplicationSettings::clearRecentRomDirectories()
{
    SettingsManager::instance().setValue("Recent/RomDirs", QStringList());
    emit recentItemsChanged();
}

void ApplicationSettings::loadSettings()
{
    // Default settings are loaded via the getters
}

void ApplicationSettings::saveSettings()
{
    SettingsManager::instance().sync();
}

} // namespace QT_UI
