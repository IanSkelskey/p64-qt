#include "NotificationSystem.h"
#include <MainWindow.h>
#include <QApplication>
#include <QMessageBox>
#include <QFileInfo>

namespace QT_UI {

NotificationSystem& NotificationSystem::instance()
{
    static NotificationSystem instance;
    return instance;
}

NotificationSystem::NotificationSystem()
    : m_mainWindow(nullptr)
{
}

NotificationSystem::~NotificationSystem()
{
}

void NotificationSystem::setMainWindow(MainWindow* mainWindow)
{
    m_mainWindow = mainWindow;
    
    // Connect signals to slots in the main window
    if (m_mainWindow) {
        connect(this, &NotificationSystem::showError, 
                m_mainWindow, &MainWindow::onShowError);
        connect(this, &NotificationSystem::showWarning, 
                m_mainWindow, &MainWindow::onShowWarning);
        connect(this, &NotificationSystem::showMessage, 
                m_mainWindow, &MainWindow::onShowMessage);
        connect(this, &NotificationSystem::showFatalError, 
                m_mainWindow, &MainWindow::onShowFatalError);
        
        connect(this, &NotificationSystem::recentDirAdded, 
                m_mainWindow, &MainWindow::onRecentDirAdded);
        connect(this, &NotificationSystem::recentRomAdded, 
                m_mainWindow, &MainWindow::onRecentRomAdded);
        connect(this, &NotificationSystem::menuRefreshNeeded, 
                m_mainWindow, &MainWindow::onMenuRefreshNeeded);
        connect(this, &NotificationSystem::romBrowserVisibilityChanged, 
                m_mainWindow, &MainWindow::onRomBrowserVisibilityChanged);
        connect(this, &NotificationSystem::windowBroughtToTop, 
                m_mainWindow, &MainWindow::onWindowBroughtToTop);
        connect(this, &NotificationSystem::fullScreenChanged, 
                m_mainWindow, &MainWindow::onFullScreenChanged);
        connect(this, &NotificationSystem::captionChanged, 
                m_mainWindow, &MainWindow::onCaptionChanged);
        connect(this, &NotificationSystem::statusBarVisibilityChanged, 
                m_mainWindow, &MainWindow::onStatusBarVisibilityChanged);
    }
}

void NotificationSystem::DisplayError(const char* message) const
{
    if (m_mainWindow) {
        emit showError(QString::fromUtf8(message));
    } else {
        QMessageBox::critical(nullptr, tr("Error"), QString::fromUtf8(message));
    }
}

void NotificationSystem::DisplayWarning(const char* message) const
{
    if (m_mainWindow) {
        emit showWarning(QString::fromUtf8(message));
    } else {
        QMessageBox::warning(nullptr, tr("Warning"), QString::fromUtf8(message));
    }
}

void NotificationSystem::DisplayMessage(const char* message, unsigned int timeout) const
{
    if (m_mainWindow) {
        emit showMessage(QString::fromUtf8(message), timeout);
    } else {
        // If no main window, just display a message box
        QMessageBox::information(nullptr, tr("Information"), QString::fromUtf8(message));
    }
}

void NotificationSystem::DisplayMessage2(const char* message) const
{
    // Display with default timeout
    DisplayMessage(message, 5000);  // 5 seconds
}

void NotificationSystem::FatalError(const char* message) const
{
    if (m_mainWindow) {
        emit showFatalError(QString::fromUtf8(message));
    } else {
        QMessageBox::critical(nullptr, tr("Fatal Error"), QString::fromUtf8(message));
        QApplication::exit(1);
    }
}

void NotificationSystem::AddRecentDir(const char* path)
{
    emit recentDirAdded(QString::fromUtf8(path));
}

void NotificationSystem::AddRecentRom(const char* path)
{
    emit recentRomAdded(QString::fromUtf8(path));
}

void NotificationSystem::RefreshMenu(void)
{
    emit menuRefreshNeeded();
}

void NotificationSystem::HideRomBrowser(void)
{
    emit romBrowserVisibilityChanged(false);
}

void NotificationSystem::ShowRomBrowser(void)
{
    emit romBrowserVisibilityChanged(true);
}

void NotificationSystem::BringToTop(void)
{
    emit windowBroughtToTop();
}

void NotificationSystem::ChangeFullScreen(void) const
{
    emit fullScreenChanged(true);
}

void NotificationSystem::ExitFullScreen(void) const
{
    emit fullScreenChanged(false);
}

void NotificationSystem::SetWindowCaption(const char* caption) const
{
    emit captionChanged(QString::fromUtf8(caption));
}

void NotificationSystem::ShowStatusBar(bool showBar) const
{
    emit statusBarVisibilityChanged(showBar);
}

void NotificationSystem::ProcessMessages(void) const
{
    // Process pending events in the Qt event loop
    QApplication::processEvents();
}

} // namespace QT_UI
