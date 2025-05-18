#include "EmulationSettings.h"
#include "SettingsManager.h"
#include <QDir>
#include <QApplication>

namespace QT_UI {

EmulationSettings::EmulationSettings(QObject* parent)
    : QObject(parent)
{
    loadSettings();
}

// CPU settings
EmulationSettings::CpuCore EmulationSettings::cpuCore() const
{
    return static_cast<CpuCore>(SettingsManager::instance().value(
        "Emulation/CpuCore", 
        static_cast<int>(CpuCore::DynamicRecompiler)).toInt());
}

bool EmulationSettings::enableCompilerCache() const
{
    return SettingsManager::instance().value("Emulation/EnableCompilerCache", true).toBool();
}

bool EmulationSettings::assumeWideScreen() const
{
    return SettingsManager::instance().value("Emulation/AssumeWideScreen", false).toBool();
}

bool EmulationSettings::enableExpansionPak() const
{
    return SettingsManager::instance().value("Emulation/EnableExpansionPak", true).toBool();
}

bool EmulationSettings::enableMemoryExpansion() const
{
    return SettingsManager::instance().value("Emulation/EnableMemoryExpansion", false).toBool();
}

bool EmulationSettings::enableOverclock() const
{
    return SettingsManager::instance().value("Emulation/EnableOverclock", false).toBool();
}

float EmulationSettings::overclockFactor() const
{
    return SettingsManager::instance().value("Emulation/OverclockFactor", 1.0f).toFloat();
}

EmulationSettings::CountPerOp EmulationSettings::countPerOp() const
{
    return static_cast<CountPerOp>(SettingsManager::instance().value(
        "Emulation/CountPerOp", 
        static_cast<int>(CountPerOp::Auto)).toInt());
}

EmulationSettings::RdramSize EmulationSettings::rdramSize() const
{
    return static_cast<RdramSize>(SettingsManager::instance().value(
        "Emulation/RdramSize", 
        static_cast<int>(RdramSize::Size4MB)).toInt());
}

bool EmulationSettings::randomizeInterrupts() const
{
    return SettingsManager::instance().value("Emulation/RandomizeInterrupts", true).toBool();
}

bool EmulationSettings::enableDebugger() const
{
    return SettingsManager::instance().value("Emulation/EnableDebugger", false).toBool();
}

bool EmulationSettings::limitVIs() const
{
    return SettingsManager::instance().value("Emulation/LimitVIs", true).toBool();
}

int EmulationSettings::viLimit() const
{
    return SettingsManager::instance().value("Emulation/VILimit", 60).toInt();
}

bool EmulationSettings::syncToAudio() const
{
    return SettingsManager::instance().value("Emulation/SyncToAudio", true).toBool();
}

bool EmulationSettings::autoSaveMemory() const
{
    return SettingsManager::instance().value("Emulation/AutoSaveMemory", true).toBool();
}

bool EmulationSettings::autoDumpRom() const
{
    return SettingsManager::instance().value("Emulation/AutoDumpRom", false).toBool();
}

bool EmulationSettings::useEmptyGameBoost() const
{
    return SettingsManager::instance().value("Emulation/UseEmptyGameBoost", true).toBool();
}

bool EmulationSettings::enableGameSpecificSettings() const
{
    return SettingsManager::instance().value("Emulation/EnableGameSpecificSettings", true).toBool();
}

// Controller Pak settings
EmulationSettings::ControllerPakSize EmulationSettings::controllerPakSize() const
{
    return static_cast<ControllerPakSize>(SettingsManager::instance().value(
        "Emulation/ControllerPakSize", 
        static_cast<int>(ControllerPakSize::Size32KB)).toInt());
}

bool EmulationSettings::allowSaveStateSaving() const
{
    return SettingsManager::instance().value("Emulation/AllowSaveStateSaving", true).toBool();
}

bool EmulationSettings::autoSaveStateEnabled() const
{
    return SettingsManager::instance().value("Emulation/AutoSaveStateEnabled", false).toBool();
}

int EmulationSettings::autoSaveStateInterval() const
{
    return SettingsManager::instance().value("Emulation/AutoSaveStateInterval", 300).toInt();
}

QString EmulationSettings::saveStateDirectory() const
{
    QString defaultDir = QDir(QApplication::applicationDirPath()).filePath("Save/States");
    return SettingsManager::instance().value("Emulation/SaveStateDirectory", defaultDir).toString();
}

// Setters
void EmulationSettings::setCpuCore(CpuCore core)
{
    if (this->cpuCore() != core) {
        SettingsManager::instance().setValue("Emulation/CpuCore", static_cast<int>(core));
        emit cpuCoreChanged();
        emit settingsChanged();
    }
}

void EmulationSettings::setEnableCompilerCache(bool enable)
{
    if (this->enableCompilerCache() != enable) {
        SettingsManager::instance().setValue("Emulation/EnableCompilerCache", enable);
        emit settingsChanged();
    }
}

void EmulationSettings::setAssumeWideScreen(bool enable)
{
    if (this->assumeWideScreen() != enable) {
        SettingsManager::instance().setValue("Emulation/AssumeWideScreen", enable);
        emit settingsChanged();
    }
}

void EmulationSettings::setEnableExpansionPak(bool enable)
{
    if (this->enableExpansionPak() != enable) {
        SettingsManager::instance().setValue("Emulation/EnableExpansionPak", enable);
        emit memorySettingsChanged();
        emit settingsChanged();
    }
}

void EmulationSettings::setEnableMemoryExpansion(bool enable)
{
    if (this->enableMemoryExpansion() != enable) {
        SettingsManager::instance().setValue("Emulation/EnableMemoryExpansion", enable);
        emit memorySettingsChanged();
        emit settingsChanged();
    }
}

void EmulationSettings::setEnableOverclock(bool enable)
{
    if (this->enableOverclock() != enable) {
        SettingsManager::instance().setValue("Emulation/EnableOverclock", enable);
        emit overclockSettingsChanged();
        emit settingsChanged();
    }
}

void EmulationSettings::setOverclockFactor(float factor)
{
    if (this->overclockFactor() != factor) {
        SettingsManager::instance().setValue("Emulation/OverclockFactor", factor);
        emit overclockSettingsChanged();
        emit settingsChanged();
    }
}

void EmulationSettings::setCountPerOp(CountPerOp count)
{
    if (this->countPerOp() != count) {
        SettingsManager::instance().setValue("Emulation/CountPerOp", static_cast<int>(count));
        emit settingsChanged();
    }
}

void EmulationSettings::setRdramSize(RdramSize size)
{
    if (this->rdramSize() != size) {
        SettingsManager::instance().setValue("Emulation/RdramSize", static_cast<int>(size));
        emit memorySettingsChanged();
        emit settingsChanged();
    }
}

void EmulationSettings::setRandomizeInterrupts(bool enable)
{
    if (this->randomizeInterrupts() != enable) {
        SettingsManager::instance().setValue("Emulation/RandomizeInterrupts", enable);
        emit settingsChanged();
    }
}

void EmulationSettings::setEnableDebugger(bool enable)
{
    if (this->enableDebugger() != enable) {
        SettingsManager::instance().setValue("Emulation/EnableDebugger", enable);
        emit settingsChanged();
    }
}

void EmulationSettings::setLimitVIs(bool enable)
{
    if (this->limitVIs() != enable) {
        SettingsManager::instance().setValue("Emulation/LimitVIs", enable);
        emit settingsChanged();
    }
}

void EmulationSettings::setViLimit(int limit)
{
    if (this->viLimit() != limit) {
        SettingsManager::instance().setValue("Emulation/VILimit", limit);
        emit settingsChanged();
    }
}

void EmulationSettings::setSyncToAudio(bool enable)
{
    if (this->syncToAudio() != enable) {
        SettingsManager::instance().setValue("Emulation/SyncToAudio", enable);
        emit settingsChanged();
    }
}

void EmulationSettings::setAutoSaveMemory(bool enable)
{
    if (this->autoSaveMemory() != enable) {
        SettingsManager::instance().setValue("Emulation/AutoSaveMemory", enable);
        emit settingsChanged();
    }
}

void EmulationSettings::setAutoDumpRom(bool enable)
{
    if (this->autoDumpRom() != enable) {
        SettingsManager::instance().setValue("Emulation/AutoDumpRom", enable);
        emit settingsChanged();
    }
}

void EmulationSettings::setUseEmptyGameBoost(bool enable)
{
    if (this->useEmptyGameBoost() != enable) {
        SettingsManager::instance().setValue("Emulation/UseEmptyGameBoost", enable);
        emit settingsChanged();
    }
}

void EmulationSettings::setEnableGameSpecificSettings(bool enable)
{
    if (this->enableGameSpecificSettings() != enable) {
        SettingsManager::instance().setValue("Emulation/EnableGameSpecificSettings", enable);
        emit settingsChanged();
    }
}

void EmulationSettings::setControllerPakSize(ControllerPakSize size)
{
    if (this->controllerPakSize() != size) {
        SettingsManager::instance().setValue("Emulation/ControllerPakSize", static_cast<int>(size));
        emit settingsChanged();
    }
}

void EmulationSettings::setAllowSaveStateSaving(bool allow)
{
    if (this->allowSaveStateSaving() != allow) {
        SettingsManager::instance().setValue("Emulation/AllowSaveStateSaving", allow);
        emit settingsChanged();
    }
}

void EmulationSettings::setAutoSaveStateEnabled(bool enable)
{
    if (this->autoSaveStateEnabled() != enable) {
        SettingsManager::instance().setValue("Emulation/AutoSaveStateEnabled", enable);
        emit settingsChanged();
    }
}

void EmulationSettings::setAutoSaveStateInterval(int seconds)
{
    if (this->autoSaveStateInterval() != seconds) {
        SettingsManager::instance().setValue("Emulation/AutoSaveStateInterval", seconds);
        emit settingsChanged();
    }
}

void EmulationSettings::setSaveStateDirectory(const QString& directory)
{
    if (this->saveStateDirectory() != directory) {
        SettingsManager::instance().setValue("Emulation/SaveStateDirectory", directory);
        emit settingsChanged();
    }
}

void EmulationSettings::loadSettings()
{
    // Load settings from SettingsManager
    // All getters will call SettingsManager::instance().value() directly
}

void EmulationSettings::saveSettings()
{
    // Save settings to SettingsManager
    // All settings are saved when they are changed via the setters
    SettingsManager::instance().sync();
}

} // namespace QT_UI
