#pragma once

#include <QObject>
#include <QString>
#include <QVariant>  // Added for completeness and future-proofing

namespace QT_UI {

class SettingsManager;

/**
 * @brief Settings related to N64 emulation core
 * 
 * This class manages settings for the emulation core,
 * including CPU, memory, and compatibility options.
 */
class EmulationSettings : public QObject
{
    Q_OBJECT

public:
    explicit EmulationSettings(QObject* parent = nullptr);

    // CPU settings
    enum class CpuCore {
        Interpreter,
        DynamicRecompiler
    };
    Q_ENUM(CpuCore)

    enum class CountPerOp {
        Auto = 0,
        Count1 = 1,
        Count2 = 2,
        Count3 = 3,
        Count4 = 4,
        Count5 = 5,
        Count6 = 6,
        Count7 = 7,
        Count8 = 8
    };
    Q_ENUM(CountPerOp)

    enum class RdramSize {
        Size4MB = 4,
        Size8MB = 8
    };
    Q_ENUM(RdramSize)

    // Getters
    CpuCore cpuCore() const;
    bool enableCompilerCache() const;
    bool assumeWideScreen() const;
    bool enableExpansionPak() const;
    bool enableMemoryExpansion() const;
    bool enableOverclock() const;
    float overclockFactor() const;
    CountPerOp countPerOp() const;
    RdramSize rdramSize() const;
    bool randomizeInterrupts() const;
    bool enableDebugger() const;
    bool limitVIs() const;
    int viLimit() const;
    bool syncToAudio() const;
    bool autoSaveMemory() const;
    bool autoDumpRom() const;
    bool useEmptyGameBoost() const;
    bool enableGameSpecificSettings() const;

    // Controller Pak settings
    enum class ControllerPakSize {
        Size32KB = 32,      // Standard N64 Controller Pak
        Size128KB = 128,    // 1Meg Datel
        Size512KB = 512,    // 4Meg Datel
        Size1984KB = 1984   // Maximum size
    };
    Q_ENUM(ControllerPakSize)

    ControllerPakSize controllerPakSize() const;
    bool allowSaveStateSaving() const;
    bool autoSaveStateEnabled() const;
    int autoSaveStateInterval() const;
    QString saveStateDirectory() const;

    // Setters
    void setCpuCore(CpuCore core);
    void setEnableCompilerCache(bool enable);
    void setAssumeWideScreen(bool enable);
    void setEnableExpansionPak(bool enable);
    void setEnableMemoryExpansion(bool enable);
    void setEnableOverclock(bool enable);
    void setOverclockFactor(float factor);
    void setCountPerOp(CountPerOp count);
    void setRdramSize(RdramSize size);
    void setRandomizeInterrupts(bool enable);
    void setEnableDebugger(bool enable);
    void setLimitVIs(bool enable);
    void setViLimit(int limit);
    void setSyncToAudio(bool enable);
    void setAutoSaveMemory(bool enable);
    void setAutoDumpRom(bool enable);
    void setUseEmptyGameBoost(bool enable);
    void setEnableGameSpecificSettings(bool enable);
    void setControllerPakSize(ControllerPakSize size);
    void setAllowSaveStateSaving(bool allow);
    void setAutoSaveStateEnabled(bool enable);
    void setAutoSaveStateInterval(int seconds);
    void setSaveStateDirectory(const QString& directory);

signals:
    void settingsChanged();
    void cpuCoreChanged();
    void memorySettingsChanged();
    void overclockSettingsChanged();

private:
    friend class SettingsManager;
    
    void loadSettings();
    void saveSettings();
};

} // namespace QT_UI
