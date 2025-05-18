#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>

namespace QT_UI {

class SettingsManager;

/**
 * @brief General application settings
 * 
 * This class manages general application settings like UI behavior,
 * appearance, and recent items.
 */
class ApplicationSettings : public QObject
{
    Q_OBJECT

public:
    explicit ApplicationSettings(QObject* parent = nullptr);

    // Theme settings
    enum class Theme {
        SystemTheme = 0,
        LightTheme = 1,
        DarkTheme = 2
    };
    Q_ENUM(Theme)

    // General behavior settings
    bool pauseWhenInactive() const;
    bool enterFullscreenWhenLoadingRom() const;
    bool disableScreensaver() const;
    bool enableDiscord() const;
    bool hideAdvancedSettings() const;
    int maxRecentRoms() const;
    int maxRecentRomDirs() const;

    // Theme settings
    Theme theme() const;
    
    // Window settings
    QByteArray mainWindowGeometry() const;
    QByteArray mainWindowState() const;
    
    // Recent items
    QStringList recentRoms() const;
    QStringList recentRomDirectories() const;

    // Setters
    void setPauseWhenInactive(bool pause);
    void setEnterFullscreenWhenLoadingRom(bool enter);
    void setDisableScreensaver(bool disable);
    void setEnableDiscord(bool enable);
    void setHideAdvancedSettings(bool hide);
    void setMaxRecentRoms(int max);
    void setMaxRecentRomDirs(int max);
    void setTheme(Theme theme);
    void setMainWindowGeometry(const QByteArray& geometry);
    void setMainWindowState(const QByteArray& state);
    
    // Recent items management
    void addRecentRom(const QString& path);
    void addRecentRomDirectory(const QString& path);
    void clearRecentRoms();
    void clearRecentRomDirectories();

signals:
    void generalSettingsChanged();
    void themeChanged();
    void recentItemsChanged();

private:
    friend class SettingsManager;
    
    void loadSettings();
    void saveSettings();
};

} // namespace QT_UI
