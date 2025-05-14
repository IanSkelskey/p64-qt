#pragma once

#include <QObject>
#include <QString>
#include "../../Notification.h"

namespace QT_UI {

class MainWindow;

/**
 * @brief Qt implementation of the notification system
 * 
 * This class provides a Qt-based implementation of the notification system,
 * which uses signals and slots to communicate with the UI.
 */
class NotificationSystem : public QObject, public CNotification
{
    Q_OBJECT

public:
    /**
     * @brief Get the singleton instance of the notification system
     * @return Reference to the notification system
     */
    static NotificationSystem& instance();

    // CNotification interface implementation
    void DisplayError(const char* message) const override;
    void DisplayWarning(const char* message) const override;
    void DisplayMessage(const char* message, unsigned int timeout) const override;
    void DisplayMessage2(const char* message) const override;
    
    void FatalError(const char* message) const override;
    void AddRecentDir(const char* path) override;
    void AddRecentRom(const char* path) override;
    void RefreshMenu(void) override;
    void HideRomBrowser(void) override;
    void ShowRomBrowser(void) override;
    void BringToTop(void) override;
    void ChangeFullScreen(void) const override;
    void ExitFullScreen(void) const override;
    void SetWindowCaption(const char* caption) const override;
    void ShowStatusBar(bool ShowBar) const override;
    void ProcessMessages(void) const override;

    // Methods to connect to MainWindow
    void setMainWindow(MainWindow* mainWindow);

signals:
    // Signals that trigger UI updates
    void showError(const QString& message) const;
    void showWarning(const QString& message) const;
    void showMessage(const QString& message, int timeout) const;
    void showFatalError(const QString& message) const;
    
    void recentDirAdded(const QString& path) const;
    void recentRomAdded(const QString& path) const;
    void menuRefreshNeeded() const;
    void romBrowserVisibilityChanged(bool visible) const;
    void windowBroughtToTop() const;
    void fullScreenChanged(bool fullScreen) const;
    void captionChanged(const QString& caption) const;
    void statusBarVisibilityChanged(bool visible) const;

private:
    // Private constructor and destructor (singleton)
    NotificationSystem();
    ~NotificationSystem();
    
    // Prevent copying
    NotificationSystem(const NotificationSystem&) = delete;
    NotificationSystem& operator=(const NotificationSystem&) = delete;

    // Main window pointer
    MainWindow* m_mainWindow;
};

} // namespace QT_UI
