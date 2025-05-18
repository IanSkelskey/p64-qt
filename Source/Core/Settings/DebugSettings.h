#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>  // Changed from forward declaration to proper include

namespace QT_UI {

class SettingsManager;

/**
 * @brief Settings related to debugging and development tools
 * 
 * This class manages settings for debugging features, including
 * breakpoints, logging, and development tools.
 */
class DebugSettings : public QObject
{
    Q_OBJECT

public:
    explicit DebugSettings(QObject* parent = nullptr);

    // Debug console options
    bool isDebugConsoleEnabled() const;
    bool isVerboseLoggingEnabled() const;
    QString getLogFilePath() const;
    int getLogLevel() const;
    QStringList getEnabledLogCategories() const;

    // Emulation debugging options
    bool isDebuggerEnabled() const;
    int getDebuggerPort() const;
    bool isRemoteDebuggingEnabled() const;
    bool isProfiling() const;
    bool isMemoryBreakpointsEnabled() const;
    bool isRegisterBreakpointsEnabled() const;
    bool isExceptionBreakpointsEnabled() const;
    
    // Development tools
    bool isMemoryViewerEnabled() const;
    bool isDisassemblerEnabled() const;
    bool isPerformanceCountersVisible() const;
    bool isCPULoggingEnabled() const;

    // Cache settings
    bool isInterpreterCacheEnabled() const;
    bool isDynarecCacheEnabled() const;
    int getDynarecBlockSize() const;
    
    // Setters
    void setDebugConsoleEnabled(bool enabled);
    void setVerboseLoggingEnabled(bool enabled);
    void setLogFilePath(const QString& path);
    void setLogLevel(int level);
    void setEnabledLogCategories(const QStringList& categories);
    void setDebuggerEnabled(bool enabled);
    void setDebuggerPort(int port);
    void setRemoteDebuggingEnabled(bool enabled);
    void setProfiling(bool enabled);
    void setMemoryBreakpointsEnabled(bool enabled);
    void setRegisterBreakpointsEnabled(bool enabled);
    void setExceptionBreakpointsEnabled(bool enabled);
    void setMemoryViewerEnabled(bool enabled);
    void setDisassemblerEnabled(bool enabled);
    void setPerformanceCountersVisible(bool visible);
    void setCPULoggingEnabled(bool enabled);
    void setInterpreterCacheEnabled(bool enabled);
    void setDynarecCacheEnabled(bool enabled);
    void setDynarecBlockSize(int size);
    
    // Debug operations
    void clearAllBreakpoints();
    void saveBreakpoints(const QString& filename);
    void loadBreakpoints(const QString& filename);
    void exportProfilingData(const QString& filename);

signals:
    void debugOptionsChanged();
    void loggingOptionsChanged();
    void breakpointOptionsChanged();
    void profileOptionsChanged();
    void cacheOptionsChanged();

private:
    friend class SettingsManager;
    
    void loadSettings();
    void saveSettings();
};

} // namespace QT_UI
