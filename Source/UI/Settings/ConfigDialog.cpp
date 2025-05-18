#include "ConfigDialog.h"
#include "../../Core/Settings/SettingsManager.h"
#include "../../Core/Settings/ApplicationSettings.h"
#include "../../Core/Settings/PluginSettings.h"
#include "../../Core/Settings/SaveSettings.h"
#include "../../Core/Settings/RomBrowserSettings.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>
#include <QFileDialog>
#include <QSettings>
#include <QSpacerItem>
#include <QMessageBox>
#include <algorithm>
#include "../RomBrowser/RomListModel.h"

namespace QT_UI {

ConfigDialog::ConfigDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    loadSettings();
    
    // Connect the dialog buttons
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        saveSettings();
        accept();
    });
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

ConfigDialog::~ConfigDialog()
{
    // All widgets are parented and automatically cleaned up
}

void ConfigDialog::setupUi()
{
    // Set window properties
    setWindowTitle(tr("Configuration"));
    setWindowIcon(QIcon(":/icons/pj64.ico"));
    resize(600, 550);
    
    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Create tab widget
    m_tabWidget = new QTabWidget(this);
    
    // Create General tab
    QWidget *generalTab = new QWidget();
    QVBoxLayout *generalLayout = new QVBoxLayout(generalTab);
    
    // Create toolbox for general settings
    m_generalToolBox = new QToolBox(generalTab);
    
    // General section
    QWidget *generalSection = new QWidget();
    QVBoxLayout *generalSectionLayout = new QVBoxLayout(generalSection);
    
    m_pauseWhenInactiveCheck = new QCheckBox(tr("Pause emulation when window is not active"), generalSection);
    m_enterFullscreenCheck = new QCheckBox(tr("Enter fullscreen mode when loading a ROM"), generalSection);
    m_disableScreensaverCheck = new QCheckBox(tr("Disable screensaver when running a ROM"), generalSection);
    m_enableDiscordCheck = new QCheckBox(tr("Enable Discord Rich Presence"), generalSection);
    m_hideAdvancedSettingsCheck = new QCheckBox(tr("Hide advanced settings"), generalSection);
    
    generalSectionLayout->addWidget(m_pauseWhenInactiveCheck);
    generalSectionLayout->addWidget(m_enterFullscreenCheck);
    generalSectionLayout->addWidget(m_disableScreensaverCheck);
    generalSectionLayout->addWidget(m_enableDiscordCheck);
    generalSectionLayout->addWidget(m_hideAdvancedSettingsCheck);
    
    generalSectionLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    
    QFormLayout *romsFormLayout = new QFormLayout();
    QLabel *maxRomsLabel = new QLabel(tr("Max # of ROMs remembered (0-10):"), generalSection);
    m_maxRomsSpinBox = new QSpinBox(generalSection);
    m_maxRomsSpinBox->setMinimum(0);
    m_maxRomsSpinBox->setMaximum(10);
    m_maxRomsSpinBox->setValue(4);
    romsFormLayout->addRow(maxRomsLabel, m_maxRomsSpinBox);
    
    QLabel *maxRomDirsLabel = new QLabel(tr("Max # of ROM dirs remembered (0-10):"), generalSection);
    m_maxRomDirsSpinBox = new QSpinBox(generalSection);
    m_maxRomDirsSpinBox->setMinimum(0);
    m_maxRomDirsSpinBox->setMaximum(10);
    m_maxRomDirsSpinBox->setValue(4);
    romsFormLayout->addRow(maxRomDirsLabel, m_maxRomDirsSpinBox);
    
    generalSectionLayout->addLayout(romsFormLayout);
    generalSectionLayout->addStretch();
    
    m_generalToolBox->addItem(generalSection, tr("General"));
    
    // Advanced section
    QWidget *advancedSection = new QWidget();
    m_generalToolBox->addItem(advancedSection, tr("Advanced"));
    
    // Defaults section
    QWidget *defaultsSection = new QWidget();
    m_generalToolBox->addItem(defaultsSection, tr("Defaults"));
    
    // Directories section
    QWidget *directoriesSection = new QWidget();
    QVBoxLayout *directoriesLayout = new QVBoxLayout(directoriesSection);
    
    // Plugin directory group
    QGroupBox *pluginDirGroup = new QGroupBox(tr("Plugin directory:"), directoriesSection);
    QVBoxLayout *pluginDirLayout = new QVBoxLayout(pluginDirGroup);
    
    m_pluginDirEdit = new QLineEdit(pluginDirGroup);
    pluginDirLayout->addWidget(m_pluginDirEdit);
    
    m_browsePluginButton = new QPushButton(tr("Browse..."), pluginDirGroup);
    m_browsePluginButton->setMaximumWidth(378);
    connect(m_browsePluginButton, &QPushButton::clicked, this, &ConfigDialog::browsePluginDir);
    pluginDirLayout->addWidget(m_browsePluginButton);
    
    m_useDefaultPluginDirCheck = new QCheckBox(tr("Use Default: $AppPath\\Plugin\\"), pluginDirGroup);
    pluginDirLayout->addWidget(m_useDefaultPluginDirCheck);
    
    directoriesLayout->addWidget(pluginDirGroup);
    
    // Saves directory group
    QGroupBox *savesDirGroup = new QGroupBox(tr("N64 native saves directory:"), directoriesSection);
    QVBoxLayout *savesDirLayout = new QVBoxLayout(savesDirGroup);
    
    m_savesDirEdit = new QLineEdit(savesDirGroup);
    savesDirLayout->addWidget(m_savesDirEdit);
    
    m_browseSavesButton = new QPushButton(tr("Browse..."), savesDirGroup);
    m_browseSavesButton->setMaximumWidth(378);
    connect(m_browseSavesButton, &QPushButton::clicked, this, &ConfigDialog::browseSavesDir);
    savesDirLayout->addWidget(m_browseSavesButton);
    
    m_useDefaultSavesDirCheck = new QCheckBox(tr("Use Default: $AppPath\\Save\\"), savesDirGroup);
    savesDirLayout->addWidget(m_useDefaultSavesDirCheck);
    
    directoriesLayout->addWidget(savesDirGroup);
    directoriesLayout->addStretch();
    
    m_generalToolBox->addItem(directoriesSection, tr("Directories"));
    
    // 64DD section
    QWidget *ddSection = new QWidget();
    m_generalToolBox->addItem(ddSection, tr("64DD"));
    
    generalLayout->addWidget(m_generalToolBox);
    
    // Set the current toolbox page
    m_generalToolBox->setCurrentIndex(0); // General page
    
    m_tabWidget->addTab(generalTab, tr("General"));
    
    // ROM Browser tab - now set up in separate method
    setupRomBrowserTab();
    
    // Hotkeys tab
    QWidget *hotkeysTab = new QWidget();
    m_tabWidget->addTab(hotkeysTab, tr("Hotkeys"));
    
    // Plugins tab
    QWidget *pluginsTab = new QWidget();
    m_tabWidget->addTab(pluginsTab, tr("Plugins"));
    
    mainLayout->addWidget(m_tabWidget);
    
    // Add button box
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply | QDialogButtonBox::Reset, this);
    connect(m_buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &ConfigDialog::applySettings);
    connect(m_buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, this, &ConfigDialog::resetSettings);
    mainLayout->addWidget(m_buttonBox);
    
    setLayout(mainLayout);
}

void ConfigDialog::setupRomBrowserTab()
{
    QWidget *romBrowserTab = new QWidget();
    QVBoxLayout *romBrowserLayout = new QVBoxLayout(romBrowserTab);
    
    // Options group
    QGroupBox *optionsGroup = new QGroupBox(tr("Options"), romBrowserTab);
    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsGroup);
    
    m_useRomBrowserCheck = new QCheckBox(tr("Use ROM Browser"), optionsGroup);
    m_recursionCheck = new QCheckBox(tr("Use directory recursion when scanning for ROMs"), optionsGroup);
    m_showExtensionsCheck = new QCheckBox(tr("Show file extensions"), optionsGroup);
    
    optionsLayout->addWidget(m_useRomBrowserCheck);
    optionsLayout->addWidget(m_recursionCheck);
    optionsLayout->addWidget(m_showExtensionsCheck);
    
    romBrowserLayout->addWidget(optionsGroup);
    
    // Columns group
    QGroupBox *columnsGroup = new QGroupBox(tr("Columns"), romBrowserTab);
    QHBoxLayout *columnsLayout = new QHBoxLayout(columnsGroup);
    
    // Available columns
    QVBoxLayout *availableLayout = new QVBoxLayout();
    QLabel *availableLabel = new QLabel(tr("Available Fields:"), columnsGroup);
    m_availableColumnsList = new QListWidget(columnsGroup);
    availableLayout->addWidget(availableLabel);
    availableLayout->addWidget(m_availableColumnsList);
    
    // Add/Remove buttons
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    m_addColumnButton = new QPushButton(tr("Add ->"), columnsGroup);
    m_removeColumnButton = new QPushButton(tr("<- Remove"), columnsGroup);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_addColumnButton);
    buttonLayout->addWidget(m_removeColumnButton);
    buttonLayout->addStretch();
    
    // Shown columns
    QVBoxLayout *showingLayout = new QVBoxLayout();
    QLabel *showingLabel = new QLabel(tr("Show Fields:"), columnsGroup);
    m_shownColumnsList = new QListWidget(columnsGroup);
    showingLayout->addWidget(showingLabel);
    showingLayout->addWidget(m_shownColumnsList);
    
    // Up/Down buttons
    QVBoxLayout *orderButtonLayout = new QVBoxLayout();
    m_moveUpButton = new QPushButton(tr("Up"), columnsGroup);
    m_moveDownButton = new QPushButton(tr("Down"), columnsGroup);
    orderButtonLayout->addStretch();
    orderButtonLayout->addWidget(m_moveUpButton);
    orderButtonLayout->addWidget(m_moveDownButton);
    orderButtonLayout->addStretch();
    
    // Connect the buttons
    connect(m_addColumnButton, &QPushButton::clicked, this, &ConfigDialog::addSelectedColumn);
    connect(m_removeColumnButton, &QPushButton::clicked, this, &ConfigDialog::removeSelectedColumn);
    connect(m_moveUpButton, &QPushButton::clicked, this, &ConfigDialog::moveColumnUp);
    connect(m_moveDownButton, &QPushButton::clicked, this, &ConfigDialog::moveColumnDown);
    
    // Connect selection changes to update button states
    connect(m_availableColumnsList, &QListWidget::itemSelectionChanged, this, &ConfigDialog::updateColumnButtonStates);
    connect(m_shownColumnsList, &QListWidget::itemSelectionChanged, this, &ConfigDialog::updateColumnButtonStates);
    
    // Connect ROM browser checkbox to enable/disable other controls
    connect(m_useRomBrowserCheck, &QCheckBox::toggled, this, &ConfigDialog::romBrowserEnabledChanged);
    
    columnsLayout->addLayout(availableLayout);
    columnsLayout->addLayout(buttonLayout);
    columnsLayout->addLayout(showingLayout);
    columnsLayout->addLayout(orderButtonLayout);
    
    romBrowserLayout->addWidget(columnsGroup);
    romBrowserLayout->addStretch();
    
    m_tabWidget->addTab(romBrowserTab, tr("ROM Browser"));
}

void ConfigDialog::loadSettings()
{
    auto& settings = QT_UI::SettingsManager::instance();
    
    // Load general settings
    m_pauseWhenInactiveCheck->setChecked(settings.application()->pauseWhenInactive());
    m_enterFullscreenCheck->setChecked(settings.application()->enterFullscreenWhenLoadingRom());
    m_disableScreensaverCheck->setChecked(settings.application()->disableScreensaver());
    m_enableDiscordCheck->setChecked(settings.application()->enableDiscord());
    m_hideAdvancedSettingsCheck->setChecked(settings.application()->hideAdvancedSettings());
    m_maxRomsSpinBox->setValue(settings.application()->maxRecentRoms());
    m_maxRomDirsSpinBox->setValue(settings.application()->maxRecentRomDirs());
    
    // Load directory settings
    bool useDefaultPlugin = settings.plugins()->useDefaultPluginDirectory();
    m_useDefaultPluginDirCheck->setChecked(useDefaultPlugin);
    m_pluginDirEdit->setText(settings.plugins()->pluginDirectory());
    m_pluginDirEdit->setEnabled(!useDefaultPlugin);
    
    bool useDefaultSaves = settings.saves()->useDefaultSavesDirectory();
    m_useDefaultSavesDirCheck->setChecked(useDefaultSaves);
    m_savesDirEdit->setText(settings.saves()->getSavesDirectory());
    m_savesDirEdit->setEnabled(!useDefaultSaves);

    // Apply the same pattern for other settings that have already been migrated
    m_useRomBrowserCheck->setChecked(settings.romBrowser()->isEnabled());
    m_recursionCheck->setChecked(settings.romBrowser()->recursiveScan());
    m_showExtensionsCheck->setChecked(settings.romBrowser()->showFileExtensions());
    
    // Load ROM column settings
    loadColumnSettings();
}

void ConfigDialog::saveSettings()
{
    auto& settings = QT_UI::SettingsManager::instance();
    
    // Save general settings
    settings.application()->setPauseWhenInactive(m_pauseWhenInactiveCheck->isChecked());
    settings.application()->setEnterFullscreenWhenLoadingRom(m_enterFullscreenCheck->isChecked());
    settings.application()->setDisableScreensaver(m_disableScreensaverCheck->isChecked());
    settings.application()->setEnableDiscord(m_enableDiscordCheck->isChecked());
    settings.application()->setHideAdvancedSettings(m_hideAdvancedSettingsCheck->isChecked());
    settings.application()->setMaxRecentRoms(m_maxRomsSpinBox->value());
    settings.application()->setMaxRecentRomDirs(m_maxRomDirsSpinBox->value());
    
    // Save directory settings
    settings.plugins()->setUseDefaultPluginDirectory(m_useDefaultPluginDirCheck->isChecked());
    settings.plugins()->setPluginDirectory(m_pluginDirEdit->text());
    settings.saves()->setUseDefaultSavesDirectory(m_useDefaultSavesDirCheck->isChecked());
    settings.saves()->setSavesDirectory(m_savesDirEdit->text());
    
    // Save ROM browser settings
    settings.romBrowser()->setEnabled(m_useRomBrowserCheck->isChecked());
    settings.romBrowser()->setRecursiveScan(m_recursionCheck->isChecked());
    settings.romBrowser()->setShowFileExtensions(m_showExtensionsCheck->isChecked());
    
    // Create a list of visible column IDs in order
    QVariantList visibleColumns;
    QStringList colNames;    
    for (int i = 0; i < m_shownColumnsList->count(); i++) {
        QListWidgetItem *item = m_shownColumnsList->item(i);
        int columnId = item->data(Qt::UserRole).toInt();
        visibleColumns.append(columnId);
        colNames.append(item->text());
    }
    
    // Debug output
    qDebug() << "Saving columns to settings. Count:" << visibleColumns.size();
    qDebug() << "Column order:" << colNames.join(", ");
    
    // Set visible columns through the settings manager
    settings.romBrowser()->setVisibleColumns(visibleColumns);
    
    qDebug() << "Column settings saved successfully";
    
    // Signal that column settings changed - will trigger reload of columns in views
    qDebug() << "Emitting column settings changed signal";
    emit columnSettingsChanged();
}

void ConfigDialog::loadColumnSettings()
{
    // Clear existing column lists
    m_availableColumnsList->clear();
    m_shownColumnsList->clear();
    
    // Get column settings from ROM browser settings
    auto& settings = QT_UI::SettingsManager::instance();
    QVariantList visibleColumns = settings.romBrowser()->visibleColumns();
    
    // Populate lists based on the settings
    populateColumnLists();
    
    // Update control states based on settings
    updateColumnButtonStates();
    
    // Update enabled state of ROM browser controls
    romBrowserEnabledChanged(m_useRomBrowserCheck->isChecked());
}

void ConfigDialog::populateColumnLists()
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
        {static_cast<int>(RomListModel::CartridgeCode), tr("Cartridge Code")}, // Changed from ProductID, "Product ID"
        {static_cast<int>(RomListModel::ForceFeedback), tr("Force Feedback")},
        {static_cast<int>(RomListModel::CICChip), tr("CIC Chip")},
        {static_cast<int>(RomListModel::Status), tr("Status")}
    };
    
    // Load current visible columns from settings
    QSettings settings;
    QList<QVariant> visibleColumns = settings.value("RomBrowser/VisibleColumns").toList();
    
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

void ConfigDialog::addSelectedColumn()
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
}

void ConfigDialog::removeSelectedColumn()
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
}

void ConfigDialog::moveColumnUp()
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
}

void ConfigDialog::moveColumnDown()
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
}

void ConfigDialog::updateColumnButtonStates()
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

void ConfigDialog::romBrowserEnabledChanged(bool enabled)
{
    // Update the state of all ROM browser controls
    m_recursionCheck->setEnabled(enabled);
    m_showExtensionsCheck->setEnabled(enabled);
    m_availableColumnsList->setEnabled(enabled);
    m_shownColumnsList->setEnabled(enabled);
    
    // The button states depend on both enabled state and selection
    updateColumnButtonStates();
}

void ConfigDialog::browsePluginDir()
{
    QString dir = QFileDialog::getExistingDirectory(
        this, 
        tr("Select Plugin Directory"),
        m_pluginDirEdit->text(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    if (!dir.isEmpty()) {
        m_pluginDirEdit->setText(dir);
    }
}

void ConfigDialog::browseSavesDir()
{
    QString dir = QFileDialog::getExistingDirectory(
        this, 
        tr("Select Saves Directory"),
        m_savesDirEdit->text(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    if (!dir.isEmpty()) {
        m_savesDirEdit->setText(dir);
    }
}

void ConfigDialog::applySettings()
{
    saveSettings();
    QMessageBox::information(this, tr("Settings Applied"), tr("Settings have been applied."));
}

void ConfigDialog::resetSettings()
{
    // Reset to default values
    m_pauseWhenInactiveCheck->setChecked(true);
    m_enterFullscreenCheck->setChecked(false);
    m_disableScreensaverCheck->setChecked(true);
    m_enableDiscordCheck->setChecked(true);
    m_hideAdvancedSettingsCheck->setChecked(false);
    m_maxRomsSpinBox->setValue(4);
    m_maxRomDirsSpinBox->setValue(4);
    
    m_useDefaultPluginDirCheck->setChecked(true);
    m_pluginDirEdit->setText("Plugin");
    m_pluginDirEdit->setEnabled(false);
    
    m_useDefaultSavesDirCheck->setChecked(true);
    m_savesDirEdit->setText("Save");
    m_savesDirEdit->setEnabled(false);
    
    // Reset ROM browser settings
    m_useRomBrowserCheck->setChecked(true);
    m_recursionCheck->setChecked(true);
    m_showExtensionsCheck->setChecked(true);
    
    // Reset columns to default
    m_columnInfo.clear();
    populateColumnLists();
}

} // namespace QT_UI
