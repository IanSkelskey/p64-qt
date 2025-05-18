#include "PluginSettings.h"
#include "SettingsManager.h"
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDebug>
#include <QLibrary>

namespace QT_UI {

PluginSettings::PluginSettings(QObject* parent)
    : QObject(parent)
{
    loadSettings();
}

// Directory settings
bool PluginSettings::useDefaultPluginDirectory() const
{
    return SettingsManager::instance().value("Directories/UseDefaultPlugin", true).toBool();
}

QString PluginSettings::pluginDirectory() const
{
    if (useDefaultPluginDirectory()) {
        return QDir(QCoreApplication::applicationDirPath()).filePath("Plugin");
    }
    return SettingsManager::instance().value("Directories/PluginDir", 
                                           QDir(QCoreApplication::applicationDirPath()).filePath("Plugin")).toString();
}

void PluginSettings::setUseDefaultPluginDirectory(bool useDefault)
{
    if (useDefaultPluginDirectory() != useDefault) {
        SettingsManager::instance().setValue("Directories/UseDefaultPlugin", useDefault);
        emit directorySettingsChanged();
    }
}

void PluginSettings::setPluginDirectory(const QString& directory)
{
    if (pluginDirectory() != directory) {
        SettingsManager::instance().setValue("Directories/PluginDir", directory);
        emit directorySettingsChanged();
    }
}

QString PluginSettings::pluginTypeToString(PluginType type) const
{
    switch (type) {
        case PluginType::Video: return "Video";
        case PluginType::Audio: return "Audio";
        case PluginType::Input: return "Input";
        case PluginType::RSP:   return "RSP";
        default: return "Unknown";
    }
}

QString PluginSettings::pluginSettingKey(PluginType type, const QString& pluginName, const QString& setting) const
{
    QString baseKey = QString("Plugins/%1/%2").arg(pluginTypeToString(type)).arg(pluginName);
    if (!setting.isEmpty()) {
        return baseKey + "/" + setting;
    }
    return baseKey;
}

// Basic plugin settings
QString PluginSettings::getSelectedPlugin(PluginType type) const
{
    return SettingsManager::instance().value(
        QString("Plugins/%1/Selected").arg(pluginTypeToString(type)),
        "").toString();
}

QString PluginSettings::getPluginPath(PluginType type, const QString& pluginName) const
{
    return SettingsManager::instance().value(
        pluginSettingKey(type, pluginName, "Path"),
        "").toString();
}

QStringList PluginSettings::getAvailablePlugins(PluginType type) const
{
    return SettingsManager::instance().value(
        QString("Plugins/%1/Available").arg(pluginTypeToString(type)),
        QStringList()).toStringList();
}

bool PluginSettings::isPluginEnabled(PluginType type, const QString& pluginName) const
{
    return SettingsManager::instance().value(
        pluginSettingKey(type, pluginName, "Enabled"),
        true).toBool();
}

PluginSettings::RSPType PluginSettings::getRSPType() const
{
    return static_cast<RSPType>(SettingsManager::instance().value(
        "Plugins/RSP/Type",
        static_cast<int>(RSPType::Dynamic)).toInt());
}

// Plugin-specific settings
QVariant PluginSettings::getPluginSetting(PluginType type, const QString& pluginName, const QString& setting, const QVariant& defaultValue) const
{
    return SettingsManager::instance().value(
        pluginSettingKey(type, pluginName, setting),
        defaultValue);
}

QMap<QString, QVariant> PluginSettings::getAllPluginSettings(PluginType type, const QString& pluginName) const
{
    QMap<QString, QVariant> settings;
    QString prefix = pluginSettingKey(type, pluginName) + "/";
    
    // This is a simplistic approach, in reality you'd need to iterate through all actual keys
    // with this prefix in the QSettings object
    
    // For demonstration purposes, we'll just populate with some common settings
    if (type == PluginType::Video) {
        settings["Resolution"] = getPluginSetting(type, pluginName, "Resolution", "640x480");
        settings["FullScreen"] = getPluginSetting(type, pluginName, "FullScreen", false);
        settings["VSync"] = getPluginSetting(type, pluginName, "VSync", true);
    }
    else if (type == PluginType::Audio) {
        settings["Latency"] = getPluginSetting(type, pluginName, "Latency", 100);
        settings["Synchronize"] = getPluginSetting(type, pluginName, "Synchronize", true);
    }
    
    return settings;
}

// Setters
void PluginSettings::setSelectedPlugin(PluginType type, const QString& pluginName)
{
    QString currentPlugin = getSelectedPlugin(type);
    if (currentPlugin != pluginName) {
        SettingsManager::instance().setValue(
            QString("Plugins/%1/Selected").arg(pluginTypeToString(type)),
            pluginName);
        emit selectedPluginChanged(type, pluginName);
    }
}

void PluginSettings::setPluginPath(PluginType type, const QString& pluginName, const QString& path)
{
    QString currentPath = getPluginPath(type, pluginName);
    if (currentPath != path) {
        SettingsManager::instance().setValue(
            pluginSettingKey(type, pluginName, "Path"),
            path);
    }
}

void PluginSettings::setPluginEnabled(PluginType type, const QString& pluginName, bool enabled)
{
    bool currentEnabled = isPluginEnabled(type, pluginName);
    if (currentEnabled != enabled) {
        SettingsManager::instance().setValue(
            pluginSettingKey(type, pluginName, "Enabled"),
            enabled);
    }
}

void PluginSettings::setRSPType(RSPType type)
{
    RSPType currentType = getRSPType();
    if (currentType != type) {
        SettingsManager::instance().setValue(
            "Plugins/RSP/Type",
            static_cast<int>(type));
        emit rspTypeChanged();
    }
}

void PluginSettings::setPluginSetting(PluginType type, const QString& pluginName, const QString& setting, const QVariant& value)
{
    QVariant currentValue = getPluginSetting(type, pluginName, setting);
    if (currentValue != value) {
        SettingsManager::instance().setValue(
            pluginSettingKey(type, pluginName, setting),
            value);
        emit pluginSettingChanged(type, pluginName, setting);
    }
}

// Plugin management
bool PluginSettings::rescanPlugins()
{
    // This would scan plugin directories and update available plugins
    QStringList pluginDirs = searchPluginDirectories();
    
    // Reset available plugin lists
    QStringList videoPlugins, audioPlugins, inputPlugins, rspPlugins;
    
    // For each directory, scan for plugins
    for (const QString& dir : pluginDirs) {
        QDir pluginDir(dir);
        QStringList entries = pluginDir.entryList(QDir::Files);
        
        for (const QString& entry : entries) {
            QString filePath = pluginDir.filePath(entry);
            
            if (verifyPlugin(filePath)) {
                QMap<QString, QString> info = getPluginInfo(filePath);
                QString type = info["Type"].toLower();
                QString name = info["Name"];
                
                if (!name.isEmpty()) {
                    if (type == "video") {
                        setPluginPath(PluginType::Video, name, filePath);
                        videoPlugins.append(name);
                    } 
                    else if (type == "audio") {
                        setPluginPath(PluginType::Audio, name, filePath);
                        audioPlugins.append(name);
                    }
                    else if (type == "input") {
                        setPluginPath(PluginType::Input, name, filePath);
                        inputPlugins.append(name);
                    }
                    else if (type == "rsp") {
                        setPluginPath(PluginType::RSP, name, filePath);
                        rspPlugins.append(name);
                    }
                }
            }
        }
    }
    
    // Save the available plugin lists
    SettingsManager::instance().setValue("Plugins/Video/Available", videoPlugins);
    SettingsManager::instance().setValue("Plugins/Audio/Available", audioPlugins);
    SettingsManager::instance().setValue("Plugins/Input/Available", inputPlugins);
    SettingsManager::instance().setValue("Plugins/RSP/Available", rspPlugins);
    
    emit pluginsRescanCompleted();
    return true;
}

QStringList PluginSettings::searchPluginDirectories() const
{
    QStringList dirs;
    
    // Default application plugin directory
    QString appDir = QCoreApplication::applicationDirPath();
    dirs << appDir;
    dirs << QDir(appDir).filePath("Plugin");
    
    // User-configured plugin directory
    if (!useDefaultPluginDirectory()) {
        dirs << pluginDirectory();
    }
    
    return dirs;
}

bool PluginSettings::verifyPlugin(const QString& pluginPath) const
{
    // This would verify if the file is a valid N64 plugin
    // Simplified implementation for example
    QFileInfo fileInfo(pluginPath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        return false;
    }
    
    QLibrary lib(pluginPath);
    if (!lib.load()) {
        return false;
    }
    
    // Check for common N64 plugin exports
    // This is a simplified check - real plugins have specific exports
    bool hasExports = lib.resolve("PluginStartup") != nullptr &&
                     lib.resolve("PluginShutdown") != nullptr;
    
    lib.unload();
    return hasExports;
}

QMap<QString, QString> PluginSettings::getPluginInfo(const QString& pluginPath) const
{
    QMap<QString, QString> info;
    
    // This would extract metadata from the plugin
    // Simplified implementation for example
    QFileInfo fileInfo(pluginPath);
    QString fileName = fileInfo.fileName().toLower();
    
    // Guess plugin type from filename (simplified approach)
    if (fileName.contains("video")) {
        info["Type"] = "Video";
        info["Name"] = fileInfo.baseName();
    }
    else if (fileName.contains("audio")) {
        info["Type"] = "Audio";
        info["Name"] = fileInfo.baseName();
    }
    else if (fileName.contains("input")) {
        info["Type"] = "Input";
        info["Name"] = fileInfo.baseName();
    }
    else if (fileName.contains("rsp")) {
        info["Type"] = "RSP";
        info["Name"] = fileInfo.baseName();
    }
    
    return info;
}

void PluginSettings::loadSettings()
{
    // Default settings are loaded via the getters
}

void PluginSettings::saveSettings()
{
    SettingsManager::instance().sync();
}

} // namespace QT_UI
