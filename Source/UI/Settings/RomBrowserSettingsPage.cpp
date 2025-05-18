#include "RomBrowserSettingsPage.h"
#include "../../Core/Settings/SettingsManager.h"
#include "../../Core/Settings/RomBrowserSettings.h"
#include "../RomBrowser/RomListModel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSettings>
#include <algorithm>

namespace QT_UI {

RomBrowserSettingsPage::RomBrowserSettingsPage(QWidget* parent)
    : BaseSettingsPage(parent)
{
    setupUI();
    loadSettings();
}

void RomBrowserSettingsPage::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Options group
    QGroupBox* optionsGroup = new QGroupBox(tr("Options"));
    QVBoxLayout* optionsLayout = new QVBoxLayout(optionsGroup);
    
    m_useRomBrowserCheck = new QCheckBox(tr("Use ROM Browser"));
    m_recursionCheck = new QCheckBox(tr("Use directory recursion when scanning for ROMs"));
    m_showExtensionsCheck = new QCheckBox(tr("Show file extensions"));
    
    optionsLayout->addWidget(m_useRomBrowserCheck);
    optionsLayout->addWidget(m_recursionCheck);
    optionsLayout->addWidget(m_showExtensionsCheck);
    
    mainLayout->addWidget(optionsGroup);
    
    // View mode group
    QGroupBox* viewModeGroup = new QGroupBox(tr("View Mode"));
    QVBoxLayout* viewModeLayout = new QVBoxLayout(viewModeGroup);
    
    QHBoxLayout* viewComboLayout = new QHBoxLayout();
    QLabel* viewModeLabel = new QLabel(tr("Display ROMs as:"));
    m_viewModeCombo = new QComboBox();
    m_viewModeCombo->addItem(tr("Details (List)"));
    m_viewModeCombo->addItem(tr("Grid (Thumbnails)"));
    viewComboLayout->addWidget(viewModeLabel);
    viewComboLayout->addWidget(m_viewModeCombo);
    viewComboLayout->addStretch();
    
    m_showTitlesCheck = new QCheckBox(tr("Show ROM titles in grid view"));
    
    QHBoxLayout* scaleLayout = new QHBoxLayout();
    QLabel* scaleLabel = new QLabel(tr("Cover scale:"));
    m_coverScaleSpinBox = new QDoubleSpinBox();
    m_coverScaleSpinBox->setMinimum(0.5);
    m_coverScaleSpinBox->setMaximum(2.0);
    m_coverScaleSpinBox->setSingleStep(0.1);
    m_coverScaleSpinBox->setDecimals(1);
    scaleLayout->addWidget(scaleLabel);
    scaleLayout->addWidget(m_coverScaleSpinBox);
    scaleLayout->addStretch();
    
    viewModeLayout->addLayout(viewComboLayout);
    viewModeLayout->addWidget(m_showTitlesCheck);
    viewModeLayout->addLayout(scaleLayout);
    
    mainLayout->addWidget(viewModeGroup);
    
    // Columns group
    QGroupBox* columnsGroup = new QGroupBox(tr("Columns"));
    QHBoxLayout* columnsLayout = new QHBoxLayout(columnsGroup);
    
    // Available columns
    QVBoxLayout* availableLayout = new QVBoxLayout();
    QLabel* availableLabel = new QLabel(tr("Available Fields:"));
    m_availableColumnsList = new QListWidget();
    availableLayout->addWidget(availableLabel);
    availableLayout->addWidget(m_availableColumnsList);
    
    // Add/Remove buttons
    QVBoxLayout* buttonLayout = new QVBoxLayout();
    m_addColumnButton = new QPushButton(tr("Add ->"));
    m_removeColumnButton = new QPushButton(tr("<- Remove"));
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_addColumnButton);
    buttonLayout->addWidget(m_removeColumnButton);
    buttonLayout->addStretch();
    
    // Shown columns
    QVBoxLayout* showingLayout = new QVBoxLayout();
    QLabel* showingLabel = new QLabel(tr("Show Fields:"));
    m_shownColumnsList = new QListWidget();
    showingLayout->addWidget(showingLabel);
    showingLayout->addWidget(m_shownColumnsList);
    
    // Up/Down buttons
    QVBoxLayout* orderButtonLayout = new QVBoxLayout();
    m_moveUpButton = new QPushButton(tr("Up"));
    m_moveDownButton = new QPushButton(tr("Down"));
    orderButtonLayout->addStretch();
    orderButtonLayout->addWidget(m_moveUpButton);
    orderButtonLayout->addWidget(m_moveDownButton);
    orderButtonLayout->addStretch();
    
    columnsLayout->addLayout(availableLayout);
    columnsLayout->addLayout(buttonLayout);
    columnsLayout->addLayout(showingLayout);
    columnsLayout->addLayout(orderButtonLayout);
    
    mainLayout->addWidget(columnsGroup);
    mainLayout->addStretch();
    
    // Connect signals
    connect(m_useRomBrowserCheck, &QCheckBox::toggled, this, &RomBrowserSettingsPage::romBrowserEnabledChanged);
    connect(m_recursionCheck, &QCheckBox::toggled, this, &BaseSettingsPage::settingsChanged);
    connect(m_showExtensionsCheck, &QCheckBox::toggled, this, &BaseSettingsPage::settingsChanged);
    connect(m_viewModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &RomBrowserSettingsPage::viewModeChanged);
    connect(m_showTitlesCheck, &QCheckBox::toggled, this, &BaseSettingsPage::settingsChanged);
    connect(m_coverScaleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), 
            this, &BaseSettingsPage::settingsChanged);
    
    // Connect column management buttons
    connect(m_addColumnButton, &QPushButton::clicked, this, &RomBrowserSettingsPage::addSelectedColumn);
    connect(m_removeColumnButton, &QPushButton::clicked, this, &RomBrowserSettingsPage::removeSelectedColumn);
    connect(m_moveUpButton, &QPushButton::clicked, this, &RomBrowserSettingsPage::moveColumnUp);
    connect(m_moveDownButton, &QPushButton::clicked, this, &RomBrowserSettingsPage::moveColumnDown);
    
    // Connect list selections to update button states
    connect(m_availableColumnsList, &QListWidget::itemSelectionChanged, 
            this, &RomBrowserSettingsPage::updateColumnButtonStates);
    connect(m_shownColumnsList, &QListWidget::itemSelectionChanged, 
            this, &RomBrowserSettingsPage::updateColumnButtonStates);
}

void RomBrowserSettingsPage::loadSettings()
{
    auto& settings = SettingsManager::instance();
    auto* romBrowserSettings = settings.romBrowser();
    
    // Load basic settings
    m_useRomBrowserCheck->setChecked(romBrowserSettings->isEnabled());
    m_recursionCheck->setChecked(romBrowserSettings->recursiveScan());
    m_showExtensionsCheck->setChecked(romBrowserSettings->showFileExtensions());
    
    // Load view mode settings
    m_viewModeCombo->setCurrentIndex(static_cast<int>(romBrowserSettings->viewMode()));
    m_showTitlesCheck->setChecked(romBrowserSettings->showTitles());
    m_coverScaleSpinBox->setValue(romBrowserSettings->coverScale());
    
    // Update view mode-dependent controls
    viewModeChanged(m_viewModeCombo->currentIndex());
    
    // Load column settings
    populateColumnLists();
    
    // Update enabled state of controls
    romBrowserEnabledChanged(m_useRomBrowserCheck->isChecked());
}

void RomBrowserSettingsPage::saveSettings()
{
    auto& settings = SettingsManager::instance();
    auto* romBrowserSettings = settings.romBrowser();
    
    // Save basic settings
    romBrowserSettings->setEnabled(m_useRomBrowserCheck->isChecked());
    romBrowserSettings->setRecursiveScan(m_recursionCheck->isChecked());
    romBrowserSettings->setShowFileExtensions(m_showExtensionsCheck->isChecked());
    
    // Save view mode settings
    romBrowserSettings->setViewMode(static_cast<RomBrowserSettings::ViewMode>(m_viewModeCombo->currentIndex()));
    romBrowserSettings->setShowTitles(m_showTitlesCheck->isChecked());
    romBrowserSettings->setCoverScale(m_coverScaleSpinBox->value());
    
    // Save column settings
    QVariantList visibleColumns;
    for (int i = 0; i < m_shownColumnsList->count(); i++) {
        QListWidgetItem* item = m_shownColumnsList->item(i);
        int columnId = item->data(Qt::UserRole).toInt();
        visibleColumns.append(columnId);
    }
    
    romBrowserSettings->setVisibleColumns(visibleColumns);
    emit columnSettingsChanged();
}

void RomBrowserSettingsPage::resetSettings()
{
    m_useRomBrowserCheck->setChecked(true);
    m_recursionCheck->setChecked(true);
    m_showExtensionsCheck->setChecked(true);
    m_viewModeCombo->setCurrentIndex(0); // Details view
    m_showTitlesCheck->setChecked(true);
    m_coverScaleSpinBox->setValue(1.0);
    
    // Reset columns to default
    m_columnInfo.clear();
    populateColumnLists();
    
    // Update dependent UI
    romBrowserEnabledChanged(true);
    viewModeChanged(0);
    
    emit settingsChanged();
}

void RomBrowserSettingsPage::populateColumnLists()
{
    // Clear existing items
    m_availableColumnsList->clear();
    m_shownColumnsList->clear();
    m_columnInfo.clear();
    
    // Define all possible columns based on RomListModel::RomColumns enum
    struct {
        int id;
        QString name;
    } columns[] = {
        {static_cast<int>(RomListModel::FileName), tr("File Name")},
        {static_cast<int>(RomListModel::GoodName), tr("Good Name")},
        {static_cast<int>(RomListModel::InternalName), tr("Internal Name")},
        {static_cast<int>(RomListModel::Size), tr("Size")},
        {static_cast<int>(RomListModel::Country), tr("Country")},
        {static_cast<int>(RomListModel::ReleaseDate), tr("Release Date")},
        {static_cast<int>(RomListModel::Players), tr("Players")},
        {static_cast<int>(RomListModel::Genre), tr("Genre")},
        {static_cast<int>(RomListModel::Developer), tr("Developer")},
        {static_cast<int>(RomListModel::CRC1), tr("CRC1")},
        {static_cast<int>(RomListModel::CRC2), tr("CRC2")},
        {static_cast<int>(RomListModel::MD5), tr("MD5")},
        {static_cast<int>(RomListModel::FilePath), tr("File Path")},
        {static_cast<int>(RomListModel::CartID), tr("Cart ID")},
        {static_cast<int>(RomListModel::MediaType), tr("Media")}, 
        {static_cast<int>(RomListModel::CartridgeCode), tr("Cartridge Code")},
        {static_cast<int>(RomListModel::ForceFeedback), tr("Force Feedback")},
        {static_cast<int>(RomListModel::CICChip), tr("CIC Chip")},
        {static_cast<int>(RomListModel::Status), tr("Status")}
    };
    
    // Load current visible columns
    QVariantList visibleColumns = SettingsManager::instance().romBrowser()->visibleColumns();
    
    // If no saved columns, use defaults
    if (visibleColumns.isEmpty()) {
        visibleColumns.append(RomListModel::FileName);
        visibleColumns.append(RomListModel::GoodName);
        visibleColumns.append(RomListModel::InternalName);
        visibleColumns.append(RomListModel::Size);
        visibleColumns.append(RomListModel::Country);
    }
    
    // Create column info for each column
    for (const auto &col : columns) {
        ColumnInfo info;
        info.columnId = col.id;
        info.name = col.name;
        
        // Check if this column is in visible columns
        bool found = false;
        int position = -1;
        
        for (int i = 0; i < visibleColumns.size(); ++i) {
            if (visibleColumns[i].toInt() == col.id) {
                found = true;
                position = i;
                break;
            }
        }
        
        info.selected = found;
        info.position = position;
        m_columnInfo.append(info);
    }
    
    // Sort columns into shown and available lists
    QVector<ColumnInfo> shownColumns;
    QVector<ColumnInfo> availableColumns;
    
    for (const auto &col : m_columnInfo) {
        if (col.selected) {
            shownColumns.append(col);
        } else {
            availableColumns.append(col);
        }
    }
    
    // Sort shown columns by position
    std::sort(shownColumns.begin(), shownColumns.end(), 
              [](const ColumnInfo &a, const ColumnInfo &b) { return a.position < b.position; });
    
    // Sort available columns alphabetically
    std::sort(availableColumns.begin(), availableColumns.end(),
              [](const ColumnInfo &a, const ColumnInfo &b) { return a.name < b.name; });
    
    // Add to the lists
    for (const auto &col : availableColumns) {
        QListWidgetItem *item = new QListWidgetItem(col.name);
        item->setData(Qt::UserRole, col.columnId);
        m_availableColumnsList->addItem(item);
    }
    
    for (const auto &col : shownColumns) {
        QListWidgetItem *item = new QListWidgetItem(col.name);
        item->setData(Qt::UserRole, col.columnId);
        m_shownColumnsList->addItem(item);
    }
    
    updateColumnButtonStates();
}

void RomBrowserSettingsPage::addSelectedColumn()
{
    QListWidgetItem *item = m_availableColumnsList->currentItem();
    if (!item) return;
    
    int columnId = item->data(Qt::UserRole).toInt();
    QString columnName = item->text();
    
    // Add to shown list
    QListWidgetItem *newItem = new QListWidgetItem(columnName);
    newItem->setData(Qt::UserRole, columnId);
    m_shownColumnsList->addItem(newItem);
    m_shownColumnsList->setCurrentItem(newItem);
    
    // Remove from available list
    delete item;
    
    // Update column info
    for (int i = 0; i < m_columnInfo.size(); ++i) {
        if (m_columnInfo[i].columnId == columnId) {
            m_columnInfo[i].selected = true;
            m_columnInfo[i].position = m_shownColumnsList->count() - 1;
            break;
        }
    }
    
    updateColumnButtonStates();
    emit settingsChanged();
}

void RomBrowserSettingsPage::removeSelectedColumn()
{
    QListWidgetItem *item = m_shownColumnsList->currentItem();
    if (!item) return;
    
    int columnId = item->data(Qt::UserRole).toInt();
    QString columnName = item->text();
    
    // Add to available list
    QListWidgetItem *newItem = new QListWidgetItem(columnName);
    newItem->setData(Qt::UserRole, columnId);
    m_availableColumnsList->addItem(newItem);
    m_availableColumnsList->setCurrentItem(newItem);
    
    // Remove from shown list
    delete item;
    
    // Update column info
    for (int i = 0; i < m_columnInfo.size(); ++i) {
        if (m_columnInfo[i].columnId == columnId) {
            m_columnInfo[i].selected = false;
            m_columnInfo[i].position = -1;
            break;
        }
    }
    
    // Update positions of remaining shown columns
    for (int i = 0; i < m_shownColumnsList->count(); ++i) {
        QListWidgetItem *shownItem = m_shownColumnsList->item(i);
        int shownColumnId = shownItem->data(Qt::UserRole).toInt();
        
        for (int j = 0; j < m_columnInfo.size(); ++j) {
            if (m_columnInfo[j].columnId == shownColumnId) {
                m_columnInfo[j].position = i;
                break;
            }
        }
    }
    
    updateColumnButtonStates();
    emit settingsChanged();
}

void RomBrowserSettingsPage::moveColumnUp()
{
    int row = m_shownColumnsList->currentRow();
    if (row <= 0) return;
    
    QListWidgetItem *item = m_shownColumnsList->takeItem(row);
    m_shownColumnsList->insertItem(row - 1, item);
    m_shownColumnsList->setCurrentItem(item);
    
    // Update positions in column info
    for (int i = 0; i < m_shownColumnsList->count(); ++i) {
        QListWidgetItem *shownItem = m_shownColumnsList->item(i);
        int shownColumnId = shownItem->data(Qt::UserRole).toInt();
        
        for (int j = 0; j < m_columnInfo.size(); ++j) {
            if (m_columnInfo[j].columnId == shownColumnId) {
                m_columnInfo[j].position = i;
                break;
            }
        }
    }
    
    updateColumnButtonStates();
    emit settingsChanged();
}

void RomBrowserSettingsPage::moveColumnDown()
{
    int row = m_shownColumnsList->currentRow();
    if (row < 0 || row >= m_shownColumnsList->count() - 1) return;
    
    QListWidgetItem *item = m_shownColumnsList->takeItem(row);
    m_shownColumnsList->insertItem(row + 1, item);
    m_shownColumnsList->setCurrentItem(item);
    
    // Update positions in column info
    for (int i = 0; i < m_shownColumnsList->count(); ++i) {
        QListWidgetItem *shownItem = m_shownColumnsList->item(i);
        int shownColumnId = shownItem->data(Qt::UserRole).toInt();
        
        for (int j = 0; j < m_columnInfo.size(); ++j) {
            if (m_columnInfo[j].columnId == shownColumnId) {
                m_columnInfo[j].position = i;
                break;
            }
        }
    }
    
    updateColumnButtonStates();
    emit settingsChanged();
}

void RomBrowserSettingsPage::updateColumnButtonStates()
{
    bool romBrowserEnabled = m_useRomBrowserCheck->isChecked();
    
    // Enable/disable add button
    m_addColumnButton->setEnabled(
        romBrowserEnabled && m_availableColumnsList->currentItem() != nullptr);
    
    // Enable/disable remove button
    m_removeColumnButton->setEnabled(
        romBrowserEnabled && m_shownColumnsList->currentItem() != nullptr);
    
    // Enable/disable up button
    int currentRow = m_shownColumnsList->currentRow();
    m_moveUpButton->setEnabled(
        romBrowserEnabled && currentRow > 0);
    
    // Enable/disable down button
    m_moveDownButton->setEnabled(
        romBrowserEnabled && currentRow >= 0 && currentRow < m_shownColumnsList->count() - 1);
}

void RomBrowserSettingsPage::romBrowserEnabledChanged(bool enabled)
{
    // Update the state of all ROM browser controls
    m_recursionCheck->setEnabled(enabled);
    m_showExtensionsCheck->setEnabled(enabled);
    m_viewModeCombo->setEnabled(enabled);
    m_showTitlesCheck->setEnabled(enabled && m_viewModeCombo->currentIndex() == 1); // Only in grid view
    m_coverScaleSpinBox->setEnabled(enabled && m_viewModeCombo->currentIndex() == 1); // Only in grid view
    m_availableColumnsList->setEnabled(enabled);
    m_shownColumnsList->setEnabled(enabled);
    
    // The button states depend on both enabled state and selection
    updateColumnButtonStates();
}

void RomBrowserSettingsPage::viewModeChanged(int index)
{
    // Enable/disable grid view specific settings
    bool isGridView = (index == 1);
    m_showTitlesCheck->setEnabled(m_useRomBrowserCheck->isChecked() && isGridView);
    m_coverScaleSpinBox->setEnabled(m_useRomBrowserCheck->isChecked() && isGridView);
}

} // namespace QT_UI
