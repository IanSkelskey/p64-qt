#include "MainWindow.h"
#include "../Core/Settings/SettingsManager.h"
#include "../Core/Settings/ApplicationSettings.h"
// Remove UI include
// #include "ui_mainwindow.h"

#include "Emulation/EmulationViewport.h"
#include "RomBrowser/RomBrowserWidget.h"
#include "Settings/ConfigDialog.h"
#include "Settings/GraphicsSettingsDialog.h"
#include "About/AboutDialog.h"
#include "Tools/CoverDownloader.h"
#include "Theme/ThemeManager.h"
#include "Theme/IconHelper.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>
#include <QIcon>
#include <QResource>
#include <QDesktopServices>
#include <QStackedWidget>
#include <QTimer>
#include <QMenuBar>
#include <QMenu>
#include <QStyleFactory>
#include <QDebug>
#include <QActionGroup>
#include <QVBoxLayout>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    // Remove UI initialization
    // , ui(new Ui::MainWindow)
    , menuCreated(false)
    , detailViewAction(nullptr)
    , gridViewAction(nullptr)
    , viewModeGroup(nullptr)
    , m_aboutDialog(nullptr)
    , m_configDialog(nullptr)
    , m_coverDownloader(nullptr)
    , m_graphicsSettingsDialog(nullptr) // Initialize to nullptr
{
    // Set up the basic window properties instead of using UI
    // ui->setupUi(this);
    setWindowTitle("Project64");
    setWindowIcon(QIcon(":/icons/pj64.ico"));
    resize(800, 600);
    setMinimumSize(640, 480);
    
    // Create status bar
    statusBar();
    
    // Set up central widget with layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setCentralWidget(centralWidget);
    
    // Create central widgets first
    createCentralWidget();
    
    // Initialize menu-related member variables
    startEmulationAction = nullptr;
    endEmulationAction = nullptr;
    romInfoAction = nullptr;
    fullscreenAction = nullptr;
    alwaysOnTopAction = nullptr;
    graphicsSettingsAction = nullptr;
    audioSettingsAction = nullptr;
    rspSettingsAction = nullptr;
    inputSettingsAction = nullptr;
    
    // Initialize view mode action group
    viewModeGroup = new QActionGroup(this);
    viewModeGroup->setExclusive(true);
    
    // Force rebuild the menu bar completely
    createMenuBar();
    
    // Connect menu actions
    connectMenuActions();
    
    // Other initialization
    loadSettings();
    
    // Update UI state after initialization
    updateUIState(false);
    
    // Connect theme change signal to update UI
    connect(&QT_UI::ThemeManager::instance(), &QT_UI::ThemeManager::themeChanged,
            this, [this](QT_UI::ThemeManager::Theme theme) {
                // Update UI when theme changes
                createMenuBar(); // Recreate menu with correct icons
                
                // Make sure the ROM browser updates its styles
                if (romBrowserWidget) {
                    romBrowserWidget->setupStatusArea();
                }
            });
}

void MainWindow::createMenuBar()
{
    // Remove existing menu bar if any
    QMenuBar* oldMenuBar = menuBar();
    if (oldMenuBar) {
        setMenuBar(nullptr);
        delete oldMenuBar;
    }
    
    // Create a new menu bar
    QMenuBar* mainMenuBar = new QMenuBar(this);
    mainMenuBar->setNativeMenuBar(false);
    
    // Apply enhanced stylesheet to add padding around menu items and icons
    mainMenuBar->setStyleSheet(
        "QMenuBar { padding: 2px; margin: 0px; }"
        "QMenuBar::item { padding: 4px 10px; margin: 1px; }"
        "QMenu { padding: 2px; }"
        "QMenu::item { padding: 6px 26px 6px 26px; }"
        "QMenu::item:selected { background-color: palette(highlight); color: palette(highlighted-text); }"
        "QMenu::icon { padding-left: 8px; margin-right: 6px; }" // Add padding to the left of icons and margin on right
        "QToolBar::separator { width: 6px; }"  // Add spacing between toolbar items
        // Set icon size in the stylesheet
        "QMenu::icon { width: 20px; height: 20px; }"
    );
    
    setMenuBar(mainMenuBar);
    
    // Define icon size to use throughout menu creation
    QSize iconSize(20, 20);
    
    // Create File menu
    QMenu* fileMenu = mainMenuBar->addMenu(tr("&File"));
    
    // Create file menu actions with theme icons
    QAction* openRomAction = new QAction(QT_UI::IconHelper::getOpenIcon(), tr("Open ROM"), this);
    openRomAction->setShortcut(QKeySequence("Ctrl+O"));
    connect(openRomAction, &QAction::triggered, this, &MainWindow::onOpenROM);
    
    QAction* openComboAction = new QAction(QT_UI::IconHelper::getCombinedFilesIcon(), tr("Open Combo"), this);
    openComboAction->setShortcut(QKeySequence("Ctrl+Shift+O"));
    
    QAction* romInfoAction = new QAction(QT_UI::IconHelper::getRomInfoIcon(), tr("ROM Info..."), this);
    romInfoAction->setEnabled(false);
    
    QAction* startEmulationAction = new QAction(QT_UI::IconHelper::getPlayIcon(), tr("Start Emulation"), this);
    startEmulationAction->setShortcut(QKeySequence("F11"));
    startEmulationAction->setEnabled(false);
    connect(startEmulationAction, &QAction::triggered, this, &MainWindow::onStartEmulation);
    
    QAction* endEmulationAction = new QAction(QT_UI::IconHelper::getStopIcon(), tr("End Emulation"), this);
    endEmulationAction->setEnabled(false);
    connect(endEmulationAction, &QAction::triggered, this, &MainWindow::onEndEmulation);
    
    QAction* chooseRomDirAction = new QAction(QT_UI::IconHelper::getFolderSettingsIcon(), tr("Choose ROM Directory..."), this);
    connect(chooseRomDirAction, &QAction::triggered, this, &MainWindow::onChooseRomDirectory);
    
    QAction* refreshRomListAction = new QAction(QT_UI::IconHelper::getRefreshIcon(), tr("Refresh ROM List"), this);
    refreshRomListAction->setShortcut(QKeySequence("F5"));
    connect(refreshRomListAction, &QAction::triggered, this, &MainWindow::onRefreshRomList);
    
    QAction* exitAction = new QAction(QT_UI::IconHelper::getExitIcon(), tr("Exit"), this);
    connect(exitAction, &QAction::triggered, this, &MainWindow::onExit);
    
    // Add actions to file menu
    fileMenu->addAction(openRomAction);
    fileMenu->addAction(openComboAction);
    fileMenu->addAction(romInfoAction);
    fileMenu->addSeparator();
    fileMenu->addAction(startEmulationAction);
    fileMenu->addAction(endEmulationAction);
    fileMenu->addSeparator();
    
    // Add Language submenu
    QMenu* languageMenu = fileMenu->addMenu(tr("Language"));
    languageMenu->setIcon(QT_UI::IconHelper::getGlobalIcon());
    QAction* availableLanguagesAction = new QAction(QT_UI::IconHelper::getGlobalIcon(), tr("(Available Languages)"), this);
    languageMenu->addAction(availableLanguagesAction);
    
    fileMenu->addSeparator();
    fileMenu->addAction(chooseRomDirAction);
    fileMenu->addAction(refreshRomListAction);
    fileMenu->addSeparator();
    
    // Add Recent ROM submenu
    QMenu* recentRomMenu = fileMenu->addMenu(tr("Recent ROM"));
    recentRomMenu->setIcon(QT_UI::IconHelper::getCartridgeIcon());
    QAction* recentRomsAction = new QAction(QT_UI::IconHelper::getCartridgeIcon(), tr("(Recent ROMs)"), this);
    recentRomMenu->addAction(recentRomsAction);
    
    // Add Recent ROM Directories submenu
    QMenu* recentRomDirsMenu = fileMenu->addMenu(tr("Recent ROM Directories"));
    recentRomDirsMenu->setIcon(QT_UI::IconHelper::getFolderIcon());
    QAction* recentRomDirsAction = new QAction(QT_UI::IconHelper::getFolderIcon(), tr("(Recent ROM Directories)"), this);
    recentRomDirsMenu->addAction(recentRomDirsAction);
    
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);
    
    // Create Options menu
    QMenu* optionsMenu = mainMenuBar->addMenu(tr("Options"));
    
    QAction* fullscreenAction = new QAction(QT_UI::IconHelper::getFullscreenIcon(), tr("Fullscreen"), this);
    fullscreenAction->setShortcut(QKeySequence("Alt+Return"));
    fullscreenAction->setEnabled(false);
    connect(fullscreenAction, &QAction::triggered, this, &MainWindow::onToggleFullscreen);
    
    QAction* alwaysOnTopAction = new QAction(QT_UI::IconHelper::getPinIcon(), tr("Always on top"), this);
    alwaysOnTopAction->setCheckable(true);
    alwaysOnTopAction->setEnabled(false);
    connect(alwaysOnTopAction, &QAction::triggered, this, &MainWindow::onToggleAlwaysOnTop);
    
    QAction* graphicsSettingsAction = new QAction(QT_UI::IconHelper::getGraphicsIcon(), tr("Graphics Settings"), this);
    // Change this line from false to true to enable the menu option
    graphicsSettingsAction->setEnabled(true);
    connect(graphicsSettingsAction, &QAction::triggered, this, &MainWindow::onGraphicsSettings);
    
    QAction* audioSettingsAction = new QAction(QT_UI::IconHelper::getAudioIcon(), tr("Audio Settings"), this);
    audioSettingsAction->setEnabled(false);
    connect(audioSettingsAction, &QAction::triggered, this, &MainWindow::onAudioSettings);
    
    QAction* rspSettingsAction = new QAction(QT_UI::IconHelper::getRspIcon(), tr("RSP Settings"), this);
    rspSettingsAction->setEnabled(false);
    connect(rspSettingsAction, &QAction::triggered, this, &MainWindow::onRSPSettings);
    
    QAction* inputSettingsAction = new QAction(QT_UI::IconHelper::getControllerIcon(), tr("Input Settings"), this);
    inputSettingsAction->setEnabled(false);
    connect(inputSettingsAction, &QAction::triggered, this, &MainWindow::onInputSettings);
    
    QAction* showCpuStatsAction = new QAction(QT_UI::IconHelper::getStatisticsIcon(), tr("Show CPU Stats"), this);
    showCpuStatsAction->setCheckable(true);
    connect(showCpuStatsAction, &QAction::triggered, this, &MainWindow::onShowCPUStats);
    
    QAction* configAction = new QAction(QT_UI::IconHelper::getSettingsIcon(), tr("Configuration"), this);
    connect(configAction, &QAction::triggered, this, &MainWindow::onConfiguration);
    
    optionsMenu->addAction(fullscreenAction);
    optionsMenu->addAction(alwaysOnTopAction);
    optionsMenu->addSeparator();
    optionsMenu->addAction(graphicsSettingsAction);
    optionsMenu->addAction(audioSettingsAction);
    optionsMenu->addAction(rspSettingsAction);
    optionsMenu->addAction(inputSettingsAction);
    optionsMenu->addSeparator();
    optionsMenu->addAction(showCpuStatsAction);
    optionsMenu->addAction(configAction);
    
    // Create Debugger menu
    QMenu* debuggerMenu = mainMenuBar->addMenu(tr("&Debugger"));
    
    // Create View menu
    QMenu* viewMenu = mainMenuBar->addMenu(tr("&View"));
    
    // Create view mode actions if they don't exist
    detailViewAction = new QAction(QT_UI::IconHelper::getDetailViewIcon(), tr("&Detail View"), this);
    detailViewAction->setCheckable(true);
    detailViewAction->setChecked(true);
    connect(detailViewAction, &QAction::triggered, this, &MainWindow::onSetDetailView);
    
    gridViewAction = new QAction(QT_UI::IconHelper::getGridViewIcon(), tr("&Grid View"), this);
    gridViewAction->setCheckable(true);
    connect(gridViewAction, &QAction::triggered, this, &MainWindow::onSetGridView);
    
    // Add actions to group for mutual exclusivity
    viewModeGroup->addAction(detailViewAction);
    viewModeGroup->addAction(gridViewAction);
    
    // Add view actions to menu
    viewMenu->addAction(detailViewAction);
    viewMenu->addAction(gridViewAction);
    
    // Add Show Titles action
    showTitlesAction = new QAction(QT_UI::IconHelper::getShowTitlesIcon(), tr("Show &Titles"), this);
    showTitlesAction->setCheckable(true);
    showTitlesAction->setChecked(true);
    connect(showTitlesAction, &QAction::triggered, this, &MainWindow::onToggleShowTitles);
    viewMenu->addAction(showTitlesAction);
    
    viewMenu->addSeparator();
    
    // Add Theme submenu
    QMenu* themeMenu = viewMenu->addMenu(QT_UI::IconHelper::getThemeIcon(), tr("Theme"));
    
    // Create action group for theme selection (mutually exclusive)
    QActionGroup* themeGroup = new QActionGroup(this);
    themeGroup->setExclusive(true);
    
    QAction* darkModeAction = new QAction(tr("Dark Mode"), this);
    darkModeAction->setCheckable(true);
    darkModeAction->setActionGroup(themeGroup);
    connect(darkModeAction, &QAction::triggered, this, &MainWindow::onSetDarkMode);
    
    QAction* lightModeAction = new QAction(tr("Light Mode"), this);
    lightModeAction->setCheckable(true);
    lightModeAction->setActionGroup(themeGroup);
    connect(lightModeAction, &QAction::triggered, this, &MainWindow::onSetLightMode);
    
    QAction* useSystemPrefsAction = new QAction(tr("Use System Prefs"), this);
    useSystemPrefsAction->setCheckable(true);
    useSystemPrefsAction->setActionGroup(themeGroup);
    useSystemPrefsAction->setChecked(true);
    connect(useSystemPrefsAction, &QAction::triggered, this, &MainWindow::onUseSystemPrefs);
    
    // Check the appropriate action based on current theme
    switch (QT_UI::ThemeManager::instance().currentTheme()) {
    case QT_UI::ThemeManager::DarkTheme:
        darkModeAction->setChecked(true);
        break;
    case QT_UI::ThemeManager::LightTheme:
        lightModeAction->setChecked(true);
        break;
    case QT_UI::ThemeManager::SystemTheme:
    default:
        useSystemPrefsAction->setChecked(true);
        break;
    }
    
    themeMenu->addAction(darkModeAction);
    themeMenu->addAction(lightModeAction);
    themeMenu->addAction(useSystemPrefsAction);
    
    // Create Tools menu
    QMenu* toolsMenu = mainMenuBar->addMenu(tr("Tools"));
    
    QAction* coverDownloaderAction = new QAction(QT_UI::IconHelper::getDownloadIcon(), tr("Cover Downloader"), this);
    connect(coverDownloaderAction, &QAction::triggered, this, &MainWindow::onCoverDownloader);
    toolsMenu->addAction(coverDownloaderAction);
    
    // Create Help menu
    QMenu* helpMenu = mainMenuBar->addMenu(tr("Help"));
    
    QAction* supportAction = new QAction(QT_UI::IconHelper::getSupportIcon(), tr("Support Project64"), this);
    connect(supportAction, &QAction::triggered, this, &MainWindow::onSupportProject64);
    
    QAction* discordAction = new QAction(QT_UI::IconHelper::getDiscordIcon(), tr("Discord"), this);
    connect(discordAction, &QAction::triggered, this, &MainWindow::onDiscord);
    
    QAction* websiteAction = new QAction(QT_UI::IconHelper::getWebsiteIcon(), tr("Website"), this);
    connect(websiteAction, &QAction::triggered, this, &MainWindow::onWebsite);
    
    QAction* aboutAction = new QAction(QT_UI::IconHelper::getAboutIcon(), tr("About Project64"), this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
    
    helpMenu->addAction(supportAction);
    helpMenu->addAction(discordAction);
    helpMenu->addAction(websiteAction);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAction);
    
    // Store references to important actions
    this->startEmulationAction = startEmulationAction;
    this->endEmulationAction = endEmulationAction;
    this->romInfoAction = romInfoAction;
    this->fullscreenAction = fullscreenAction;
    this->alwaysOnTopAction = alwaysOnTopAction;
    this->graphicsSettingsAction = graphicsSettingsAction;
    this->audioSettingsAction = audioSettingsAction;
    this->rspSettingsAction = rspSettingsAction;
    this->inputSettingsAction = inputSettingsAction;
    
    // Force update
    mainMenuBar->setVisible(true);
    menuCreated = true;
    
    // Debug output
    qDebug() << "Menu bar created with" << mainMenuBar->actions().count() << "menus";
    for (QAction* action : mainMenuBar->actions()) {
        qDebug() << "  Menu:" << action->text();
    }
}

void MainWindow::ensureMenuBarVisible()
{
    // Make sure menu bar is visible if it exists
    QMenuBar* mainMenuBar = menuBar();
    if (mainMenuBar) {
        mainMenuBar->setVisible(true);
        mainMenuBar->setNativeMenuBar(false);
        mainMenuBar->raise();
        
        // Debug output to check menu bar status
        qDebug() << "Menu bar exists:" << (mainMenuBar != nullptr);
        qDebug() << "Menu bar is visible:" << mainMenuBar->isVisible();
        qDebug() << "Menu count:" << mainMenuBar->actions().count();
        
        // Create menus if needed
        if (mainMenuBar->actions().isEmpty() && !menuCreated) {
            createMenuBar();
        }
    } else if (!menuCreated) {
        createMenuBar();
    }
}

MainWindow::~MainWindow()
{
    saveSettings();
    
    // Clean up dialog instances
    if (m_aboutDialog) {
        delete m_aboutDialog;
        m_aboutDialog = nullptr;
    }
    
    if (m_configDialog) {
        delete m_configDialog;
        m_configDialog = nullptr;
    }
    
    if (m_coverDownloader) {
        delete m_coverDownloader;
        m_coverDownloader = nullptr;
    }
    
    if (m_graphicsSettingsDialog) {
        delete m_graphicsSettingsDialog;
        m_graphicsSettingsDialog = nullptr;
    }
}

void MainWindow::initialize()
{
    romBrowserWidget->initialize();
}

void MainWindow::connectMenuActions()
{
    // Connect ROM browser signals
    if (romBrowserWidget) {
        connect(romBrowserWidget, &QT_UI::RomBrowserWidget::romDoubleClicked, 
                this, &MainWindow::onStartEmulation);
        
        // Update menu actions when ROM browser view mode changes
        connect(romBrowserWidget, &QT_UI::RomBrowserWidget::layoutChange, this, [this]() {
            if (romBrowserWidget->viewMode() == QT_UI::RomListModel::DetailView) {
                if (detailViewAction) detailViewAction->setChecked(true);
                if (gridViewAction) gridViewAction->setChecked(false);
                
                // Hide Show Titles action when in detail view
                if (showTitlesAction) showTitlesAction->setVisible(false);
            } else {
                if (detailViewAction) detailViewAction->setChecked(false);
                if (gridViewAction) gridViewAction->setChecked(true);
                
                // Show the Show Titles action when in grid view
                if (showTitlesAction) {
                    showTitlesAction->setVisible(true);
                    showTitlesAction->setChecked(romBrowserWidget->showTitles());
                }
            }
        });
        
        // Connect to the show titles toggled signal
        connect(romBrowserWidget, &QT_UI::RomBrowserWidget::showTitlesToggled,
                this, [this](bool show) {
            if (showTitlesAction) {
                showTitlesAction->setChecked(show);
            }
        });
    }

    // Remove incorrect disconnect that causes compilation error
    // disconnect(this, &MainWindow::triggered, nullptr, nullptr);
    
    // Debug the number of actions found
    qDebug() << "Finding actions in the UI...";
    
    // Connect menu actions by finding them in the UI
    const QList<QAction*> actions = findChildren<QAction*>();
    qDebug() << "Found" << actions.count() << "actions in the UI";
    
    for (QAction* action : actions) {
        // Instead of disconnecting and reconnecting which could cause issues,
        // just ensure we don't have duplicate connections by using Qt::UniqueConnection
        
        qDebug() << "Action:" << action->text() << "Object name:" << action->objectName();
        
        if (action->objectName() == "actionOpen_ROM" || action->text().contains("Open ROM")) {
            connect(action, &QAction::triggered, this, &MainWindow::onOpenROM, Qt::UniqueConnection);
        }
        else if (action->objectName() == "actionExit" || action->text().contains("Exit")) {
            connect(action, &QAction::triggered, this, &MainWindow::onExit, Qt::UniqueConnection);
        }
        else if (action->objectName() == "actionConfiguration" || action->text().contains("Configuration")) {
            connect(action, &QAction::triggered, this, &MainWindow::onConfiguration, Qt::UniqueConnection);
        }
        else if (action->objectName() == "actionCover_Downloader" || action->text().contains("Cover Downloader")) {
            connect(action, &QAction::triggered, this, &MainWindow::onCoverDownloader, Qt::UniqueConnection);
        }
        else if (action->objectName() == "actionAbout_Project64" || action->text().contains("About Project64")) {
            connect(action, &QAction::triggered, this, &MainWindow::onAbout, Qt::UniqueConnection);
        }
        // Add more connections as needed
    }
}

void MainWindow::createCentralWidget()
{
    // Create stacked widget to switch between ROM browser and emulation
    centralStackedWidget = new QStackedWidget(this);
    
    // Add the centralStackedWidget to the main layout
    centralWidget()->layout()->addWidget(centralStackedWidget);
    
    // Create ROM browser widget
    romBrowserWidget = new QT_UI::RomBrowserWidget(this);
    centralStackedWidget->addWidget(romBrowserWidget);
    
    // Create emulation viewport
    emulationViewport = new QT_UI::EmulationViewport(this);
    centralStackedWidget->addWidget(emulationViewport);
    
    // Connect ROM selection signal
    connect(romBrowserWidget, &QT_UI::RomBrowserWidget::romSelected, 
            this, &MainWindow::onRomSelected);
            
    // Show ROM browser by default
    centralStackedWidget->setCurrentWidget(romBrowserWidget);
}

void MainWindow::loadSettings()
{
    auto& settings = QT_UI::SettingsManager::instance();
    
    // Load window geometry
    restoreGeometry(settings.application()->mainWindowGeometry());
    restoreState(settings.application()->mainWindowState());
}

void MainWindow::saveSettings()
{
    auto& settings = QT_UI::SettingsManager::instance();
    
    // Save window geometry
    settings.application()->setMainWindowGeometry(saveGeometry());
    settings.application()->setMainWindowState(saveState());
}

void MainWindow::updateUIState(bool romLoaded)
{
    // Set window title based on state
    if (romLoaded) {
        setWindowTitle("Project64 - ROM Loaded");
    } else {
        setWindowTitle("Project64");
    }
    
    // Update the enabled state of actions based on whether a ROM is loaded
    QList<QAction*> actions = findChildren<QAction*>();
    for (QAction* action : actions) {
        // Enable/disable specific actions based on ROM loaded state
        if (action->objectName() == "actionStart_Emulation" || 
            action->text() == "Start Emulation") {
            action->setEnabled(romLoaded);
        }
        else if (action->objectName() == "actionEnd_Emulation" || 
                 action->text() == "End Emulation") {
            action->setEnabled(romLoaded);
        }
        // Make sure Graphics Settings stays enabled regardless of ROM state
        else if (action->objectName() == "actionGraphics_Settings" || 
                 action->text() == "Graphics Settings") {
            action->setEnabled(true);
        }
        // Add more action state updates as needed
    }
    
    // Only show the Show Titles action when in grid view
    if (romBrowserWidget && showTitlesAction) {
        bool isGridView = (romBrowserWidget->viewMode() == QT_UI::RomListModel::GridView);
        showTitlesAction->setVisible(isGridView);
        if (isGridView) {
            showTitlesAction->setChecked(romBrowserWidget->showTitles());
        }
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    saveSettings();
    event->accept();
}

// File menu slots
void MainWindow::onOpenROM()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open ROM"),
                                                   QString(),
                                                   tr("N64 ROMs (*.z64 *.n64 *.v64);;All Files (*)"));
    if (!fileName.isEmpty()) {
        onRomSelected(fileName);
    }
}

void MainWindow::onOpenCombo()
{
    // Implementation for opening combo ROMs
}

void MainWindow::onRomInfo()
{
    // Show ROM info dialog
}

void MainWindow::onStartEmulation()
{
    if (emulationViewport->startEmulation()) {
        centralStackedWidget->setCurrentWidget(emulationViewport);
        updateUIState(true);
    }
}

void MainWindow::onEndEmulation()
{
    if (emulationViewport->stopEmulation()) {
        centralStackedWidget->setCurrentWidget(romBrowserWidget);
        updateUIState(false);
    }
}

void MainWindow::onChooseRomDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose ROM Directory"),
                                                   QString(),
                                                   QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty()) {
        // Set the ROM directory in the browser widget
        romBrowserWidget->setDirectory(dir); 
    }
}

void MainWindow::onRefreshRomList()
{
    romBrowserWidget->refresh();
}

void MainWindow::onExit()
{
    close();
}

// Options menu slots
void MainWindow::onToggleFullscreen()
{
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

void MainWindow::onToggleAlwaysOnTop()
{
    Qt::WindowFlags flags = windowFlags();
    if (flags & Qt::WindowStaysOnTopHint) {
        setWindowFlags(flags & ~Qt::WindowStaysOnTopHint);
    } else {
        setWindowFlags(flags | Qt::WindowStaysOnTopHint);
    }
    show(); // Need to show the window again after changing flags
}

void MainWindow::onGraphicsSettings()
{
    // Create a temporary dialog instance instead of caching it
    // This avoids potential issues with initialization and cleanup
    QT_UI::GraphicsSettingsDialog graphicsDialog(this);
    graphicsDialog.exec();
}

void MainWindow::onAudioSettings()
{
    // Show audio settings dialog
}

void MainWindow::onRSPSettings()
{
    // Show RSP settings dialog
}

void MainWindow::onInputSettings()
{
    // Show input settings dialog
}

void MainWindow::onShowCPUStats()
{
    // Toggle CPU stats display
}

void MainWindow::onConfiguration()
{
    // Create config dialog if it doesn't exist
    if (!m_configDialog) {
        createConfigDialog();
    }
    
    // Show the dialog
    showDialog(m_configDialog);
}

void MainWindow::createConfigDialog()
{
    // Create a new configuration dialog
    m_configDialog = new QT_UI::ConfigDialog(this);
    
    // Connect to the column settings changed signal
    connect(m_configDialog, &QT_UI::ConfigDialog::columnSettingsChanged, 
            this, &MainWindow::handleColumnSettingsChanged);
}

void MainWindow::handleColumnSettingsChanged()
{
    qDebug() << "Column settings changed signal received in MainWindow";
    
    // Update ROM Browser columns if it exists
    if (romBrowserWidget) {
        qDebug() << "Forcing reload of column settings in ROM browser";
        
        // Ensure settings are written to disk first
        QSettings settings("Project64", "QtUI");
        settings.sync();
        
        // Add a delay to ensure everything is saved properly
        QTimer::singleShot(250, this, [this]() {
            if (romBrowserWidget) {
                // Force a reload of the column settings
                romBrowserWidget->reloadColumnSettings();
                
                // Force the widget to redraw completely
                romBrowserWidget->update();
                
                // Also ensure the main window updates its layout
                QCoreApplication::processEvents();
            }
        });
    } else {
        qDebug() << "ROM Browser widget not available";
    }
}

// View menu slots
void MainWindow::onToggleRomBrowserView()
{
    if (romBrowserWidget) {
        if (detailViewAction && detailViewAction->isChecked()) {
            onSetGridView();
        } else {
            onSetDetailView();
        }
    }
}

void MainWindow::onSetDetailView()
{
    if (romBrowserWidget) {
        romBrowserWidget->setViewMode(QT_UI::RomListModel::DetailView);
        if (detailViewAction) detailViewAction->setChecked(true);
        if (gridViewAction) gridViewAction->setChecked(false);
    }
}

void MainWindow::onSetGridView()
{
    if (romBrowserWidget) {
        romBrowserWidget->setViewMode(QT_UI::RomListModel::GridView);
        if (detailViewAction) detailViewAction->setChecked(false);
        if (gridViewAction) gridViewAction->setChecked(true);
    }
}

void MainWindow::onSetDarkMode()
{
    QT_UI::ThemeManager::instance().setTheme(QT_UI::ThemeManager::DarkTheme);
}

void MainWindow::onSetLightMode()
{
    QT_UI::ThemeManager::instance().setTheme(QT_UI::ThemeManager::LightTheme);
}

void MainWindow::onUseSystemPrefs()
{
    QT_UI::ThemeManager::instance().setTheme(QT_UI::ThemeManager::SystemTheme);
}

// Tools menu slots
void MainWindow::onCoverDownloader()
{
    qDebug() << "Opening Cover Downloader dialog";
    if (!m_coverDownloader) {
        m_coverDownloader = new QT_UI::CoverDownloader(this);
        
        // Connect the coversDownloaded signal to refresh the ROM browser
        connect(m_coverDownloader, &QT_UI::CoverDownloader::coversDownloaded,
                this, &MainWindow::onCoversDownloaded);
    }
    showDialog(m_coverDownloader);
}

// Handle downloaded covers by refreshing the ROM browser
void MainWindow::onCoversDownloaded(int successCount)
{
    if (successCount > 0 && romBrowserWidget) {
        // Refresh the ROM browser to show the new covers
        romBrowserWidget->refreshCovers();
        
        // Show a status message
        statusBar()->showMessage(tr("%1 new cover(s) downloaded successfully").arg(successCount), 3000);
    }
}

// Help menu slots
void MainWindow::onSupportProject64()
{
    QDesktopServices::openUrl(QUrl("https://www.pj64-emu.com/support-project64"));
}

void MainWindow::onDiscord()
{
    QDesktopServices::openUrl(QUrl("https://discord.gg/project64"));
}

void MainWindow::onWebsite()
{
    QDesktopServices::openUrl(QUrl("https://www.pj64-emu.com"));
}

void MainWindow::onAbout()
{
    qDebug() << "Opening About dialog";
    if (!m_aboutDialog) {
        m_aboutDialog = new QT_UI::AboutDialog(this);
    }
    showDialog(m_aboutDialog);
}

// ROM browser slots
void MainWindow::onRomSelected(const QString& romPath)
{
    // Handle ROM selection
    qDebug() << "ROM selected:" << romPath;
    
    // Enable rom-specific actions
    updateUIState(true);
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    
    // Ensure menu is visible when window is shown
    QTimer::singleShot(0, this, &MainWindow::ensureMenuBarVisible);
}

// Add the onToggleShowTitles implementation
void MainWindow::onToggleShowTitles()
{
    if (romBrowserWidget && 
        romBrowserWidget->viewMode() == QT_UI::RomListModel::GridView) {
        bool showTitles = showTitlesAction->isChecked();
        romBrowserWidget->setShowTitles(showTitles);
    }
}

// Improved helper method to show dialogs consistently
void MainWindow::showDialog(QDialog* dialog)
{
    if (!dialog) return;

    // Ensure the dialog is properly prepared
    dialog->setModal(true);
    
    // First hide the dialog to make sure it's not already visible
    dialog->hide();
    
    // Then position it in the center of the parent
    dialog->move(this->frameGeometry().center() - dialog->rect().center());
    
    // Activate and raise it to the top
    dialog->activateWindow();
    dialog->raise();
    
    // For debugging
    qDebug() << "Showing dialog with pointer:" << dialog;
    
    // Show as a modal dialog - this blocks until the dialog is closed
    int result = dialog->exec();
    
    qDebug() << "Dialog closed with result:" << result;
}
