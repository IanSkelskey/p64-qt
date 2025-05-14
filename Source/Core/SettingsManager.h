#pragma once

#include <QObject>
#include <QString>
#include <QSettings>
#include <QVariant>
#include <QMap>
#include <QSize>
#include <QVector>

namespace QT_UI {

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

    // General settings
    bool pauseWhenInactive() const;
    bool enterFullscreenWhenLoadingRom() const;
    bool disableScreensaver() const;
    bool enableDiscord() const;
    bool hideAdvancedSettings() const;
    int maxRecentRoms() const;
    int maxRecentRomDirs() const;

    void setPauseWhenInactive(bool pause);
    void setEnterFullscreenWhenLoadingRom(bool enter);
    void setDisableScreensaver(bool disable);
    void setEnableDiscord(bool enable);
    void setHideAdvancedSettings(bool hide);
    void setMaxRecentRoms(int max);
    void setMaxRecentRomDirs(int max);

    // Directory settings
    bool useDefaultPluginDirectory() const;
    bool useDefaultSavesDirectory() const;
    QString pluginDirectory() const;
    QString savesDirectory() const;
    QString coverDirectory() const;
    
    void setUseDefaultPluginDirectory(bool useDefault);
    void setUseDefaultSavesDirectory(bool useDefault);
    void setPluginDirectory(const QString& directory);
    void setSavesDirectory(const QString& directory);
    void setCoverDirectory(const QString& directory);

    // ROM Browser settings
    bool romBrowserEnabled() const;
    bool recursiveScan() const;
    bool showFileExtensions() const;
    QString lastRomDirectory() const;
    QByteArray tableHeaderState() const;

    void setRomBrowserEnabled(bool enabled);
    void setRecursiveScan(bool recursive);
    void setShowFileExtensions(bool show);
    void setLastRomDirectory(const QString& directory);
    void setTableHeaderState(const QByteArray& state);

    // View settings
    enum ViewMode {
        DetailView = 0,
        GridView = 1
    };

    ViewMode viewMode() const;
    bool showTitles() const;
    float coverScale() const;

    void setViewMode(ViewMode mode);
    void setShowTitles(bool show);
    void setCoverScale(float scale);

    // Column settings
    QVariantList visibleColumns() const;
    void setVisibleColumns(const QVariantList& columns);

    // Theme settings
    enum Theme {
        SystemTheme = 0,
        LightTheme = 1,
        DarkTheme = 2
    };

    Theme theme() const;
    void setTheme(Theme theme);

    // Recent items
    QStringList recentRoms() const;
    QStringList recentRomDirectories() const;
    
    void addRecentRom(const QString& path);
    void addRecentRomDirectory(const QString& path);
    void clearRecentRoms();
    void clearRecentRomDirectories();

    // URL Templates for Cover Downloader
    QString coverUrlTemplates() const;
    bool coverDownloaderUseTitleNames() const;
    bool coverDownloaderOverwriteExisting() const;

    void setCoverUrlTemplates(const QString& templates);
    void setCoverDownloaderUseTitleNames(bool use);
    void setCoverDownloaderOverwriteExisting(bool overwrite);

    // Window settings
    QByteArray mainWindowGeometry() const;
    QByteArray mainWindowState() const;

    void setMainWindowGeometry(const QByteArray& geometry);
    void setMainWindowState(const QByteArray& state);

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
