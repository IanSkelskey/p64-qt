#pragma once

#include <QObject>
#include <QString>
#include <QMap>
#include <QStringList>
#include <QVariant>  // Added for QVariant complete type

namespace QT_UI {

class SettingsManager;

/**
 * @brief Settings related to N64 emulation plugins
 * 
 * This class manages settings for plugins including video, audio,
 * input, and RSP plugins, their paths, and configurations.
 */
class PluginSettings : public QObject
{
    Q_OBJECT

public:
    explicit PluginSettings(QObject* parent = nullptr);

    // Plugin types
    enum class PluginType {
        Video,
        Audio,
        Input,
        RSP
    };
    Q_ENUM(PluginType)
    
    // RSP plugin types
    enum class RSPType {
        Interpreter,
        Dynamic,
        Static,
        Custom
    };
    Q_ENUM(RSPType)

    // Directory settings
    bool useDefaultPluginDirectory() const;
    QString pluginDirectory() const;
    
    // Basic plugin settings
    QString getSelectedPlugin(PluginType type) const;
    QString getPluginPath(PluginType type, const QString& pluginName) const;
    QStringList getAvailablePlugins(PluginType type) const;
    bool isPluginEnabled(PluginType type, const QString& pluginName) const;
    RSPType getRSPType() const;

    // Plugin-specific settings
    QVariant getPluginSetting(PluginType type, const QString& pluginName, const QString& setting, const QVariant& defaultValue = QVariant()) const;
    QMap<QString, QVariant> getAllPluginSettings(PluginType type, const QString& pluginName) const;

    // Directory setters
    void setUseDefaultPluginDirectory(bool useDefault);
    void setPluginDirectory(const QString& directory);
    
    // Plugin setters
    void setSelectedPlugin(PluginType type, const QString& pluginName);
    void setPluginPath(PluginType type, const QString& pluginName, const QString& path);
    void setPluginEnabled(PluginType type, const QString& pluginName, bool enabled);
    void setRSPType(RSPType type);
    void setPluginSetting(PluginType type, const QString& pluginName, const QString& setting, const QVariant& value);
    
    // Plugin management
    bool rescanPlugins();
    QStringList searchPluginDirectories() const;
    bool verifyPlugin(const QString& pluginPath) const;
    QMap<QString, QString> getPluginInfo(const QString& pluginPath) const;

signals:
    void selectedPluginChanged(PluginType type, const QString& pluginName);
    void pluginSettingChanged(PluginType type, const QString& pluginName, const QString& setting);
    void pluginsRescanCompleted();
    void rspTypeChanged();
    void directorySettingsChanged();

private:
    friend class SettingsManager;
    
    void loadSettings();
    void saveSettings();
    QString pluginTypeToString(PluginType type) const;
    QString pluginSettingKey(PluginType type, const QString& pluginName, const QString& setting = QString()) const;
};

} // namespace QT_UI
