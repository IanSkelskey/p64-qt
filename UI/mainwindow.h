#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QShowEvent>
#include <QActionGroup>

// Forward declarations
class QStackedWidget;

namespace QT_UI {
    class RomBrowserWidget;
    class EmulationViewport;
    class AboutDialog;
    class ConfigDialog;
    class CoverDownloader;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    void initialize();

private slots:
    // File menu slots
    void onOpenROM();
    void onOpenCombo();
    void onRomInfo();
    void onStartEmulation();
    void onEndEmulation();
    void onChooseRomDirectory();
    void onRefreshRomList();
    void onExit();
    
    // Options menu slots
    void onToggleFullscreen();
    void onToggleAlwaysOnTop();
    void onGraphicsSettings();
    void onAudioSettings();
    void onRSPSettings();
    void onInputSettings();
    void onShowCPUStats();
    void onConfiguration();
    
    // View menu slots
    void onToggleRomBrowserView();
    void onSetDetailView();
    void onSetGridView();
    void onToggleShowTitles(); // New slot
    void onSetDarkMode();
    void onSetLightMode();
    void onUseSystemPrefs();
    
    // Tools menu slots
    void onCoverDownloader();
    
    // Help menu slots
    void onSupportProject64();
    void onDiscord();
    void onWebsite();
    void onAbout();
    
    // ROM browser slots
    void onRomSelected(const QString& romPath);
    
    // Helper slots for menu bar visibility
    void ensureMenuBarVisible();

private:
    void createCentralWidget();
    void createMenuBar();
    void connectMenuActions();
    void loadSettings();
    void saveSettings();
    void updateUIState(bool romLoaded);
    
    // Override events
    virtual void showEvent(QShowEvent* event) override;
    virtual void closeEvent(QCloseEvent* event) override;

private:
    QStackedWidget* centralStackedWidget;
    QT_UI::RomBrowserWidget* romBrowserWidget;
    QT_UI::EmulationViewport* emulationViewport;
    bool menuCreated;

    // Key menu actions that need to be accessed later
    QAction* startEmulationAction;
    QAction* endEmulationAction;
    QAction* romInfoAction;
    QAction* fullscreenAction;
    QAction* alwaysOnTopAction;
    QAction* graphicsSettingsAction;
    QAction* audioSettingsAction;
    QAction* rspSettingsAction;
    QAction* inputSettingsAction;
    
    // View menu actions and group
    QAction* detailViewAction;
    QAction* gridViewAction;
    QAction* showTitlesAction; // New action for showing titles
    QActionGroup* viewModeGroup;

    // Dialog instances
    QT_UI::AboutDialog* m_aboutDialog;
    QT_UI::ConfigDialog* m_configDialog;
    QT_UI::CoverDownloader* m_coverDownloader;

    // Helper method for showing dialogs
    void showDialog(QDialog* dialog);
};

#endif // MAINWINDOW_H
