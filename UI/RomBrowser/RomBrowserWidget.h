#pragma once

#include <QWidget>
#include <QTreeView>
#include <QListView>
#include <QStackedWidget>
#include <QToolBar>
#include <QLineEdit>
#include <QComboBox>
#include <QSortFilterProxyModel>
#include <QFileSystemModel>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QSlider>
#include <QAction>
#include <QToolButton>

#include "RomListModel.h"

namespace QT_UI {

/**
 * @brief Widget for browsing and selecting ROMs
 */
class RomBrowserWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit RomBrowserWidget(QWidget* parent = nullptr);
    ~RomBrowserWidget();
    
    /**
     * @brief Refreshes the ROM list
     */
    void refresh();
    
    /**
     * @brief Sets the directory to scan for ROMs
     * @param directory Path to the directory
     */
    void setDirectory(const QString& directory);
    
    /**
     * @brief Gets the currently selected ROM path
     * @return Path to the selected ROM, or empty string if none selected
     */
    QString getSelectedRomPath() const;

    /**
     * @brief Initializes the widget, refreshing the ROM list
     */
    void initialize() { refresh(); }
    
    /**
     * @brief Gets the current view mode
     * @return Current view mode (DetailView or GridView)
     */
    RomListModel::ViewMode viewMode() const;

    /**
     * @brief Sets whether titles are shown in grid view
     * @param show True to show titles, false to hide
     */
    void setShowTitles(bool show);
    
    /**
     * @brief Gets whether titles are shown in grid view
     * @return True if titles are shown
     */
    bool showTitles() const;
    
signals:
    /**
     * @brief Emitted when a ROM is selected
     * @param romPath Path to the selected ROM
     */
    void romSelected(const QString& romPath);
    
    /**
     * @brief Emitted when a ROM is double-clicked (should trigger loading)
     * @param romPath Path to the double-clicked ROM
     */
    void romDoubleClicked(const QString& romPath);
    
    /**
     * @brief Emitted when the view layout changes
     */
    void layoutChange();
    
    /**
     * @brief Emitted when the show titles setting is toggled
     * @param show True if titles are now shown, false if hidden
     */
    void showTitlesToggled(bool show);
    
public slots:
    /**
     * @brief Changes the current view mode
     * @param mode The view mode to switch to (DetailView or GridView)
     */
    void setViewMode(RomListModel::ViewMode mode);
    
private slots:
    void onViewModeToggled();
    void onTitleVisibilityToggled();
    void onZoomValueChanged(int value);
    void onFilterTextChanged(const QString& text);
    void onSortIndicatorChanged(int column, Qt::SortOrder order);
    void onItemClicked(const QModelIndex& index);
    void onItemDoubleClicked(const QModelIndex& index);
    void onSetDirectoryClicked();
    void onScanStarted();
    void onScanProgress(int current, int total);
    void onScanFinished();
    void onCoverDirectoryClicked();
    void updateToolbar();
    
private:
    void createViews();
    void createToolbar();
    void createEmptyState();
    void createConnections();
    void updateEmptyStateVisibility();
    void loadSettings();
    void saveSettings();
    void setupGridView();
    void resizeDetailViewColumns();
    void setupStatusArea(); // New method for status area styling
    
    // UI components
    QStackedWidget* m_viewStack;
    QTreeView* m_detailView;
    QListView* m_gridView;
    QToolBar* m_toolbar;
    QLineEdit* m_searchBox;
    QLabel* m_statusLabel;
    QProgressBar* m_progressBar;
    
    // Toolbar actions
    QAction* m_detailViewAction;
    QAction* m_gridViewAction;
    QAction* m_showTitlesAction;
    QAction* m_coverDirAction;
    
    // Grid view controls
    QSlider* m_zoomSlider;
    QLabel* m_zoomLabel;
    
    // Empty state widgets
    QWidget* m_emptyStateWidget;
    QLabel* m_emptyStateLabel;
    QPushButton* m_setDirButton;
    
    // Models
    RomListModel* m_romListModel;
    QSortFilterProxyModel* m_proxyModel;
    RomGridDelegate* m_gridDelegate;
    
    // State
    QString m_currentDirectory;
};

} // namespace QT_UI
