#include "SaveSettings.h"
#include "SettingsManager.h"
#include <QDir>
#include <QFileInfo>
#include <QApplication>
#include <QDebug>

namespace QT_UI {

SaveSettings::SaveSettings(QObject* parent)
    : QObject(parent)
{
    loadSettings();
}

QString SaveSettings::getMemoryCardSettingKey(int port, const QString& setting) const
{
    return QString("MemoryCards/Port%1/%2").arg(port).arg(setting);
}

// Directory settings
bool SaveSettings::useDefaultSavesDirectory() const
{
    return SettingsManager::instance().value("Directories/UseDefaultSaves", true).toBool();
}

QString SaveSettings::getSavesDirectory() const
{
    // Default to application's Save directory
    if (useDefaultSavesDirectory()) {
        return QDir(QApplication::applicationDirPath()).filePath("Save");
    }
    return SettingsManager::instance().value("Directories/SavesDir", 
                                           QDir(QApplication::applicationDirPath()).filePath("Save")).toString();
}

void SaveSettings::setUseDefaultSavesDirectory(bool useDefault)
{
    if (useDefaultSavesDirectory() != useDefault) {
        SettingsManager::instance().setValue("Directories/UseDefaultSaves", useDefault);
        emit directorySettingsChanged();
    }
}

void SaveSettings::setSavesDirectory(const QString& directory)
{
    if (getSavesDirectory() != directory) {
        SettingsManager::instance().setValue("Directories/SavesDir", directory);
        emit directorySettingsChanged();
    }
}

// Save file settings
bool SaveSettings::getAutoSaveEnabled() const
{
    return SettingsManager::instance().value("Saves/AutoSaveEnabled", true).toBool();
}

int SaveSettings::getAutoSaveInterval() const
{
    return SettingsManager::instance().value("Saves/AutoSaveInterval", 60).toInt();
}

QString SaveSettings::getDefaultSaveFormat() const
{
    return SettingsManager::instance().value("Saves/DefaultFormat", "SRAM").toString();
}

// Save state settings
bool SaveSettings::getSaveStatesEnabled() const
{
    return SettingsManager::instance().value("SaveStates/Enabled", true).toBool();
}

QString SaveSettings::getSaveStatesDirectory() const
{
    QString defaultDir = QDir(getSavesDirectory()).filePath("States");
    return SettingsManager::instance().value("SaveStates/Directory", defaultDir).toString();
}

int SaveSettings::getMaxSaveSlots() const
{
    return SettingsManager::instance().value("SaveStates/MaxSlots", 10).toInt();
}

bool SaveSettings::getCompressSaveStates() const
{
    return SettingsManager::instance().value("SaveStates/Compress", true).toBool();
}

int SaveSettings::getCompressionLevel() const
{
    return SettingsManager::instance().value("SaveStates/CompressionLevel", 6).toInt();
}

bool SaveSettings::getSaveScreenshotWithState() const
{
    return SettingsManager::instance().value("SaveStates/SaveScreenshot", true).toBool();
}

// Memory card settings
QString SaveSettings::getMemoryCardFile(int port) const
{
    QString defaultPath = QDir(getSavesDirectory()).filePath(QString("memcard%1.mpc").arg(port));
    return SettingsManager::instance().value(getMemoryCardSettingKey(port, "File"), defaultPath).toString();
}

int SaveSettings::getMemoryCardSize(int port) const
{
    // Default to 32KB for standard N64 memory cards
    return SettingsManager::instance().value(getMemoryCardSettingKey(port, "Size"), 32).toInt();
}

bool SaveSettings::isMemoryCardEnabled(int port) const
{
    // By default, enable memory cards for the first two controller ports
    return SettingsManager::instance().value(getMemoryCardSettingKey(port, "Enabled"), port < 2).toBool();
}

QDateTime SaveSettings::getLastBackupTime(int port) const
{
    return SettingsManager::instance().value(getMemoryCardSettingKey(port, "LastBackup"), QDateTime()).toDateTime();
}

bool SaveSettings::getAutoBackupMemoryCards() const
{
    return SettingsManager::instance().value("MemoryCards/AutoBackup", true).toBool();
}

int SaveSettings::getBackupInterval() const
{
    // Default to daily backups (1440 minutes)
    return SettingsManager::instance().value("MemoryCards/BackupInterval", 1440).toInt();
}

int SaveSettings::getMaxMemoryCardBackups() const
{
    return SettingsManager::instance().value("MemoryCards/MaxBackups", 5).toInt();
}

// Battery save settings
bool SaveSettings::getBatterySaveEnabled() const
{
    return SettingsManager::instance().value("Saves/BatterySaveEnabled", true).toBool();
}

// Info about existing save files
QStringList SaveSettings::getAvailableSaveStates(const QString& romPath) const
{
    QStringList result;
    QFileInfo fileInfo(romPath);
    QString baseName = fileInfo.completeBaseName();
    QString statesDir = getSaveStatesDirectory();
    
    QDir dir(statesDir);
    QStringList filters;
    filters << QString("%1.s??").arg(baseName);
    dir.setNameFilters(filters);
    
    return dir.entryList(QDir::Files, QDir::Time);
}

QStringList SaveSettings::getAvailableMemoryCardBackups(int port) const
{
    QStringList result;
    QString memoryCardFile = getMemoryCardFile(port);
    if (memoryCardFile.isEmpty()) return result;
    
    QFileInfo fileInfo(memoryCardFile);
    QString baseName = fileInfo.completeBaseName();
    QString backupDir = QDir(fileInfo.path()).filePath("backups");
    
    QDir dir(backupDir);
    QStringList filters;
    filters << QString("%1_backup_*.mpc").arg(baseName);
    dir.setNameFilters(filters);
    
    return dir.entryList(QDir::Files, QDir::Time);
}

// Setters
void SaveSettings::setAutoSaveEnabled(bool enabled)
{
    if (getAutoSaveEnabled() != enabled) {
        SettingsManager::instance().setValue("Saves/AutoSaveEnabled", enabled);
        emit saveSettingsChanged();
    }
}

void SaveSettings::setAutoSaveInterval(int seconds)
{
    if (getAutoSaveInterval() != seconds) {
        SettingsManager::instance().setValue("Saves/AutoSaveInterval", seconds);
        emit saveSettingsChanged();
    }
}

void SaveSettings::setDefaultSaveFormat(const QString& format)
{
    if (getDefaultSaveFormat() != format) {
        SettingsManager::instance().setValue("Saves/DefaultFormat", format);
        emit saveSettingsChanged();
    }
}

void SaveSettings::setSaveStatesEnabled(bool enabled)
{
    if (getSaveStatesEnabled() != enabled) {
        SettingsManager::instance().setValue("SaveStates/Enabled", enabled);
        emit saveStateSettingsChanged();
    }
}

void SaveSettings::setSaveStatesDirectory(const QString& directory)
{
    if (getSaveStatesDirectory() != directory) {
        SettingsManager::instance().setValue("SaveStates/Directory", directory);
        emit saveStateSettingsChanged();
    }
}

void SaveSettings::setMaxSaveSlots(int count)
{
    if (getMaxSaveSlots() != count) {
        SettingsManager::instance().setValue("SaveStates/MaxSlots", count);
        emit saveStateSettingsChanged();
    }
}

void SaveSettings::setCompressSaveStates(bool compress)
{
    if (getCompressSaveStates() != compress) {
        SettingsManager::instance().setValue("SaveStates/Compress", compress);
        emit saveStateSettingsChanged();
    }
}

void SaveSettings::setCompressionLevel(int level)
{
    if (getCompressionLevel() != level) {
        SettingsManager::instance().setValue("SaveStates/CompressionLevel", level);
        emit saveStateSettingsChanged();
    }
}

void SaveSettings::setSaveScreenshotWithState(bool enabled)
{
    if (getSaveScreenshotWithState() != enabled) {
        SettingsManager::instance().setValue("SaveStates/SaveScreenshot", enabled);
        emit saveStateSettingsChanged();
    }
}

void SaveSettings::setMemoryCardFile(int port, const QString& filepath)
{
    if (getMemoryCardFile(port) != filepath) {
        SettingsManager::instance().setValue(getMemoryCardSettingKey(port, "File"), filepath);
        emit memoryCardSettingsChanged(port);
    }
}

void SaveSettings::setMemoryCardSize(int port, int sizeKB)
{
    if (getMemoryCardSize(port) != sizeKB) {
        SettingsManager::instance().setValue(getMemoryCardSettingKey(port, "Size"), sizeKB);
        emit memoryCardSettingsChanged(port);
    }
}

void SaveSettings::setMemoryCardEnabled(int port, bool enabled)
{
    if (isMemoryCardEnabled(port) != enabled) {
        SettingsManager::instance().setValue(getMemoryCardSettingKey(port, "Enabled"), enabled);
        emit memoryCardSettingsChanged(port);
    }
}

void SaveSettings::setLastBackupTime(int port, const QDateTime& time)
{
    if (getLastBackupTime(port) != time) {
        SettingsManager::instance().setValue(getMemoryCardSettingKey(port, "LastBackup"), time);
    }
}

void SaveSettings::setAutoBackupMemoryCards(bool enabled)
{
    if (getAutoBackupMemoryCards() != enabled) {
        SettingsManager::instance().setValue("MemoryCards/AutoBackup", enabled);
        emit saveSettingsChanged();
    }
}

void SaveSettings::setBackupInterval(int minutes)
{
    if (getBackupInterval() != minutes) {
        SettingsManager::instance().setValue("MemoryCards/BackupInterval", minutes);
        emit saveSettingsChanged();
    }
}

void SaveSettings::setMaxMemoryCardBackups(int count)
{
    if (getMaxMemoryCardBackups() != count) {
        SettingsManager::instance().setValue("MemoryCards/MaxBackups", count);
        emit saveSettingsChanged();
    }
}

void SaveSettings::setBatterySaveEnabled(bool enabled)
{
    if (getBatterySaveEnabled() != enabled) {
        SettingsManager::instance().setValue("Saves/BatterySaveEnabled", enabled);
        emit saveSettingsChanged();
    }
}

// Operations
bool SaveSettings::createMemoryCard(int port, int sizeKB)
{
    // This would be implemented to create a memory card file with the given size
    QString filepath = getMemoryCardFile(port);
    qDebug() << "Creating memory card at" << filepath << "with size" << sizeKB << "KB";
    
    setMemoryCardSize(port, sizeKB);
    setMemoryCardEnabled(port, true);
    emit memoryCardSettingsChanged(port);
    return true;
}

bool SaveSettings::backupMemoryCard(int port)
{
    // This would be implemented to create a backup of the memory card
    QString memoryCardFile = getMemoryCardFile(port);
    if (memoryCardFile.isEmpty() || !QFile::exists(memoryCardFile)) {
        return false;
    }
    
    QFileInfo fileInfo(memoryCardFile);
    QString backupDir = QDir(fileInfo.path()).filePath("backups");
    QDir().mkpath(backupDir);
    
    QDateTime now = QDateTime::currentDateTime();
    QString backupName = QString("%1_backup_%2.mpc")
                            .arg(fileInfo.completeBaseName())
                            .arg(now.toString("yyyyMMdd_HHmmss"));
    QString backupPath = QDir(backupDir).filePath(backupName);
    
    // Perform the backup operation
    bool success = QFile::copy(memoryCardFile, backupPath);
    if (success) {
        setLastBackupTime(port, now);
        emit memoryCardBackupCreated(port, backupName);
    }
    
    // Clean up old backups if we exceed the maximum
    QStringList backups = getAvailableMemoryCardBackups(port);
    int maxBackups = getMaxMemoryCardBackups();
    if (backups.size() > maxBackups) {
        for (int i = maxBackups; i < backups.size(); i++) {
            QFile::remove(QDir(backupDir).filePath(backups[i]));
        }
    }
    
    return success;
}

bool SaveSettings::restoreMemoryCardBackup(int port, const QString& backupFileName)
{
    // This would be implemented to restore a memory card from a backup
    QString memoryCardFile = getMemoryCardFile(port);
    if (memoryCardFile.isEmpty()) {
        return false;
    }
    
    QFileInfo fileInfo(memoryCardFile);
    QString backupDir = QDir(fileInfo.path()).filePath("backups");
    QString backupPath = QDir(backupDir).filePath(backupFileName);
    
    if (!QFile::exists(backupPath)) {
        return false;
    }
    
    // Backup the current file before restoration
    backupMemoryCard(port);
    
    // Perform the restore operation
    bool success = QFile::remove(memoryCardFile) && QFile::copy(backupPath, memoryCardFile);
    
    if (success) {
        emit memoryCardSettingsChanged(port);
    }
    
    return success;
}

QString SaveSettings::getSaveStatePath(int slot, const QString& romPath) const
{
    QFileInfo fileInfo(romPath);
    QString baseName = fileInfo.completeBaseName();
    QString statesDir = getSaveStatesDirectory();
    QDir().mkpath(statesDir);
    
    return QDir(statesDir).filePath(QString("%1.s%2").arg(baseName).arg(slot, 2, 10, QChar('0')));
}

bool SaveSettings::saveSaveState(int slot, const QString& romPath)
{
    // This would be implemented to create a save state
    if (!getSaveStatesEnabled()) {
        return false;
    }
    
    QString savePath = getSaveStatePath(slot, romPath);
    qDebug() << "Saving state to" << savePath;
    
    // The actual implementation would call into the emulator's save state functionality
    // For now, we just simulate success
    emit saveStateCreated(slot);
    return true;
}

bool SaveSettings::loadSaveState(int slot, const QString& romPath)
{
    // This would be implemented to load a save state
    if (!getSaveStatesEnabled()) {
        return false;
    }
    
    QString savePath = getSaveStatePath(slot, romPath);
    if (!QFile::exists(savePath)) {
        return false;
    }
    
    qDebug() << "Loading state from" << savePath;
    
    // The actual implementation would call into the emulator's load state functionality
    // For now, we just simulate success
    emit saveStateLoaded(slot);
    return true;
}

bool SaveSettings::deleteSaveState(int slot, const QString& romPath)
{
    // This would be implemented to delete a save state
    QString savePath = getSaveStatePath(slot, romPath);
    if (!QFile::exists(savePath)) {
        return false;
    }
    
    return QFile::remove(savePath);
}

void SaveSettings::loadSettings()
{
    // Default settings are loaded via the getters
}

void SaveSettings::saveSettings()
{
    SettingsManager::instance().sync();
}

} // namespace QT_UI
