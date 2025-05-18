#include "RomBrowserWidget.h"
#include "../../Core/Settings/SettingsManager.h"
#include "../../Core/Settings/RomBrowserSettings.h"
#include "../Theme/IconHelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSettings>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QToolButton>
#include <QAction>
#include <QLabel>
#include <QStandardPaths>
#include <QFont>
#include <QButtonGroup>
#include <QMenu>
#include <QApplication>
#include <QTimer>  // Add this include for QTimer

namespace QT_UI {

// Constants for grid view zoom
const int MIN_ZOOM = 50;   // 50% scale
const int MAX_ZOOM = 200;  // 200% scale
const int DEFAULT_ZOOM = 100;  // 100% scale

RomBrowserWidget::RomBrowserWidget(QWidget* parent)
    : QWidget(parent)
    , m_romListModel(nullptr)
    , m_proxyModel(nullptr)
    , m_gridDelegate(nullptr)
{
    // Create models
    m_romListModel = new RomListModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_romListModel);
    m_proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    
    // Create UI components
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    createToolbar();
    mainLayout->addWidget(m_toolbar);
    
    createViews();
    mainLayout->addWidget(m_viewStack);
    
    // Status bar at the bottom
    QHBoxLayout* statusLayout = new QHBoxLayout();
    statusLayout->setContentsMargins(5, 2, 5, 2);
    
    m_statusLabel = new QLabel(tr("Ready"), this);
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_progressBar->setTextVisible(true);
    m_progressBar->setMinimum(0);
    
    statusLayout->addWidget(m_statusLabel, 1);
    statusLayout->addWidget(m_progressBar);
    
    mainLayout->addLayout(statusLayout);
    
    // Set up status area styling
    setupStatusArea();
    
    // Create empty state widgets
    createEmptyState();
    
    // Set up the grid view with delegate
    setupGridView();
    
    // Establish connections
    createConnections();
    
    // Load saved settings
    loadSettings();
    
    // Update toolbar based on current view mode
    updateToolbar();
}

RomBrowserWidget::~RomBrowserWidget()
{
    saveSettings();
}

void RomBrowserWidget::refresh()
{
    m_romListModel->refresh();
}

void RomBrowserWidget::setDirectory(const QString& directory)
{
    if (directory.isEmpty())
        return;
    
    QFileInfo dirInfo(directory);
    if (!dirInfo.exists() || !dirInfo.isDir())
        return;
    
    m_currentDirectory = directory;
    m_romListModel->setRomDirectory(directory);
    
    // Update status label
    m_statusLabel->setText(tr("Current directory: %1").arg(directory));
    
    // Update empty state visibility
    updateEmptyStateVisibility();
}

QString RomBrowserWidget::getSelectedRomPath() const
{
    QModelIndex currentIndex;
    
    // Get the current selection from the active view
    if (m_romListModel->viewMode() == RomListModel::DetailView) {
        currentIndex = m_detailView->currentIndex();
    } else {  // GridView
        currentIndex = m_gridView->currentIndex();
    }
    
    if (!currentIndex.isValid())
        return QString();
    
    return m_proxyModel->data(currentIndex, Qt::UserRole).toString();
}

RomListModel::ViewMode RomBrowserWidget::viewMode() const
{
    return m_romListModel->viewMode();
}

void RomBrowserWidget::setViewMode(RomListModel::ViewMode mode)
{
    if (m_romListModel->viewMode() == mode)
        return;
    
    m_romListModel->setViewMode(mode);
    
    // Update the view stack
    if (mode == RomListModel::DetailView) {
        m_viewStack->setCurrentWidget(m_detailView);
        resizeDetailViewColumns();
    } else {  // GridView
        m_viewStack->setCurrentWidget(m_gridView);
        setupGridView();
    }
    
    // Update toolbar to show/hide grid view specific controls
    updateToolbar();
    
    // Notify other components of the layout change
    emit layoutChange();
}

void RomBrowserWidget::onViewModeToggled()
{
    // Toggle between detail and grid view
    if (m_romListModel->viewMode() == RomListModel::DetailView) {
        setViewMode(RomListModel::GridView);
        m_gridViewAction->setChecked(true);
        m_detailViewAction->setChecked(false);
    } else {
        setViewMode(RomListModel::DetailView);
        m_detailViewAction->setChecked(true);
        m_gridViewAction->setChecked(false);
    }
}

void RomBrowserWidget::onTitleVisibilityToggled()
{
    // Toggle the button's checked state directly
    // This will trigger the lambda connected to the toggled signal
    if (m_showTitlesAction) {
        m_showTitlesAction->setChecked(!m_showTitlesAction->isChecked());
    }
}

void RomBrowserWidget::onZoomValueChanged(int value)
{
    // Convert slider value (50-200) to scale (0.5-2.0)
    float scale = value / 100.0f;
    m_romListModel->setCoverScale(scale);
    
    // Update the zoom label
    m_zoomLabel->setText(QString("%1%").arg(value));
    
    // Update grid view to reflect new size
    setupGridView();
}

void RomBrowserWidget::onFilterTextChanged(const QString& text)
{
    m_proxyModel->setFilterFixedString(text);
}

void RomBrowserWidget::onSortIndicatorChanged(int column, Qt::SortOrder order)
{
    m_proxyModel->sort(column, order);
}

void RomBrowserWidget::onItemClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;
    
    QString romPath = m_proxyModel->data(index, Qt::UserRole).toString();
    emit romSelected(romPath);
}

void RomBrowserWidget::onItemDoubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;
    
    QString romPath = m_proxyModel->data(index, Qt::UserRole).toString();
    emit romDoubleClicked(romPath);
}

void RomBrowserWidget::onSetDirectoryClicked()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Select ROM Directory"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    if (!dir.isEmpty()) {
        setDirectory(dir);
    }
}

void RomBrowserWidget::onCoverDirectoryClicked()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Select Cover Art Directory"),
        m_romListModel->coverDirectory(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    if (!dir.isEmpty()) {
        m_romListModel->setCoverDirectory(dir);
    }
}

void RomBrowserWidget::refreshCovers()
{
    if (m_romListModel) {
        // Refresh the covers in the model
        m_romListModel->refreshCovers();
        
        // Make sure the views are updated
        if (viewMode() == RomListModel::GridView) {
            m_gridView->viewport()->update();
        } else {
            m_detailView->viewport()->update();
        }
    }
}

void RomBrowserWidget::onRefreshCoversClicked()
{
    refreshCovers();
}

void RomBrowserWidget::updateToolbar()
{
    // Update view mode buttons based on current mode
    bool isGridView = (m_romListModel->viewMode() == RomListModel::GridView);
    
    m_detailViewAction->setChecked(!isGridView);
    m_gridViewAction->setChecked(isGridView);
    
    // Show/hide grid view specific controls
    m_showTitlesAction->setVisible(isGridView);
    m_zoomSlider->setVisible(isGridView);
    m_zoomLabel->setVisible(isGridView);
    
    // Update the Show Titles action checked state
    m_showTitlesAction->setChecked(m_romListModel->showTitles());
    
    // Update zoom slider to match current scale
    int zoomValue = m_romListModel->coverScale() * 100;
    m_zoomSlider->setValue(zoomValue);
    m_zoomLabel->setText(QString("%1%").arg(zoomValue));
}

void RomBrowserWidget::onScanStarted()
{
    m_progressBar->setValue(0);
    m_progressBar->setVisible(true);
    m_statusLabel->setText(tr("Scanning for ROMs..."));
}

void RomBrowserWidget::onScanProgress(int current, int total)
{
    m_progressBar->setMaximum(total);
    m_progressBar->setValue(current);
    m_statusLabel->setText(tr("Scanning: %1 of %2").arg(current).arg(total));
}

void RomBrowserWidget::onScanFinished()
{
    m_progressBar->setVisible(false);
    int count = m_romListModel->rowCount();
    m_statusLabel->setText(tr("Found %n ROM(s)", "", count));
    
    // Update empty state visibility
    updateEmptyStateVisibility();
}

void RomBrowserWidget::createViews()
{
    // Create view stack for different view modes
    m_viewStack = new QStackedWidget(this);
    
    // Detail view (table-like with enhancements)
    m_detailView = new QTreeView(this);
    m_detailView->setModel(m_proxyModel);
    m_detailView->setSortingEnabled(true);
    m_detailView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_detailView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_detailView->setAlternatingRowColors(true);
    m_detailView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_detailView->header()->setSortIndicatorShown(true);
    m_detailView->setRootIsDecorated(false);
    m_detailView->setIconSize(QSize(32, 32));  // Show reasonable size icons
    m_viewStack->addWidget(m_detailView);
    
    // Ensure internal name and country columns are initially visible
    QVector<RomListModel::RomColumns> columns = m_romListModel->visibleColumns();
    if (!columns.contains(RomListModel::InternalName)) {
        columns.append(RomListModel::InternalName);
    }
    if (!columns.contains(RomListModel::Country)) {
        columns.append(RomListModel::Country);
    }
    m_romListModel->setVisibleColumns(columns);
    
    // Grid view for cover art
    m_gridView = new QListView(this);
    m_gridView->setModel(m_proxyModel);
    m_gridView->setViewMode(QListView::IconMode);
    m_gridView->setResizeMode(QListView::Adjust);
    m_gridView->setWrapping(true);
    m_gridView->setSpacing(15);
    m_gridView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_gridView->setUniformItemSizes(false);  // Items may vary in height if titles are shown
    m_viewStack->addWidget(m_gridView);
}

void RomBrowserWidget::createToolbar()
{
    m_toolbar = new QToolBar(tr("ROM Browser"), this);
    m_toolbar->setMovable(false);
    m_toolbar->setIconSize(QSize(20, 20));
    
    // Create view mode buttons with icons
    m_detailViewAction = m_toolbar->addAction(QT_UI::IconHelper::getDetailViewIcon(), 
                                             tr("Detail View"));
    m_detailViewAction->setCheckable(true);
    m_detailViewAction->setChecked(true);
    
    m_gridViewAction = m_toolbar->addAction(QT_UI::IconHelper::getGridViewIcon(), 
                                           tr("Grid View"));
    m_gridViewAction->setCheckable(true);
    
    m_toolbar->addSeparator();
    
    // Search field
    QLabel* searchLabel = new QLabel(tr("Search:"));
    m_toolbar->addWidget(searchLabel);
    
    m_searchBox = new QLineEdit(this);
    m_searchBox->setClearButtonEnabled(true);
    m_searchBox->setPlaceholderText(tr("Search ROMs..."));
    m_searchBox->setMinimumWidth(200);
    m_toolbar->addWidget(m_searchBox);
    
    m_toolbar->addSeparator();
    
    // Create a clear, distinct Show Titles button with icon
    m_showTitlesAction = new QAction(QT_UI::IconHelper::getShowTitlesIcon(), tr("Show Titles"), this);
    m_showTitlesAction->setCheckable(true);
    m_showTitlesAction->setChecked(m_romListModel ? m_romListModel->showTitles() : true);
    m_toolbar->addAction(m_showTitlesAction);
    
    // Connect the show titles action
    connect(m_showTitlesAction, &QAction::toggled, this, [this](bool checked) {
        if (m_romListModel) {
            m_romListModel->setShowTitles(checked);
            setupGridView();
            m_gridView->viewport()->update();
            emit showTitlesToggled(checked);
        }
    });
    
    // Zoom controls with slider and themed icons
    m_toolbar->addSeparator();
    
    // Create zoom out icon with proper theming
    m_zoomOutAction = new QAction(QT_UI::IconHelper::getZoomOutIcon(), "", this);
    m_zoomOutAction->setToolTip(tr("Zoom Out"));
    m_toolbar->addAction(m_zoomOutAction);
    
    m_zoomSlider = new QSlider(Qt::Horizontal, this);
    m_zoomSlider->setMinimum(MIN_ZOOM);
    m_zoomSlider->setMaximum(MAX_ZOOM);
    m_zoomSlider->setValue(DEFAULT_ZOOM);
    m_zoomSlider->setFixedWidth(100);
    m_toolbar->addWidget(m_zoomSlider);
    
    // Create zoom in icon with proper theming
    m_zoomInAction = new QAction(QT_UI::IconHelper::getZoomInIcon(), "", this);
    m_zoomInAction->setToolTip(tr("Zoom In"));
    m_toolbar->addAction(m_zoomInAction);
    
    m_zoomLabel = new QLabel("100%", this);
    m_zoomLabel->setFixedWidth(40);
    m_toolbar->addWidget(m_zoomLabel);
    
    m_toolbar->addSeparator();
    
    // Cover directory button with icon
    m_coverDirAction = m_toolbar->addAction(QT_UI::IconHelper::getFolderSettingsIcon(), tr("Cover Directory..."), this, &RomBrowserWidget::onCoverDirectoryClicked);
    
    // Refresh covers button with proper themed icon
    m_refreshCoversAction = m_toolbar->addAction(QT_UI::IconHelper::getRefreshIcon(), tr("Refresh Covers"), this, &RomBrowserWidget::onRefreshCoversClicked);
    
    // Connect theme change signal to update icons when theme changes
    connect(&QT_UI::ThemeManager::instance(), &QT_UI::ThemeManager::themeChanged,
            this, &RomBrowserWidget::updateToolbarIcons);
}

// Add a new method to update all toolbar icons when theme changes
void RomBrowserWidget::updateToolbarIcons()
{
    // Update toolbar action icons
    if (m_detailViewAction) m_detailViewAction->setIcon(QT_UI::IconHelper::getDetailViewIcon());
    if (m_gridViewAction) m_gridViewAction->setIcon(QT_UI::IconHelper::getGridViewIcon());
    if (m_showTitlesAction) m_showTitlesAction->setIcon(QT_UI::IconHelper::getShowTitlesIcon());
    if (m_zoomInAction) m_zoomInAction->setIcon(QT_UI::IconHelper::getZoomInIcon());
    if (m_zoomOutAction) m_zoomOutAction->setIcon(QT_UI::IconHelper::getZoomOutIcon());
    if (m_coverDirAction) m_coverDirAction->setIcon(QT_UI::IconHelper::getFolderSettingsIcon());
    if (m_refreshCoversAction) m_refreshCoversAction->setIcon(QT_UI::IconHelper::getRefreshIcon());
    
    // Force a repaint of the toolbar
    if (m_toolbar) m_toolbar->update();
}

void RomBrowserWidget::setupStatusArea()
{
    // Set object names for better styling
    m_statusLabel->setObjectName("statusLabel");
    m_progressBar->setObjectName("statusProgressBar");
    
    // Set a minimum height to ensure consistent appearance
    m_statusLabel->setMinimumHeight(24);
    m_progressBar->setMinimumHeight(18);
    
    // Use current palette colors to match the theme
    QString backgroundColor = palette().color(QPalette::Window).name();
    QString textColor = palette().color(QPalette::WindowText).name();
    
    // Create a style for the status area that matches the theme
    QString style = QString(
        "QLabel#statusLabel { color: %1; background-color: %2; padding: 2px; }"
    ).arg(textColor, backgroundColor);
    
    m_statusLabel->setStyleSheet(style);
}

void RomBrowserWidget::createEmptyState()
{
    // Create empty state widget to show when no directory is set or no ROMs found
    m_emptyStateWidget = new QWidget(this);
    m_viewStack->addWidget(m_emptyStateWidget);
    
    QVBoxLayout* emptyLayout = new QVBoxLayout(m_emptyStateWidget);
    emptyLayout->setAlignment(Qt::AlignCenter);
    
    // Add empty state content
    m_emptyStateLabel = new QLabel(this);
    m_emptyStateLabel->setAlignment(Qt::AlignCenter);
    QFont labelFont = m_emptyStateLabel->font();
    labelFont.setPointSize(14);
    m_emptyStateLabel->setFont(labelFont);
    m_emptyStateLabel->setText(tr("No ROMs found"));
    
    m_setDirButton = new QPushButton(tr("Select ROM Directory"), this);
    m_setDirButton->setMinimumWidth(200);
    
    emptyLayout->addStretch();
    emptyLayout->addWidget(m_emptyStateLabel);
    emptyLayout->addSpacing(20);
    emptyLayout->addWidget(m_setDirButton, 0, Qt::AlignCenter);
    emptyLayout->addStretch();
}

void RomBrowserWidget::createConnections()
{
    // View mode toggle buttons
    connect(m_detailViewAction, &QAction::triggered, this, [this]() {
        if (m_romListModel->viewMode() != RomListModel::DetailView) {
            setViewMode(RomListModel::DetailView);
        }
    });
    
    connect(m_gridViewAction, &QAction::triggered, this, [this]() {
        if (m_romListModel->viewMode() != RomListModel::GridView) {
            setViewMode(RomListModel::GridView);
        }
    });
    
    // Grid view controls
    connect(m_zoomSlider, &QSlider::valueChanged, this, &RomBrowserWidget::onZoomValueChanged);
    
    // Search and filter
    connect(m_searchBox, &QLineEdit::textChanged, this, &RomBrowserWidget::onFilterTextChanged);
    connect(m_detailView->header(), &QHeaderView::sortIndicatorChanged, this, &RomBrowserWidget::onSortIndicatorChanged);
    
    // Item selection
    connect(m_detailView, &QTreeView::clicked, this, &RomBrowserWidget::onItemClicked);
    connect(m_gridView, &QListView::clicked, this, &RomBrowserWidget::onItemClicked);
    
    connect(m_detailView, &QTreeView::doubleClicked, this, &RomBrowserWidget::onItemDoubleClicked);
    connect(m_gridView, &QListView::doubleClicked, this, &RomBrowserWidget::onItemDoubleClicked);
    
    // Empty state connections
    connect(m_setDirButton, &QPushButton::clicked, this, &RomBrowserWidget::onSetDirectoryClicked);
    
    // Model connections
    connect(m_romListModel, &RomListModel::scanStarted, this, &RomBrowserWidget::onScanStarted);
    connect(m_romListModel, &RomListModel::scanProgress, this, &RomBrowserWidget::onScanProgress);
    connect(m_romListModel, &RomListModel::scanFinished, this, &RomBrowserWidget::onScanFinished);
    
    // Layout changes
    connect(this, &RomBrowserWidget::layoutChange, this, [this]() {
        if (m_romListModel->viewMode() == RomListModel::GridView) {
            setupGridView();
        } else {
            resizeDetailViewColumns();
        }
    });
    
    // Connect to model's columnsChanged signal for direct updates
    connect(m_romListModel, &RomListModel::columnsChanged, this, [this]() {
        qDebug() << "Columns changed in model, updating views";
        if (m_romListModel->viewMode() == RomListModel::DetailView) {
            QTimer::singleShot(0, this, [this]() {
                m_detailView->reset();
                resizeDetailViewColumns();
                m_detailView->viewport()->update();
            });
        }
    });
}

void RomBrowserWidget::setupGridView()
{
    // Create delegate if not already created
    if (!m_gridDelegate) {
        m_gridDelegate = new RomGridDelegate(m_romListModel, this);
    }
    
    // Set the delegate for the grid view
    m_gridView->setItemDelegate(m_gridDelegate);
    
    // Calculate proper item size with room for title
    QSize coverSize = m_romListModel->coverSize();
    
    // Calculate a reasonable grid size based on content
    int itemWidth = coverSize.width() + 20;
    
    // Calculate height based on whether titles are shown
    int itemHeight = coverSize.height() + 15; // Base height with minimal padding
    if (m_romListModel->showTitles()) {
        // Add space for title and metadata (if any)
        itemHeight += 35; // Restored reasonable height for title area
    }
    
    // Set grid size with proper padding
    QSize gridSize(itemWidth, itemHeight);
    m_gridView->setGridSize(gridSize);
    
    // Reasonable spacing between items
    m_gridView->setSpacing(15);
    
    // Update the view display options
    m_gridView->setWordWrap(true);
    m_gridView->setTextElideMode(Qt::ElideRight);
    m_gridView->setDragEnabled(false);
    m_gridView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // Important: enable flow layout to use proper item sizes
    m_gridView->setResizeMode(QListView::Adjust);
    m_gridView->setUniformItemSizes(false);
    
    // Force a complete reset and refresh
    m_gridView->reset();
    m_gridView->doItemsLayout();
}

void RomBrowserWidget::updateEmptyStateVisibility()
{
    if (m_currentDirectory.isEmpty() || m_romListModel->rowCount() == 0) {
        // Show empty state widget
        if (m_currentDirectory.isEmpty()) {
            m_emptyStateLabel->setText(tr("No ROM directory selected"));
        } else {
            m_emptyStateLabel->setText(tr("No ROMs found in the selected directory"));
        }
        m_viewStack->setCurrentWidget(m_emptyStateWidget);
    } else {
        // Show the current view mode
        if (m_romListModel->viewMode() == RomListModel::DetailView) {
            m_viewStack->setCurrentWidget(m_detailView);
        } else {
            m_viewStack->setCurrentWidget(m_gridView);
        }
    }
}

void RomBrowserWidget::loadSettings()
{
    auto& settings = QT_UI::SettingsManager::instance();
    
    // Use romBrowser() to access ROM browser settings
    m_currentDirectory = settings.romBrowser()->lastRomDirectory();
    
    if (!m_currentDirectory.isEmpty()) {
        setDirectory(m_currentDirectory);
    }
    
    // Set view mode based on settings
    RomListModel::ViewMode mode = static_cast<RomListModel::ViewMode>(
        static_cast<int>(settings.romBrowser()->viewMode()));
    setViewMode(mode);
    
    QByteArray headerState = settings.romBrowser()->tableHeaderState();
    if (!headerState.isEmpty()) {
        m_detailView->header()->restoreState(headerState);
    }
}

void RomBrowserWidget::saveSettings()
{
    auto& settings = QT_UI::SettingsManager::instance();
    
    // Save current directory
    settings.romBrowser()->setLastRomDirectory(m_currentDirectory);
    
    // Save detail view header state
    settings.romBrowser()->setTableHeaderState(m_detailView->header()->saveState());
}

void RomBrowserWidget::loadColumnSettings()
{
    auto& settings = QT_UI::SettingsManager::instance();
    
    // Get visible columns from settings
    QVariantList visibleColumns = settings.romBrowser()->visibleColumns();
    
    // Set visible columns in the model
    if (m_romListModel && !visibleColumns.isEmpty()) {
        QVector<RomListModel::RomColumns> columns;
        
        for (const QVariant& var : visibleColumns) {
            bool ok;
            int colIndex = var.toInt(&ok);
            if (ok) {
                columns.append(static_cast<RomListModel::RomColumns>(colIndex));
            }
        }
        
        if (!columns.isEmpty()) {
            m_romListModel->setVisibleColumns(columns);
        }
    }
}

void RomBrowserWidget::reloadColumnSettings()
{
    auto& settings = QT_UI::SettingsManager::instance();
    
    // Get column settings from the settings manager
    QVariantList visibleColumnSettings = settings.romBrowser()->visibleColumns();
    
    qDebug() << "Reloading columns from settings. Found" << visibleColumnSettings.size() << "columns";
    
    // Only proceed if we have valid columns
    if (visibleColumnSettings.isEmpty()) {
        qDebug() << "No column settings found, not updating";
        return;
    }
    
    // Convert variant list to column IDs
    QVector<RomListModel::RomColumns> columns;
    for (const QVariant& column : visibleColumnSettings) {
        int colValue = column.toInt();
        if (colValue >= 0 && colValue < RomListModel::ColumnCount) {
            columns.append(static_cast<RomListModel::RomColumns>(colValue));
        }
    }
    
    // Only proceed if we have valid columns
    if (columns.isEmpty()) {
        qDebug() << "No valid columns after filtering, not updating";
        return;
    }
    
    qDebug() << "Applying" << columns.size() << "columns from settings to model";
    
    // Apply the columns to the model - this will trigger an update of the views
    if (m_romListModel) {
        m_romListModel->setVisibleColumns(columns);
        
        // Ensure proper resizing after columns change
        if (m_romListModel->viewMode() == RomListModel::DetailView) {
            QTimer::singleShot(0, this, &RomBrowserWidget::resizeDetailViewColumns);
        }
    } else {
        qDebug() << "RomListModel not available, can't apply column settings";
    }
}

void RomBrowserWidget::resizeDetailViewColumns()
{
    if (m_romListModel->viewMode() != RomListModel::DetailView || !m_detailView->isVisible())
        return;
    
    // Filename and Good Name columns get more space
    m_detailView->resizeColumnToContents(0);  // Filename
    m_detailView->resizeColumnToContents(1);  // Good Name
    
    // Let the header distribute remaining space
    m_detailView->header()->setSectionResizeMode(QHeaderView::Interactive);
    m_detailView->header()->setSectionResizeMode(0, QHeaderView::Stretch);  // Filename gets extra space
}

bool RomBrowserWidget::showTitles() const
{
    return m_romListModel ? m_romListModel->showTitles() : true;
}

void RomBrowserWidget::setShowTitles(bool show)
{
    if (m_romListModel) {
        if (m_romListModel->showTitles() != show) {
            m_romListModel->setShowTitles(show);
            
            // Update the toolbar action
            if (m_showTitlesAction) {
                m_showTitlesAction->setChecked(show);
            }
            
            // Update the grid view
            setupGridView();
            m_gridView->viewport()->update();
            
            // Emit signal for MainWindow to catch
            emit showTitlesToggled(show);
        }
    }
}

} // namespace QT_UI
