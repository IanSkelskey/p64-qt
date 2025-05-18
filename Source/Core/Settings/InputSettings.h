#pragma once

#include <QObject>
#include <QString>
#include <QMap>
#include <QList>

namespace QT_UI {

class SettingsManager;

/**
 * @brief Settings related to N64 controller input
 * 
 * This class manages all controller configuration, button mappings,
 * and peripheral settings for the N64 emulation.
 */
class InputSettings : public QObject
{
    Q_OBJECT

public:
    explicit InputSettings(QObject* parent = nullptr);

    // Controller types
    enum class ControllerType {
        None,
        Standard,
        Rumble,
        MouseInjector,
        Dancepad
    };
    Q_ENUM(ControllerType)

    // Controller peripheral types
    enum class PeripheralType {
        None,
        MemoryCard,
        RumblePak,
        TransferPak,
        VoicePak
    };
    Q_ENUM(PeripheralType)

    // Basic controller settings
    ControllerType getControllerType(int port) const;
    bool isControllerPluggedIn(int port) const;
    PeripheralType getPeripheralType(int port) const;

    // Controller button mappings
    QString getButtonMapping(int port, const QString& button) const;
    QMap<QString, QString> getAllButtonMappings(int port) const;
    QStringList getAvailableButtons() const;

    // Analog stick settings
    int getDeadzone(int port) const;
    int getSensitivity(int port) const;
    bool invertXAxis(int port) const;
    bool invertYAxis(int port) const;

    // Rumble settings
    int getRumbleStrength(int port) const;
    bool isRumbleEnabled(int port) const;

    // Mouse injector settings
    double getMouseSensitivity() const;
    bool invertMouseX() const;
    bool invertMouseY() const;
    bool enableMouseInjector() const;

    // Setters
    void setControllerType(int port, ControllerType type);
    void setControllerPluggedIn(int port, bool pluggedIn);
    void setPeripheralType(int port, PeripheralType type);
    void setButtonMapping(int port, const QString& button, const QString& mapping);
    void setAllButtonMappings(int port, const QMap<QString, QString>& mappings);
    void setDeadzone(int port, int deadzone);
    void setSensitivity(int port, int sensitivity);
    void setInvertXAxis(int port, bool invert);
    void setInvertYAxis(int port, bool invert);
    void setRumbleStrength(int port, int strength);
    void setRumbleEnabled(int port, bool enabled);
    void setMouseSensitivity(double sensitivity);
    void setInvertMouseX(bool invert);
    void setInvertMouseY(bool invert);
    void setEnableMouseInjector(bool enable);

    // Controller pak methods
    void createMemoryCard(int port);
    void transferGameBoyRom(int port, const QString& romPath);

signals:
    void controllerConfigChanged(int port);
    void buttonMappingChanged(int port, const QString& button);
    void rumbleSettingsChanged(int port);
    void mouseSettingsChanged();

private:
    friend class SettingsManager;

    void loadSettings();
    void saveSettings();
    QString controllerSettingsKey(int port, const QString& setting) const;
};

} // namespace QT_UI
