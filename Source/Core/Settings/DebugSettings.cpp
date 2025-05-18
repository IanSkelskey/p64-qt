#include "DebugSettings.h"
#include "SettingsManager.h"
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

namespace QT_UI {

DebugSettings::DebugSettings(QObject* parent)
    : QObject(parent)
{
    loadSettings();
}

// Debug console options
bool DebugSettings::isDebugConsoleEnabled() const
{
    return SettingsManager::instance().value("Debug/ConsoleEnabled", false).toBool();
}

bool DebugSettings::isVerboseLoggingEnabled() const
{
    return SettingsManager::instance().value("Debug/VerboseLoggingEnabled", false).toBool();
}

QString DebugSettings::getLogFilePath() const
{
    QString defaultPath = QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation))
                            .filePath("Project64/Logs/emulator.log");
    return SettingsManager::instance().value("Debug/LogFilePath", defaultPath).toString();
}

int DebugSettings::getLogLevel() const
{
    return SettingsManager::instance().value("Debug/LogLevel", 1).toInt();
}

QStringList DebugSettings::getEnabledLogCategories() const
{
    return SettingsManager::instance().value("Debug/EnabledLogCategories", QStringList()).toStringList();
}

// Emulation debugging options
bool DebugSettings::isDebuggerEnabled() const
{
    return SettingsManager::instance().value("Debug/DebuggerEnabled", false).toBool();
}

int DebugSettings::getDebuggerPort() const
{
    return SettingsManager::instance().value("Debug/DebuggerPort", 9999).toInt();
}

bool DebugSettings::isRemoteDebuggingEnabled() const
{
    return SettingsManager::instance().value("Debug/RemoteDebuggingEnabled", false).toBool();
}

bool DebugSettings::isProfiling() const
{
    return SettingsManager::instance().value("Debug/Profiling", false).toBool();
}

bool DebugSettings::isMemoryBreakpointsEnabled() const
{
    return SettingsManager::instance().value("Debug/MemoryBreakpointsEnabled", false).toBool();
}

bool DebugSettings::isRegisterBreakpointsEnabled() const
{
    return SettingsManager::instance().value("Debug/RegisterBreakpointsEnabled", false).toBool();
}

bool DebugSettings::isExceptionBreakpointsEnabled() const
{
    return SettingsManager::instance().value("Debug/ExceptionBreakpointsEnabled", false).toBool();
}

// Development tools
bool DebugSettings::isMemoryViewerEnabled() const
{
    return SettingsManager::instance().value("Debug/MemoryViewerEnabled", false).toBool();
}

bool DebugSettings::isDisassemblerEnabled() const
{
    return SettingsManager::instance().value("Debug/DisassemblerEnabled", false).toBool();
}

bool DebugSettings::isPerformanceCountersVisible() const
{
    return SettingsManager::instance().value("Debug/PerformanceCountersVisible", false).toBool();
}

bool DebugSettings::isCPULoggingEnabled() const
{
    return SettingsManager::instance().value("Debug/CPULoggingEnabled", false).toBool();
}

// Cache settings
bool DebugSettings::isInterpreterCacheEnabled() const
{
    return SettingsManager::instance().value("Debug/InterpreterCacheEnabled", true).toBool();
}

bool DebugSettings::isDynarecCacheEnabled() const
{
    return SettingsManager::instance().value("Debug/DynarecCacheEnabled", true).toBool();
}

int DebugSettings::getDynarecBlockSize() const
{
    return SettingsManager::instance().value("Debug/DynarecBlockSize", 100).toInt();
}

// Setters
void DebugSettings::setDebugConsoleEnabled(bool enabled)
{
    if (isDebugConsoleEnabled() != enabled) {
        SettingsManager::instance().setValue("Debug/ConsoleEnabled", enabled);
        emit debugOptionsChanged();
    }
}

void DebugSettings::setVerboseLoggingEnabled(bool enabled)
{
    if (isVerboseLoggingEnabled() != enabled) {
        SettingsManager::instance().setValue("Debug/VerboseLoggingEnabled", enabled);
        emit loggingOptionsChanged();
    }
}

void DebugSettings::setLogFilePath(const QString& path)
{
    if (getLogFilePath() != path) {
        SettingsManager::instance().setValue("Debug/LogFilePath", path);
        emit loggingOptionsChanged();
    }
}

void DebugSettings::setLogLevel(int level)
{
    if (getLogLevel() != level) {
        SettingsManager::instance().setValue("Debug/LogLevel", level);
        emit loggingOptionsChanged();
    }
}

void DebugSettings::setEnabledLogCategories(const QStringList& categories)
{
    SettingsManager::instance().setValue("Debug/EnabledLogCategories", categories);
    emit loggingOptionsChanged();
}

void DebugSettings::setDebuggerEnabled(bool enabled)
{
    if (isDebuggerEnabled() != enabled) {
        SettingsManager::instance().setValue("Debug/DebuggerEnabled", enabled);
        emit debugOptionsChanged();
    }
}

void DebugSettings::setDebuggerPort(int port)
{
    if (getDebuggerPort() != port) {
        SettingsManager::instance().setValue("Debug/DebuggerPort", port);
        emit debugOptionsChanged();
    }
}

void DebugSettings::setRemoteDebuggingEnabled(bool enabled)
{
    if (isRemoteDebuggingEnabled() != enabled) {
        SettingsManager::instance().setValue("Debug/RemoteDebuggingEnabled", enabled);
        emit debugOptionsChanged();
    }
}

void DebugSettings::setProfiling(bool enabled)
{
    if (isProfiling() != enabled) {
        SettingsManager::instance().setValue("Debug/Profiling", enabled);
        emit profileOptionsChanged();
    }
}

void DebugSettings::setMemoryBreakpointsEnabled(bool enabled)
{
    if (isMemoryBreakpointsEnabled() != enabled) {
        SettingsManager::instance().setValue("Debug/MemoryBreakpointsEnabled", enabled);
        emit breakpointOptionsChanged();
    }
}

void DebugSettings::setRegisterBreakpointsEnabled(bool enabled)
{
    if (isRegisterBreakpointsEnabled() != enabled) {
        SettingsManager::instance().setValue("Debug/RegisterBreakpointsEnabled", enabled);
        emit breakpointOptionsChanged();
    }
}

void DebugSettings::setExceptionBreakpointsEnabled(bool enabled)
{
    if (isExceptionBreakpointsEnabled() != enabled) {
        SettingsManager::instance().setValue("Debug/ExceptionBreakpointsEnabled", enabled);
        emit breakpointOptionsChanged();
    }
}

void DebugSettings::setMemoryViewerEnabled(bool enabled)
{
    if (isMemoryViewerEnabled() != enabled) {
        SettingsManager::instance().setValue("Debug/MemoryViewerEnabled", enabled);
        emit debugOptionsChanged();
    }
}

void DebugSettings::setDisassemblerEnabled(bool enabled)
{
    if (isDisassemblerEnabled() != enabled) {
        SettingsManager::instance().setValue("Debug/DisassemblerEnabled", enabled);
        emit debugOptionsChanged();
    }
}

void DebugSettings::setPerformanceCountersVisible(bool visible)
{
    if (isPerformanceCountersVisible() != visible) {
        SettingsManager::instance().setValue("Debug/PerformanceCountersVisible", visible);
        emit debugOptionsChanged();
    }
}

void DebugSettings::setCPULoggingEnabled(bool enabled)
{
    if (isCPULoggingEnabled() != enabled) {
        SettingsManager::instance().setValue("Debug/CPULoggingEnabled", enabled);
        emit loggingOptionsChanged();
    }
}

void DebugSettings::setInterpreterCacheEnabled(bool enabled)
{
    if (isInterpreterCacheEnabled() != enabled) {
        SettingsManager::instance().setValue("Debug/InterpreterCacheEnabled", enabled);
        emit cacheOptionsChanged();
    }
}

void DebugSettings::setDynarecCacheEnabled(bool enabled)
{
    if (isDynarecCacheEnabled() != enabled) {
        SettingsManager::instance().setValue("Debug/DynarecCacheEnabled", enabled);
        emit cacheOptionsChanged();
    }
}

void DebugSettings::setDynarecBlockSize(int size)
{
    if (getDynarecBlockSize() != size) {
        SettingsManager::instance().setValue("Debug/DynarecBlockSize", size);
        emit cacheOptionsChanged();
    }
}

// Debug operations
void DebugSettings::clearAllBreakpoints()
{
    // This would be implemented to clear all breakpoints
    emit breakpointOptionsChanged();
}

void DebugSettings::saveBreakpoints(const QString& filename)
{
    // This would be implemented to save breakpoints to a file
    qDebug() << "Saving breakpoints to" << filename;
}

void DebugSettings::loadBreakpoints(const QString& filename)
{
    // This would be implemented to load breakpoints from a file
    qDebug() << "Loading breakpoints from" << filename;
    emit breakpointOptionsChanged();
}

void DebugSettings::exportProfilingData(const QString& filename)
{
    // This would be implemented to export profiling data to a file
    qDebug() << "Exporting profiling data to" << filename;
}

void DebugSettings::loadSettings()
{
    // Default settings are loaded via the getters
}

void DebugSettings::saveSettings()
{
    SettingsManager::instance().sync();
}

} // namespace QT_UI
