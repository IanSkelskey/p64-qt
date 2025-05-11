#include "UIAbstractionLayer.h"
#include <QDebug>
#include <QFileInfo>

namespace QT_UI {

UIAbstractionLayer& UIAbstractionLayer::instance()
{
    static UIAbstractionLayer instance;
    return instance;
}

UIAbstractionLayer::UIAbstractionLayer()
{
    // Initialize connections to the emulation core will go here
}

UIAbstractionLayer::~UIAbstractionLayer()
{
    // Cleanup
}

bool UIAbstractionLayer::initialize()
{
    qDebug() << "Initializing UIAbstractionLayer";
    
    // Initialize resources and connections to the emulation core
    
    // Load any required settings
    
    // Set up any signal-slot connections needed for UI-core communication
    
    // Return true if initialization was successful
    return true;
}

bool UIAbstractionLayer::loadROM(const QString& romPath)
{
    // TODO: Implement actual ROM loading through emulation core
    
    // For now, just emit signals to demonstrate flow
    qDebug() << "UIAbstractionLayer: Loading ROM: " << romPath;
    
    QFileInfo fileInfo(romPath);
    if (!fileInfo.exists() || !fileInfo.isReadable()) {
        emit messageDisplayed(tr("Error: Cannot read ROM file"), 5000);
        return false;
    }
    
    // Emit signals for UI updates
    emit messageDisplayed(tr("Loading ROM: %1").arg(fileInfo.fileName()));
    emit romLoaded(romPath);
    
    // Auto-start emulation
    startEmulation();
    
    return true;
}

bool UIAbstractionLayer::startEmulation()
{
    // TODO: Implement actual emulation start through emulation core
    qDebug() << "UIAbstractionLayer: Starting emulation";
    
    emit messageDisplayed(tr("Starting emulation..."), 3000);
    emit emulationStarted();
    
    return true;
}

bool UIAbstractionLayer::pauseEmulation()
{
    // TODO: Implement actual emulation pause through emulation core
    qDebug() << "UIAbstractionLayer: Pausing emulation";
    
    emit messageDisplayed(tr("Emulation paused"), 3000);
    emit emulationPaused();
    
    return true;
}

bool UIAbstractionLayer::stopEmulation()
{
    // TODO: Implement actual emulation stop through emulation core
    qDebug() << "UIAbstractionLayer: Stopping emulation";
    
    emit messageDisplayed(tr("Emulation stopped"), 3000);
    emit emulationStopped();
    
    return true;
}

bool UIAbstractionLayer::resetEmulation(bool hardReset)
{
    // TODO: Implement actual emulation reset through emulation core
    qDebug() << "UIAbstractionLayer: Resetting emulation (hard reset: " << hardReset << ")";
    
    emit messageDisplayed(hardReset ? 
        tr("Hard reset performed") : 
        tr("Soft reset performed"), 3000);
    
    return true;
}

} // namespace QT_UI
