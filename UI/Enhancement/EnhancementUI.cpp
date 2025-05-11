#include "EnhancementUI.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSplitter>
#include <QDebug>
#include <QRegularExpression>
#include <QVariant>
#include <QLabel>  // Add this include for QLabel

namespace QT_UI {

// Implementation for EnhancementDialog
EnhancementDialog::EnhancementDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Enhancements"));
    resize(800, 500);

    // Create layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Create splitter for tree and notes
    QSplitter* splitter = new QSplitter(Qt::Vertical);
    
    // Create tree widget
    m_treeWidget = new QTreeWidget();
    m_treeWidget->setHeaderLabels({tr("Enhancement"), tr("Status")});
    m_treeWidget->setColumnWidth(0, 400);
    m_treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    splitter->addWidget(m_treeWidget);
    
    // Create notes area
    m_notesEdit = new QTextEdit();
    m_notesEdit->setReadOnly(true);
    m_notesEdit->setPlaceholderText(tr("Notes will be displayed here"));
    splitter->addWidget(m_notesEdit);
    
    // Add splitter to main layout
    mainLayout->addWidget(splitter);
    
    // Create buttons layout
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    
    // Create buttons
    m_addButton = new QPushButton(tr("Add"));
    m_editButton = new QPushButton(tr("Edit"));
    m_removeButton = new QPushButton(tr("Remove"));
    QPushButton* closeButton = new QPushButton(tr("Close"));
    
    // Disable edit/remove initially until something is selected
    m_editButton->setEnabled(false);
    m_removeButton->setEnabled(false);
    
    // Add buttons to layout
    buttonsLayout->addWidget(m_addButton);
    buttonsLayout->addWidget(m_editButton);
    buttonsLayout->addWidget(m_removeButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(closeButton);
    
    // Add button layout to main layout
    mainLayout->addLayout(buttonsLayout);
    
    // Connect signals and slots
    connect(m_treeWidget, &QTreeWidget::itemChanged, this, &EnhancementDialog::onItemChanged);
    connect(m_treeWidget, &QTreeWidget::itemSelectionChanged, this, &EnhancementDialog::onItemSelectionChanged);
    connect(m_addButton, &QPushButton::clicked, this, &EnhancementDialog::onAddEnhancement);
    connect(m_editButton, &QPushButton::clicked, this, &EnhancementDialog::onEditEnhancement);
    connect(m_removeButton, &QPushButton::clicked, this, &EnhancementDialog::onRemoveEnhancement);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

EnhancementDialog::~EnhancementDialog()
{
    // Clean up any resources
    for (EnhancementData* data : m_enhancements) {
        delete data;
    }
}

void EnhancementDialog::onItemChanged(QTreeWidgetItem* item, int column)
{
    if (column != 1 || !item) return;
    
    bool checked = item->checkState(1) == Qt::Checked;
    updateEnhancementState(item, checked);
}

void EnhancementDialog::onItemSelectionChanged()
{
    QTreeWidgetItem* item = m_treeWidget->currentItem();
    m_notesEdit->clear();
    
    bool validSelection = (item != nullptr);
    m_editButton->setEnabled(validSelection);
    m_removeButton->setEnabled(validSelection);
    
    if (item) {
        // Get the enhancement data pointer from the item
        QVariant itemData = item->data(0, Qt::UserRole);
        if (itemData.isValid()) {
            EnhancementData* enhancement = itemData.value<EnhancementData*>();
            if (enhancement) {
                m_notesEdit->setText(enhancement->notes);
            }
        }
    }
}

void EnhancementDialog::onEditEnhancement()
{
    QTreeWidgetItem* item = m_treeWidget->currentItem();
    if (!item) return;

    QVariant itemData = item->data(0, Qt::UserRole);
    if (itemData.isValid()) {
        EnhancementData* enhancement = itemData.value<EnhancementData*>();
        if (enhancement) {
            EditEnhancementDialog dialog(*this, enhancement);
            if (dialog.exec() == QDialog::Accepted) {
                refreshList();
                saveEnhancements();
            }
        }
    }
}

void EnhancementDialog::onAddEnhancement()
{
    EditEnhancementDialog dialog(*this);
    if (dialog.exec() == QDialog::Accepted) {
        refreshList();
        saveEnhancements();
    }
}

void EnhancementDialog::onRemoveEnhancement()
{
    QTreeWidgetItem* item = m_treeWidget->currentItem();
    if (!item) return;
    
    QVariant itemData = item->data(0, Qt::UserRole);
    if (itemData.isValid()) {
        EnhancementData* enhancement = itemData.value<EnhancementData*>();
        
        if (QMessageBox::question(this, tr("Remove Enhancement"), 
                               tr("Are you sure you want to remove the enhancement '%1'?")
                               .arg(enhancement->name),
                               QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) 
        {
            // Remove the enhancement from the list
            auto it = std::find(m_enhancements.begin(), m_enhancements.end(), enhancement);
            if (it != m_enhancements.end()) {
                m_enhancements.erase(it);
                delete enhancement;
            }
            
            // Update UI
            refreshList();
            saveEnhancements();
        }
    }
}

// Implementation for EditEnhancementDialog
EditEnhancementDialog::EditEnhancementDialog(EnhancementDialog& parent, EnhancementData* enhancement)
    : QDialog(&parent)
    , m_parent(parent)
    , m_enhancement(enhancement)
{
    setWindowTitle(enhancement ? tr("Edit Enhancement") : tr("Add Enhancement"));
    resize(600, 400);
    
    // Create layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();
    
    // Create input fields
    m_nameEdit = new QLineEdit();
    m_authorEdit = new QLineEdit();
    m_notesEdit = new QTextEdit();
    m_gamesharkCheckbox = new QCheckBox(tr("Use GameShark codes"));
    m_overclockCheckbox = new QCheckBox(tr("Enable Overclocking"));
    m_overclockModifierEdit = new QLineEdit();
    m_editGamesharkButton = new QPushButton(tr("Edit GameShark Codes..."));
    m_autoOnCheckbox = new QCheckBox(tr("Enable by default"));
    
    // Add fields to form layout
    formLayout->addRow(tr("Name:"), m_nameEdit);
    formLayout->addRow(tr("Author:"), m_authorEdit);
    formLayout->addRow(tr("Notes:"), m_notesEdit);
    formLayout->addRow("", m_gamesharkCheckbox);
    formLayout->addRow("", m_editGamesharkButton);
    formLayout->addRow("", m_overclockCheckbox);
    formLayout->addRow(tr("Overclock Modifier:"), m_overclockModifierEdit);
    formLayout->addRow("", m_autoOnCheckbox);
    
    // Add form layout to main layout
    mainLayout->addLayout(formLayout);
    
    // Create button box
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &EditEnhancementDialog::onAccept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
    // Add button box to main layout
    mainLayout->addWidget(buttonBox);
    
    // Connect signal handlers
    connect(m_gamesharkCheckbox, &QCheckBox::toggled, this, &EditEnhancementDialog::onGamesharkToggled);
    connect(m_overclockCheckbox, &QCheckBox::toggled, this, &EditEnhancementDialog::onOverClockToggled);
    connect(m_editGamesharkButton, &QPushButton::clicked, this, &EditEnhancementDialog::onEditGameshark);
    
    // If editing an existing enhancement, populate fields
    if (enhancement) {
        m_entries = enhancement->codeEntries;
        m_pluginList = enhancement->pluginList;
        m_nameEdit->setText(enhancement->name);
        m_authorEdit->setText(enhancement->author);
        m_notesEdit->setText(enhancement->notes);
        m_gamesharkCheckbox->setChecked(!enhancement->codeEntries.empty());
        m_overclockCheckbox->setChecked(enhancement->overclock);
        m_overclockModifierEdit->setText(QString::number(enhancement->overclockModifier));
        m_autoOnCheckbox->setChecked(enhancement->enabledByDefault);
    } else {
        // Set defaults for new enhancement
        m_overclockModifierEdit->setText("1");
    }
    
    // Update UI state based on checkbox values
    onGamesharkToggled(m_gamesharkCheckbox->isChecked());
    onOverClockToggled(m_overclockCheckbox->isChecked());
}

EditEnhancementDialog::~EditEnhancementDialog()
{
}

void EditEnhancementDialog::onGamesharkToggled(bool checked)
{
    m_editGamesharkButton->setEnabled(checked);
}

void EditEnhancementDialog::onOverClockToggled(bool checked)
{
    m_overclockModifierEdit->setEnabled(checked);
}

void EditEnhancementDialog::onEditGameshark()
{
    EditGamesharkDialog dialog(m_entries, m_pluginList, this);
    dialog.exec();
}

void EditEnhancementDialog::onAccept()
{
    // Validate input
    if (m_nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Enhancement name cannot be empty"));
        return;
    }
    
    // Create or update enhancement
    if (!m_enhancement) {
        m_enhancement = new EnhancementData();
        m_parent.getEnhancements().push_back(m_enhancement);
    }
    
    // Update enhancement data
    m_enhancement->name = m_nameEdit->text().trimmed();
    m_enhancement->author = m_authorEdit->text().trimmed();
    m_enhancement->notes = m_notesEdit->toPlainText().trimmed();
    m_enhancement->enabledByDefault = m_autoOnCheckbox->isChecked();
    m_enhancement->overclock = m_overclockCheckbox->isChecked();
    m_enhancement->overclockModifier = m_overclockModifierEdit->text().toInt();
    m_enhancement->codeEntries = m_entries;
    m_enhancement->pluginList = m_pluginList;
    
    // Accept dialog
    accept();
}

// Implementation for EditGamesharkDialog
EditGamesharkDialog::EditGamesharkDialog(EnhancementData::CodeEntries& entries, 
                                       EnhancementData::PluginList& pluginList, 
                                       QWidget* parent)
    : QDialog(parent)
    , m_entries(entries)
    , m_pluginList(pluginList)
{
    setWindowTitle(tr("Edit GameShark Codes"));
    resize(600, 400);
    
    // Create layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Create codes edit area - fix QLabel usage
    QLabel* codesLabel = new QLabel(tr("Enter GameShark codes (one per line, format: XXXXXXXX YYYYYYYY):"), this);
    m_codesEdit = new QPlainTextEdit();
    
    // Add existing codes to edit field
    QString codeText;
    for (const auto& entry : entries) {
        if (!codeText.isEmpty()) {
            codeText.append("\n");
        }
        codeText.append(QString("%1 %2").arg(entry.Command, 8, 16, QChar('0')).arg(entry.Value));
    }
    m_codesEdit->setPlainText(codeText);
    
    // Create plugin options
    m_limitPluginsCheckbox = new QCheckBox(tr("Limit to specific plugins"));
    m_editPluginsButton = new QPushButton(tr("Edit Plugins..."));
    m_editPluginsButton->setEnabled(!pluginList.empty());
    
    // Add widgets to layout
    mainLayout->addWidget(codesLabel);
    mainLayout->addWidget(m_codesEdit);
    mainLayout->addWidget(m_limitPluginsCheckbox);
    
    QHBoxLayout* pluginBtnLayout = new QHBoxLayout();
    pluginBtnLayout->addWidget(m_editPluginsButton);
    pluginBtnLayout->addStretch();
    mainLayout->addLayout(pluginBtnLayout);
    
    // Create button box
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &EditGamesharkDialog::onAccept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
    // Add button box to main layout
    mainLayout->addWidget(buttonBox);
    
    // Connect signals
    connect(m_limitPluginsCheckbox, &QCheckBox::toggled, this, &EditGamesharkDialog::onLimitPluginsToggled);
    connect(m_editPluginsButton, &QPushButton::clicked, this, &EditGamesharkDialog::onEditPlugins);
    
    // Set initial state
    m_limitPluginsCheckbox->setChecked(!pluginList.empty());
}

void EditGamesharkDialog::onLimitPluginsToggled(bool checked)
{
    m_editPluginsButton->setEnabled(checked);
    
    if (!checked) {
        m_pluginList.clear();
    }
}

void EditGamesharkDialog::onEditPlugins()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Select Plugins"));
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    // Create list widget for plugins
    QListWidget* listWidget = new QListWidget();
    layout->addWidget(listWidget);
    
    // Create buttons for adding/removing plugins
    QHBoxLayout* btnLayout = new QHBoxLayout();
    QLineEdit* addEdit = new QLineEdit();
    QPushButton* addButton = new QPushButton(tr("Add"));
    QPushButton* removeButton = new QPushButton(tr("Remove"));
    
    btnLayout->addWidget(addEdit);
    btnLayout->addWidget(addButton);
    btnLayout->addWidget(removeButton);
    layout->addLayout(btnLayout);
    
    // Add dialog buttons
    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttons);
    
    // Populate list with current plugins
    for (const auto& plugin : m_pluginList) {
        listWidget->addItem(plugin);
    }
    
    // Connect signals
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    // Show dialog and process result
    if (dialog.exec() == QDialog::Accepted) {
        // Update plugin list
        m_pluginList.clear();
        for (int i = 0; i < listWidget->count(); ++i) {
            m_pluginList.push_back(listWidget->item(i)->text());
        }
    }
}

void EditGamesharkDialog::onAccept()
{
    if (!parseCheatCodes(m_entries)) {
        QMessageBox::warning(this, tr("Error"), tr("Invalid GameShark codes detected. Please check your input."));
        return;
    }
    
    accept();
}

bool EditGamesharkDialog::parseCheatCodes(EnhancementData::CodeEntries& entries)
{
    // Clear existing entries
    entries.clear();
    
    QString text = m_codesEdit->toPlainText();
    QStringList lines = text.split('\n');
    
    QRegularExpression codePattern("([0-9A-F]{8})\\s+([0-9A-F]+)", QRegularExpression::CaseInsensitiveOption);
    
    for (const QString& line : lines) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine.isEmpty()) {
            continue;
        }
        
        QRegularExpressionMatch match = codePattern.match(trimmedLine);
        if (!match.hasMatch()) {
            return false;
        }
        
        EnhancementData::CodeEntry entry;
        entry.Command = match.captured(1).toUInt(nullptr, 16);
        entry.Value = match.captured(2);
        entries.push_back(entry);
    }
    
    return true;
}

} // namespace QT_UI
