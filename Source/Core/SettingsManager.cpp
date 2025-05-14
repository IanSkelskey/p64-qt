#include "SettingsManager.h"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QApplication>

namespace QT_UI {

SettingsManager& SettingsManager::instance()
{
    static SettingsManager instance;
    return instance;
}

SettingsManager::SettingsManager() 
    : QObject(nullptr)
    , m_settings("Project64", "QtUI")
{
    initializeDefaults();
}

SettingsManager::~SettingsManager()
{
    sync();
}

void SettingsManager::initializeDefaults()
{
    // Only set defaults for settings that don't exist yet
    if (!m_settings.contains("general/pauseWhenInactive"))
        m_settings.setValue("general/pauseWhenInactive", true);
    
    if (!m_settings.contains("general/enterFullscreen"))
        m_settings.setValue("general/enterFullscreen", false);
    
    if (!m_settings.contains("general/disableScreensaver"))
        m_settings.setValue("general/disableScreensaver", true);
    
    if (!m_settings.contains("general/enableDiscord"))
        m_settings.setValue("general/enableDiscord", true);
    
    if (!m_settings.contains("general/hideAdvancedSettings"))
        m_settings.setValue("general/hideAdvancedSettings", false);
    
    if (!m_settings.contains("general/maxRoms"))
        m_settings.setValue("general/maxRoms", 10);
    
    if (!m_settings.contains("general/maxRomDirs"))
        m_settings.setValue("general/maxRomDirs", 10);
    
    // Directory defaults
    if (!m_settings.contains("directories/useDefaultPlugin"))
        m_settings.setValue("directories/useDefaultPlugin", true);
    
    if (!m_settings.contains("directories/pluginDir"))
        m_settings.setValue("directories/pluginDir", QDir(QApplication::applicationDirPath()).filePath("Plugin"));
    
    if (!m_settings.contains("directories/useDefaultSaves"))
        m_settings.setValue("directories/useDefaultSaves", true);
    
    if (!m_settings.contains("directories/savesDir"))
        m_settings.setValue("directories/savesDir", QDir(QApplication::applicationDirPath()).filePath("Save"));
    
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
        m_settings.setValue("RomBrowser/ViewMode", static_cast<int>(DetailView));
    
    if (!m_settings.contains("RomBrowser/ShowTitles"))
        m_settings.setValue("RomBrowser/ShowTitles", true);
    
    if (!m_settings.contains("RomBrowser/CoverScale"))
        m_settings.setValue("RomBrowser/CoverScale", 1.0f);
    
    // Theme default
    if (!m_settings.contains("Theme/CurrentTheme"))
        m_settings.setValue("Theme/CurrentTheme", static_cast<int>(SystemTheme));
    
    // Cover Downloader defaults
    if (!m_settings.contains("Cover Downloader/UrlTemplates"))
        m_settings.setValue("Cover Downloader/UrlTemplates", "https://raw.githubusercontent.com/IanSkelskey/n64-covers/main/${rom_id}.png");
    
    if (!m_settings.contains("Cover Downloader/UseTitleNames"))
        m_settings.setValue("Cover Downloader/UseTitleNames", false);
    
    if (!m_settings.contains("Cover Downloader/OverwriteExisting"))
        m_settings.setValue("Cover Downloader/OverwriteExisting", false);
}

void SettingsManager::sync()
{
    m_settings.sync();
}

// General settings
bool SettingsManager::pauseWhenInactive() const
{
    return m_settings.value("general/pauseWhenInactive", true).toBool();
}

bool SettingsManager::enterFullscreenWhenLoadingRom() const
{
    return m_settings.value("general/enterFullscreen", false).toBool();
}

bool SettingsManager::disableScreensaver() const
{
    return m_settings.value("general/disableScreensaver", true).toBool();
}

bool SettingsManager::enableDiscord() const
{
    return m_settings.value("general/enableDiscord", true).toBool();
}

bool SettingsManager::hideAdvancedSettings() const
{
    return m_settings.value("general/hideAdvancedSettings", false).toBool();
}

int SettingsManager::maxRecentRoms() const
{
    return m_settings.value("general/maxRoms", 10).toInt();
}

int SettingsManager::maxRecentRomDirs() const
{
    return m_settings.value("general/maxRomDirs", 10).toInt();
}

void SettingsManager::setPauseWhenInactive(bool pause)
{
    if (pauseWhenInactive() != pause) {
        m_settings.setValue("general/pauseWhenInactive", pause);
        emit settingChanged("general/pauseWhenInactive");
    }
}

void SettingsManager::setEnterFullscreenWhenLoadingRom(bool enter)
{
    if (enterFullscreenWhenLoadingRom() != enter) {
        m_settings.setValue("general/enterFullscreen", enter);
        emit settingChanged("general/enterFullscreen");
    }
}

void SettingsManager::setDisableScreensaver(bool disable)
{
    if (disableScreensaver() != disable) {
        m_settings.setValue("general/disableScreensaver", disable);
        emit settingChanged("general/disableScreensaver");
    }
}

void SettingsManager::setEnableDiscord(bool enable)
{
    if (enableDiscord() != enable) {
        m_settings.setValue("general/enableDiscord", enable);
        emit settingChanged("general/enableDiscord");
    }
}

void SettingsManager::setHideAdvancedSettings(bool hide)
{
    if (hideAdvancedSettings() != hide) {
        m_settings.setValue("general/hideAdvancedSettings", hide);
        emit settingChanged("general/hideAdvancedSettings");
    }
}

void SettingsManager::setMaxRecentRoms(int max)
{
    if (maxRecentRoms() != max) {
        m_settings.setValue("general/maxRoms", max);
        emit settingChanged("general/maxRoms");
    }
}

void SettingsManager::setMaxRecentRomDirs(int max)
{
    if (maxRecentRomDirs() != max) {
        m_settings.setValue("general/maxRomDirs", max);
        emit settingChanged("general/maxRomDirs");
    }
}

// Directory settings
bool SettingsManager::useDefaultPluginDirectory() const
{
    return m_settings.value("directories/useDefaultPlugin", true).toBool();
}

bool SettingsManager::useDefaultSavesDirectory() const
{
    return m_settings.value("directories/useDefaultSaves", true).toBool();
}

QString SettingsManager::pluginDirectory() const
{
    if (useDefaultPluginDirectory()) {
        return QDir(QApplication::applicationDirPath()).filePath("Plugin");
    }
    return m_settings.value("directories/pluginDir").toString();
}

QString SettingsManager::savesDirectory() const
{
    if (useDefaultSavesDirectory()) {
        return QDir(QApplication::applicationDirPath()).filePath("Save");
    }
    return m_settings.value("directories/savesDir").toString();
}

QString SettingsManager::coverDirectory() const
{
    return m_settings.value("RomBrowser/CoverDirectory", 
                          QDir(QApplication::applicationDirPath()).filePath("covers")).toString();
}

void SettingsManager::setUseDefaultPluginDirectory(bool useDefault)
{
    if (useDefaultPluginDirectory() != useDefault) {
        m_settings.setValue("directories/useDefaultPlugin", useDefault);
        emit settingChanged("directories/useDefaultPlugin");
    }
}

void SettingsManager::setUseDefaultSavesDirectory(bool useDefault)
{
    if (useDefaultSavesDirectory() != useDefault) {
        m_settings.setValue("directories/useDefaultSaves", useDefault);
        emit settingChanged("directories/useDefaultSaves");
    }
}

void SettingsManager::setPluginDirectory(const QString& directory)
{
    if (pluginDirectory() != directory) {
        m_settings.setValue("directories/pluginDir", directory);
        emit settingChanged("directories/pluginDir");
    }
}

void SettingsManager::setSavesDirectory(const QString& directory)
{
    if (savesDirectory() != directory) {
        m_settings.setValue("directories/savesDir", directory);
        emit settingChanged("directories/savesDir");
    }
}

void SettingsManager::setCoverDirectory(const QString& directory)
{
    if (coverDirectory() != directory) {
        m_settings.setValue("RomBrowser/CoverDirectory", directory);
        emit settingChanged("RomBrowser/CoverDirectory");
    }
}

// ROM Browser settings
bool SettingsManager::romBrowserEnabled() const
{
    return m_settings.value("RomBrowser/Enabled", true).toBool();
}

bool SettingsManager::recursiveScan() const
{
    return m_settings.value("RomList/RecursiveScan", true).toBool();
}

bool SettingsManager::showFileExtensions() const
{
    return m_settings.value("RomList/ShowFileExtensions", true).toBool();
}

QString SettingsManager::lastRomDirectory() const
{
    return m_settings.value("RomBrowser/LastDirectory").toString();
}

QByteArray SettingsManager::tableHeaderState() const
{
    return m_settings.value("RomBrowser/TableHeader").toByteArray();
}

void SettingsManager::setRomBrowserEnabled(bool enabled)
{
    if (romBrowserEnabled() != enabled) {
        m_settings.setValue("RomBrowser/Enabled", enabled);
        emit settingChanged("RomBrowser/Enabled");
    }
}

void SettingsManager::setRecursiveScan(bool recursive)
{
    if (recursiveScan() != recursive) {
        m_settings.setValue("RomList/RecursiveScan", recursive);
        emit settingChanged("RomList/RecursiveScan");
    }
}

void SettingsManager::setShowFileExtensions(bool show)
{
    if (showFileExtensions() != show) {
        m_settings.setValue("RomList/ShowFileExtensions", show);
        emit settingChanged("RomList/ShowFileExtensions");
    }
}

void SettingsManager::setLastRomDirectory(const QString& directory)
{
    if (lastRomDirectory() != directory) {
        m_settings.setValue("RomBrowser/LastDirectory", directory);
        emit settingChanged("RomBrowser/LastDirectory");
    }
}

void SettingsManager::setTableHeaderState(const QByteArray& state)
{
    if (tableHeaderState() != state) {
        m_settings.setValue("RomBrowser/TableHeader", state);
        emit settingChanged("RomBrowser/TableHeader");
    }
}

// View settings
SettingsManager::ViewMode SettingsManager::viewMode() const
{
    return static_cast<ViewMode>(m_settings.value("RomBrowser/ViewMode", DetailView).toInt());
}

bool SettingsManager::showTitles() const
{
    return m_settings.value("RomBrowser/ShowTitles", true).toBool();
}

float SettingsManager::coverScale() const
{
    return m_settings.value("RomBrowser/CoverScale", 1.0f).toFloat();
}

void SettingsManager::setViewMode(ViewMode mode)
{
    if (viewMode() != mode) {
        m_settings.setValue("RomBrowser/ViewMode", static_cast<int>(mode));
        emit settingChanged("RomBrowser/ViewMode");
    }
}

void SettingsManager::setShowTitles(bool show)
{
    if (showTitles() != show) {
        m_settings.setValue("RomBrowser/ShowTitles", show);
        emit settingChanged("RomBrowser/ShowTitles");
    }
}

void SettingsManager::setCoverScale(float scale)
{
    if (coverScale() != scale) {
        m_settings.setValue("RomBrowser/CoverScale", scale);
        emit settingChanged("RomBrowser/CoverScale");
    }
}

// Column settings
QVariantList SettingsManager::visibleColumns() const
{
    return m_settings.value("RomBrowser/VisibleColumns").toList();
}

void SettingsManager::setVisibleColumns(const QVariantList& columns)
{
    // Remove any previous columns before saving new ones
    m_settings.remove("RomBrowser/VisibleColumns");
    
    m_settings.setValue("RomBrowser/VisibleColumns", columns);
    m_settings.sync(); // Force immediate save
    emit settingChanged("RomBrowser/VisibleColumns");
}

// Theme settings
SettingsManager::Theme SettingsManager::theme() const
{
    return static_cast<Theme>(m_settings.value("Theme/CurrentTheme", SystemTheme).toInt());
}

void SettingsManager::setTheme(Theme theme)
{
    if (this->theme() != theme) {
        m_settings.setValue("Theme/CurrentTheme", static_cast<int>(theme));
        emit settingChanged("Theme/CurrentTheme");
    }
}

// Recent items
QStringList SettingsManager::recentRoms() const
{
    return m_settings.value("RecentRoms").toStringList();
}

QStringList SettingsManager::recentRomDirectories() const
{
    return m_settings.value("RecentRomDirs").toStringList();
}

void SettingsManager::addRecentRom(const QString& path)
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
    
    m_settings.setValue("RecentRoms", roms);
    emit settingChanged("RecentRoms");
}

void SettingsManager::addRecentRomDirectory(const QString& path)
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
    
    m_settings.setValue("RecentRomDirs", dirs);
    emit settingChanged("RecentRomDirs");
}

void SettingsManager::clearRecentRoms()
{
    m_settings.remove("RecentRoms");
    emit settingChanged("RecentRoms");
}

void SettingsManager::clearRecentRomDirectories()
{
    m_settings.remove("RecentRomDirs");
    emit settingChanged("RecentRomDirs");
}

// URL Templates for Cover Downloader
QString SettingsManager::coverUrlTemplates() const
{
    return m_settings.value("Cover Downloader/UrlTemplates").toString();
}

bool SettingsManager::coverDownloaderUseTitleNames() const
{
    return m_settings.value("Cover Downloader/UseTitleNames", false).toBool();
}

bool SettingsManager::coverDownloaderOverwriteExisting() const
{
    return m_settings.value("Cover Downloader/OverwriteExisting", false).toBool();
}

void SettingsManager::setCoverUrlTemplates(const QString& templates)
{
    if (coverUrlTemplates() != templates) {
        m_settings.setValue("Cover Downloader/UrlTemplates", templates);
        emit settingChanged("Cover Downloader/UrlTemplates");
    }
}

void SettingsManager::setCoverDownloaderUseTitleNames(bool use)
{
    if (coverDownloaderUseTitleNames() != use) {
        m_settings.setValue("Cover Downloader/UseTitleNames", use);
        emit settingChanged("Cover Downloader/UseTitleNames");
    }
}

void SettingsManager::setCoverDownloaderOverwriteExisting(bool overwrite)
{
    if (coverDownloaderOverwriteExisting() != overwrite) {
        m_settings.setValue("Cover Downloader/OverwriteExisting", overwrite);
        emit settingChanged("Cover Downloader/OverwriteExisting");
    }
}

// Window settings
QByteArray SettingsManager::mainWindowGeometry() const
{
    return m_settings.value("mainWindowGeometry").toByteArray();
}

QByteArray SettingsManager::mainWindowState() const
{
    return m_settings.value("mainWindowState").toByteArray();
}

void SettingsManager::setMainWindowGeometry(const QByteArray& geometry)
{
    m_settings.setValue("mainWindowGeometry", geometry);
}

void SettingsManager::setMainWindowState(const QByteArray& state)
{
    m_settings.setValue("mainWindowState", state);
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
