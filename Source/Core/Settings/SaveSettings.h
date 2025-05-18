#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QVariant>  // Added for consistency and future-proofing

namespace QT_UI {

class SettingsManager;

/**
 * @brief Settings related to game saves and states
 * 
 * This class manages settings for save files, save states,
 * and memory card configurations.
 */
class SaveSettings : public QObject
{
    Q_OBJECT

public:
    explicit SaveSettings(QObject* parent = nullptr);

    // Directory settings
    bool useDefaultSavesDirectory() const;
    QString getSavesDirectory() const;
    void setUseDefaultSavesDirectory(bool useDefault);
    void setSavesDirectory(const QString& directory);
    
    // Save file settings
    bool getAutoSaveEnabled() const;
    int getAutoSaveInterval() const; // in seconds
    QString getDefaultSaveFormat() const;
    
    // Save state settings
    bool getSaveStatesEnabled() const;
    QString getSaveStatesDirectory() const;
    int getMaxSaveSlots() const;
    bool getCompressSaveStates() const;
    int getCompressionLevel() const;
    bool getSaveScreenshotWithState() const;
    
    // Memory card settings
    QString getMemoryCardFile(int port) const;
    int getMemoryCardSize(int port) const; // in KB
    bool isMemoryCardEnabled(int port) const;
    QDateTime getLastBackupTime(int port) const;
    bool getAutoBackupMemoryCards() const;
    int getBackupInterval() const; // in minutes
    int getMaxMemoryCardBackups() const;
    
    // Battery save settings
    bool getBatterySaveEnabled() const;
    
    // Info about existing save files
    QStringList getAvailableSaveStates(const QString& romPath) const;
    QStringList getAvailableMemoryCardBackups(int port) const;

    // Setters
    void setAutoSaveEnabled(bool enabled);
    void setAutoSaveInterval(int seconds);
    void setDefaultSaveFormat(const QString& format);
    void setSaveStatesEnabled(bool enabled);
    void setSaveStatesDirectory(const QString& directory);
    void setMaxSaveSlots(int count);
    void setCompressSaveStates(bool compress);
    void setCompressionLevel(int level);
    void setSaveScreenshotWithState(bool enabled);
    void setMemoryCardFile(int port, const QString& filepath);
    void setMemoryCardSize(int port, int sizeKB);
    void setMemoryCardEnabled(int port, bool enabled);
    void setLastBackupTime(int port, const QDateTime& time);
    void setAutoBackupMemoryCards(bool enabled);
    void setBackupInterval(int minutes);
    void setMaxMemoryCardBackups(int count);
    void setBatterySaveEnabled(bool enabled);
    
    // Operations
    bool createMemoryCard(int port, int sizeKB);
    bool backupMemoryCard(int port);
    bool restoreMemoryCardBackup(int port, const QString& backupFileName);
    bool saveSaveState(int slot, const QString& romPath);
    bool loadSaveState(int slot, const QString& romPath);
    bool deleteSaveState(int slot, const QString& romPath);

signals:
    void saveSettingsChanged();
    void saveStateSettingsChanged();
    void memoryCardSettingsChanged(int port);
    void saveStateCreated(int slot);
    void saveStateLoaded(int slot);
    void memoryCardBackupCreated(int port, const QString& backupFileName);
    void directorySettingsChanged();

private:
    friend class SettingsManager;
    
    void loadSettings();
    void saveSettings();
    QString getMemoryCardSettingKey(int port, const QString& setting) const;
    QString getSaveStatePath(int slot, const QString& romPath) const;
    QString getMemoryCardBackupPath(int port, const QString& backupName) const;
};

} // namespace QT_UI
