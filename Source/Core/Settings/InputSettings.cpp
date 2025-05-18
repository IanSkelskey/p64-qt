#include "InputSettings.h"
#include "SettingsManager.h"
#include <QDir>
#include <QDebug>

namespace QT_UI {

InputSettings::InputSettings(QObject* parent)
    : QObject(parent)
{
    loadSettings();
}

// Helper for controller settings key generation
QString InputSettings::controllerSettingsKey(int port, const QString& setting) const
{
    return QString("Input/Controller%1/%2").arg(port).arg(setting);
}

// Basic controller settings
InputSettings::ControllerType InputSettings::getControllerType(int port) const
{
    return static_cast<ControllerType>(SettingsManager::instance().value(
        controllerSettingsKey(port, "Type"), 
        port < 4 ? static_cast<int>(ControllerType::Standard) : static_cast<int>(ControllerType::None)).toInt());
}

bool InputSettings::isControllerPluggedIn(int port) const
{
    return SettingsManager::instance().value(controllerSettingsKey(port, "PluggedIn"), port < 4).toBool();
}

InputSettings::PeripheralType InputSettings::getPeripheralType(int port) const
{
    return static_cast<PeripheralType>(SettingsManager::instance().value(
        controllerSettingsKey(port, "Peripheral"), 
        static_cast<int>(PeripheralType::None)).toInt());
}

// Controller button mappings
QString InputSettings::getButtonMapping(int port, const QString& button) const
{
    return SettingsManager::instance().value(
        controllerSettingsKey(port, QString("Button_%1").arg(button)), "").toString();
}

QMap<QString, QString> InputSettings::getAllButtonMappings(int port) const
{
    QMap<QString, QString> mappings;
    for (const QString& button : getAvailableButtons()) {
        mappings[button] = getButtonMapping(port, button);
    }
    return mappings;
}

QStringList InputSettings::getAvailableButtons() const
{
    return {
        "A", "B", "Z", "Start", 
        "DPadUp", "DPadDown", "DPadLeft", "DPadRight",
        "CUp", "CDown", "CLeft", "CRight",
        "L", "R"
    };
}

// Analog stick settings
int InputSettings::getDeadzone(int port) const
{
    return SettingsManager::instance().value(controllerSettingsKey(port, "Deadzone"), 10).toInt();
}

int InputSettings::getSensitivity(int port) const
{
    return SettingsManager::instance().value(controllerSettingsKey(port, "Sensitivity"), 100).toInt();
}

bool InputSettings::invertXAxis(int port) const
{
    return SettingsManager::instance().value(controllerSettingsKey(port, "InvertX"), false).toBool();
}

bool InputSettings::invertYAxis(int port) const
{
    return SettingsManager::instance().value(controllerSettingsKey(port, "InvertY"), false).toBool();
}

// Rumble settings
int InputSettings::getRumbleStrength(int port) const
{
    return SettingsManager::instance().value(controllerSettingsKey(port, "RumbleStrength"), 80).toInt();
}

bool InputSettings::isRumbleEnabled(int port) const
{
    return SettingsManager::instance().value(controllerSettingsKey(port, "RumbleEnabled"), true).toBool();
}

// Mouse injector settings
double InputSettings::getMouseSensitivity() const
{
    return SettingsManager::instance().value("Input/Mouse/Sensitivity", 1.0).toDouble();
}

bool InputSettings::invertMouseX() const
{
    return SettingsManager::instance().value("Input/Mouse/InvertX", false).toBool();
}

bool InputSettings::invertMouseY() const
{
    return SettingsManager::instance().value("Input/Mouse/InvertY", false).toBool();
}

bool InputSettings::enableMouseInjector() const
{
    return SettingsManager::instance().value("Input/Mouse/Enabled", false).toBool();
}

// Setters
void InputSettings::setControllerType(int port, ControllerType type)
{
    if (getControllerType(port) != type) {
        SettingsManager::instance().setValue(controllerSettingsKey(port, "Type"), static_cast<int>(type));
        emit controllerConfigChanged(port);
    }
}

void InputSettings::setControllerPluggedIn(int port, bool pluggedIn)
{
    if (isControllerPluggedIn(port) != pluggedIn) {
        SettingsManager::instance().setValue(controllerSettingsKey(port, "PluggedIn"), pluggedIn);
        emit controllerConfigChanged(port);
    }
}

void InputSettings::setPeripheralType(int port, PeripheralType type)
{
    if (getPeripheralType(port) != type) {
        SettingsManager::instance().setValue(controllerSettingsKey(port, "Peripheral"), static_cast<int>(type));
        emit controllerConfigChanged(port);
    }
}

void InputSettings::setButtonMapping(int port, const QString& button, const QString& mapping)
{
    if (getButtonMapping(port, button) != mapping) {
        SettingsManager::instance().setValue(controllerSettingsKey(port, QString("Button_%1").arg(button)), mapping);
        emit buttonMappingChanged(port, button);
    }
}

void InputSettings::setAllButtonMappings(int port, const QMap<QString, QString>& mappings)
{
    for (auto it = mappings.constBegin(); it != mappings.constEnd(); ++it) {
        setButtonMapping(port, it.key(), it.value());
    }
}

void InputSettings::setDeadzone(int port, int deadzone)
{
    if (getDeadzone(port) != deadzone) {
        SettingsManager::instance().setValue(controllerSettingsKey(port, "Deadzone"), deadzone);
        emit controllerConfigChanged(port);
    }
}

void InputSettings::setSensitivity(int port, int sensitivity)
{
    if (getSensitivity(port) != sensitivity) {
        SettingsManager::instance().setValue(controllerSettingsKey(port, "Sensitivity"), sensitivity);
        emit controllerConfigChanged(port);
    }
}

void InputSettings::setInvertXAxis(int port, bool invert)
{
    if (invertXAxis(port) != invert) {
        SettingsManager::instance().setValue(controllerSettingsKey(port, "InvertX"), invert);
        emit controllerConfigChanged(port);
    }
}

void InputSettings::setInvertYAxis(int port, bool invert)
{
    if (invertYAxis(port) != invert) {
        SettingsManager::instance().setValue(controllerSettingsKey(port, "InvertY"), invert);
        emit controllerConfigChanged(port);
    }
}

void InputSettings::setRumbleStrength(int port, int strength)
{
    if (getRumbleStrength(port) != strength) {
        SettingsManager::instance().setValue(controllerSettingsKey(port, "RumbleStrength"), strength);
        emit rumbleSettingsChanged(port);
    }
}

void InputSettings::setRumbleEnabled(int port, bool enabled)
{
    if (isRumbleEnabled(port) != enabled) {
        SettingsManager::instance().setValue(controllerSettingsKey(port, "RumbleEnabled"), enabled);
        emit rumbleSettingsChanged(port);
    }
}

void InputSettings::setMouseSensitivity(double sensitivity)
{
    if (getMouseSensitivity() != sensitivity) {
        SettingsManager::instance().setValue("Input/Mouse/Sensitivity", sensitivity);
        emit mouseSettingsChanged();
    }
}

void InputSettings::setInvertMouseX(bool invert)
{
    if (invertMouseX() != invert) {
        SettingsManager::instance().setValue("Input/Mouse/InvertX", invert);
        emit mouseSettingsChanged();
    }
}

void InputSettings::setInvertMouseY(bool invert)
{
    if (invertMouseY() != invert) {
        SettingsManager::instance().setValue("Input/Mouse/InvertY", invert);
        emit mouseSettingsChanged();
    }
}

void InputSettings::setEnableMouseInjector(bool enable)
{
    if (enableMouseInjector() != enable) {
        SettingsManager::instance().setValue("Input/Mouse/Enabled", enable);
        emit mouseSettingsChanged();
    }
}

// Controller pak methods
void InputSettings::createMemoryCard(int port)
{
    // Implementation would create a memory card file
    // Stub implementation - actual implementation would create the file
    qDebug() << "Creating memory card for port" << port;
    emit controllerConfigChanged(port);
}

void InputSettings::transferGameBoyRom(int port, const QString& romPath)
{
    // Implementation would handle Transfer Pak GB ROM loading
    // Stub implementation - actual implementation would handle the transfer
    qDebug() << "Transferring GB ROM" << romPath << "to port" << port;
    emit controllerConfigChanged(port);
}

void InputSettings::loadSettings()
{
    // Default settings are loaded via the getters
}

void InputSettings::saveSettings()
{
    SettingsManager::instance().sync();
}

} // namespace QT_UI
