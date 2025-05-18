#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QByteArray>

namespace QT_UI {

class SettingsManager;

/**
 * @brief Settings related to ROM browser and cover display
 * 
 * This class manages settings for the ROM browser, including view modes,
 * cover art display, and ROM list behavior.
 */
class RomBrowserSettings : public QObject
{
    Q_OBJECT

public:
    explicit RomBrowserSettings(QObject* parent = nullptr);

    // View modes
    enum class ViewMode {
        DetailView = 0,
        GridView = 1
    };
    Q_ENUM(ViewMode)

    // Browser settings
    bool isEnabled() const;
    bool recursiveScan() const;
    bool showFileExtensions() const;
    QString lastRomDirectory() const;
    QByteArray tableHeaderState() const;
    
    // View settings
    ViewMode viewMode() const;
    bool showTitles() const;
    float coverScale() const;
    
    // Column settings
    QVariantList visibleColumns() const;
    
    // Cover settings
    QString coverDirectory() const;
    QString coverUrlTemplates() const;
    bool coverDownloaderUseTitleNames() const;
    bool coverDownloaderOverwriteExisting() const;

    // Setters
    void setEnabled(bool enabled);
    void setRecursiveScan(bool recursive);
    void setShowFileExtensions(bool show);
    void setLastRomDirectory(const QString& directory);
    void setTableHeaderState(const QByteArray& state);
    void setViewMode(ViewMode mode);
    void setShowTitles(bool show);
    void setCoverScale(float scale);
    void setVisibleColumns(const QVariantList& columns);
    void setCoverDirectory(const QString& directory);
    void setCoverUrlTemplates(const QString& templates);
    void setCoverDownloaderUseTitleNames(bool use);
    void setCoverDownloaderOverwriteExisting(bool overwrite);

signals:
    void romBrowserSettingsChanged();
    void viewSettingsChanged();
    void coverSettingsChanged();

private:
    friend class SettingsManager;
    
    void loadSettings();
    void saveSettings();
};

} // namespace QT_UI
