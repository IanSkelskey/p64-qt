#include "RomBrowserSettings.h"
#include "SettingsManager.h"
#include <QDir>
#include <QApplication>

namespace QT_UI {

RomBrowserSettings::RomBrowserSettings(QObject* parent)
    : QObject(parent)
{
    loadSettings();
}

// Browser settings
bool RomBrowserSettings::isEnabled() const
{
    return SettingsManager::instance().value("RomBrowser/Enabled", true).toBool();
}

bool RomBrowserSettings::recursiveScan() const
{
    return SettingsManager::instance().value("RomList/RecursiveScan", true).toBool();
}

bool RomBrowserSettings::showFileExtensions() const
{
    return SettingsManager::instance().value("RomList/ShowFileExtensions", true).toBool();
}

QString RomBrowserSettings::lastRomDirectory() const
{
    return SettingsManager::instance().value("RomBrowser/LastDirectory", "").toString();
}

QByteArray RomBrowserSettings::tableHeaderState() const
{
    return SettingsManager::instance().value("RomBrowser/TableHeader").toByteArray();
}

// View settings
RomBrowserSettings::ViewMode RomBrowserSettings::viewMode() const
{
    return static_cast<ViewMode>(SettingsManager::instance().value(
        "RomBrowser/ViewMode", 
        static_cast<int>(ViewMode::DetailView)).toInt());
}

bool RomBrowserSettings::showTitles() const
{
    return SettingsManager::instance().value("RomBrowser/ShowTitles", true).toBool();
}

float RomBrowserSettings::coverScale() const
{
    return SettingsManager::instance().value("RomBrowser/CoverScale", 1.0f).toFloat();
}

// Column settings
QVariantList RomBrowserSettings::visibleColumns() const
{
    return SettingsManager::instance().value("RomBrowser/VisibleColumns").toList();
}

// Cover settings
QString RomBrowserSettings::coverDirectory() const
{
    return SettingsManager::instance().value("RomBrowser/CoverDirectory", 
                          QDir(QApplication::applicationDirPath()).filePath("covers")).toString();
}

QString RomBrowserSettings::coverUrlTemplates() const
{
    return SettingsManager::instance().value("Cover/UrlTemplates", 
        "https://raw.githubusercontent.com/IanSkelskey/n64-covers/main/${rom_id}.png").toString();
}

bool RomBrowserSettings::coverDownloaderUseTitleNames() const
{
    return SettingsManager::instance().value("Cover/UseTitleNames", false).toBool();
}

bool RomBrowserSettings::coverDownloaderOverwriteExisting() const
{
    return SettingsManager::instance().value("Cover/OverwriteExisting", false).toBool();
}

// Setters
void RomBrowserSettings::setEnabled(bool enabled)
{
    if (isEnabled() != enabled) {
        SettingsManager::instance().setValue("RomBrowser/Enabled", enabled);
        emit romBrowserSettingsChanged();
    }
}

void RomBrowserSettings::setRecursiveScan(bool recursive)
{
    if (recursiveScan() != recursive) {
        SettingsManager::instance().setValue("RomList/RecursiveScan", recursive);
        emit romBrowserSettingsChanged();
    }
}

void RomBrowserSettings::setShowFileExtensions(bool show)
{
    if (showFileExtensions() != show) {
        SettingsManager::instance().setValue("RomList/ShowFileExtensions", show);
        emit romBrowserSettingsChanged();
    }
}

void RomBrowserSettings::setLastRomDirectory(const QString& directory)
{
    if (lastRomDirectory() != directory) {
        SettingsManager::instance().setValue("RomBrowser/LastDirectory", directory);
        emit romBrowserSettingsChanged();
    }
}

void RomBrowserSettings::setTableHeaderState(const QByteArray& state)
{
    if (tableHeaderState() != state) {
        SettingsManager::instance().setValue("RomBrowser/TableHeader", state);
        emit romBrowserSettingsChanged();
    }
}

void RomBrowserSettings::setViewMode(ViewMode mode)
{
    if (viewMode() != mode) {
        SettingsManager::instance().setValue("RomBrowser/ViewMode", static_cast<int>(mode));
        emit viewSettingsChanged();
    }
}

void RomBrowserSettings::setShowTitles(bool show)
{
    if (showTitles() != show) {
        SettingsManager::instance().setValue("RomBrowser/ShowTitles", show);
        emit viewSettingsChanged();
    }
}

void RomBrowserSettings::setCoverScale(float scale)
{
    if (coverScale() != scale) {
        SettingsManager::instance().setValue("RomBrowser/CoverScale", scale);
        emit viewSettingsChanged();
    }
}

void RomBrowserSettings::setVisibleColumns(const QVariantList& columns)
{
    // Remove any previous columns before saving new ones
    SettingsManager::instance().setValue("RomBrowser/VisibleColumns", QVariantList());
    
    SettingsManager::instance().setValue("RomBrowser/VisibleColumns", columns);
    emit viewSettingsChanged();
}

void RomBrowserSettings::setCoverDirectory(const QString& directory)
{
    if (coverDirectory() != directory) {
        SettingsManager::instance().setValue("RomBrowser/CoverDirectory", directory);
        emit coverSettingsChanged();
    }
}

void RomBrowserSettings::setCoverUrlTemplates(const QString& templates)
{
    if (coverUrlTemplates() != templates) {
        SettingsManager::instance().setValue("Cover/UrlTemplates", templates);
        emit coverSettingsChanged();
    }
}

void RomBrowserSettings::setCoverDownloaderUseTitleNames(bool use)
{
    if (coverDownloaderUseTitleNames() != use) {
        SettingsManager::instance().setValue("Cover/UseTitleNames", use);
        emit coverSettingsChanged();
    }
}

void RomBrowserSettings::setCoverDownloaderOverwriteExisting(bool overwrite)
{
    if (coverDownloaderOverwriteExisting() != overwrite) {
        SettingsManager::instance().setValue("Cover/OverwriteExisting", overwrite);
        emit coverSettingsChanged();
    }
}

void RomBrowserSettings::loadSettings()
{
    // Default settings are loaded via the getters
}

void RomBrowserSettings::saveSettings()
{
    SettingsManager::instance().sync();
}

} // namespace QT_UI
