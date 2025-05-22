#pragma once

#include <QObject>
#include <QString>

// Forward declarations
class MainWindow;

namespace QT_UI {

/**
 * @brief Base notification interface
 *
 * Abstract interface for the notification system. Implemented by the Qt notification system.
 */
class CNotification {
public:
    virtual ~CNotification() = default;

    // Interface methods to be implemented
    virtual void DisplayError(const char* message) const = 0;
    virtual void DisplayWarning(const char* message) const = 0;
    virtual void DisplayMessage(const char* message, unsigned int timeout) const = 0;
    virtual void DisplayMessage2(const char* message) const = 0;
    
    virtual void FatalError(const char* message) const = 0;
    virtual void AddRecentDir(const char* path) = 0;
    virtual void AddRecentRom(const char* path) = 0;
    virtual void RefreshMenu(void) = 0;
    virtual void HideRomBrowser(void) = 0;
    virtual void ShowRomBrowser(void) = 0;
    virtual void BringToTop(void) = 0;
    virtual void ChangeFullScreen(void) const = 0;
    virtual void ExitFullScreen(void) const = 0;
    virtual void SetWindowCaption(const char* caption) const = 0;
    virtual void ShowStatusBar(bool ShowBar) const = 0;
    virtual void ProcessMessages(void) const = 0;
};

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
