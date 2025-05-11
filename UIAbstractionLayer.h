#pragma once

#include <QObject>
#include <QString>

namespace QT_UI {

/**
 * @brief Provides an abstraction layer between the emulation core and Qt UI
 * 
 * This class follows the singleton pattern and serves as a bridge between
 * the Qt UI components and the Project64 emulation core.
 */
class UIAbstractionLayer : public QObject
{
    Q_OBJECT
    
public:
    /**
     * @brief Gets the singleton instance
     * @return Reference to the UIAbstractionLayer instance
     */
    static UIAbstractionLayer& instance();
    
    /**
     * @brief Initializes the abstraction layer, preparing it for use
     * @return True if initialization was successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Loads a ROM file
     * @param romPath Path to the ROM file
     * @return True if successfully started loading, false otherwise
     */
    bool loadROM(const QString& romPath);
    
    /**
     * @brief Starts emulation of the currently loaded ROM
     * @return True if successfully started, false otherwise
     */
    bool startEmulation();
    
    /**
     * @brief Pauses the currently running emulation
     * @return True if successfully paused, false otherwise
     */
    bool pauseEmulation();
    
    /**
     * @brief Stops the currently running emulation
     * @return True if successfully stopped, false otherwise
     */
    bool stopEmulation();
    
    /**
     * @brief Resets the emulation
     * @param hardReset True for a hard reset, false for soft reset
     * @return True if successfully reset, false otherwise
     */
    bool resetEmulation(bool hardReset);
    
signals:
    /**
     * @brief Emitted when a ROM is loaded
     * @param romPath Path to the loaded ROM
     */
    void romLoaded(const QString& romPath);
    
    /**
     * @brief Emitted when emulation starts
     */
    void emulationStarted();
    
    /**
     * @brief Emitted when emulation is paused
     */
    void emulationPaused();
    
    /**
     * @brief Emitted when emulation is stopped
     */
    void emulationStopped();
    
    /**
     * @brief Emitted to display status messages
     * @param message The message to display
     * @param timeout How long to display the message (in milliseconds, 0 for no timeout)
     */
    void messageDisplayed(const QString& message, int timeout = 0);
    
private:
    // Private constructor for singleton
    UIAbstractionLayer();
    ~UIAbstractionLayer();
    
    // Prevent copying
    UIAbstractionLayer(const UIAbstractionLayer&) = delete;
    UIAbstractionLayer& operator=(const UIAbstractionLayer&) = delete;
};

} // namespace QT_UI
