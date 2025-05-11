#include "SettingsPage.h"
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include "../../Settings/SettingsID.h"
#include "../../Settings/Settings.h"

namespace QT_UI {

SettingsPage::SettingsPage(QWidget* parent) 
    : QWidget(parent), 
      m_updatingUI(false)
{
}

SettingsPage::~SettingsPage()
{
    // Clean up all created objects
    qDeleteAll(m_checkBoxes);
    qDeleteAll(m_textBoxes);
    qDeleteAll(m_comboBoxes);
}

bool SettingsPage::isModified() const
{
    // Check if any settings have been modified
    for (auto* checkbox : m_checkBoxes) {
        if (checkbox->isModified())
            return true;
    }
    
    for (auto* textbox : m_textBoxes) {
        if (textbox->isModified())
            return true;
    }
    
    for (auto* combobox : m_comboBoxes) {
        if (combobox->isModified())
            return true;
    }
    
    return false;
}

ModifiedCheckBox* SettingsPage::addCheckBox(QCheckBox* checkBox, SettingID settingID)
{
    // Create and configure the modified checkbox
    ModifiedCheckBox* modifiedCheckBox = new ModifiedCheckBox(checkBox);
    m_checkBoxes.insert(settingID, modifiedCheckBox);
    
    // Set initial state from settings
    bool value = false;
    bool exists = g_Settings->LoadBool(settingID, value);
    modifiedCheckBox->setChecked(value);
    modifiedCheckBox->setDefaultChecked(value);
    modifiedCheckBox->setModified(false);
    
    return modifiedCheckBox;
}

ModifiedLineEdit* SettingsPage::addTextBox(QLineEdit* lineEdit, SettingID settingID, bool isString)
{
    // Create and configure the modified line edit
    ModifiedLineEdit* modifiedLineEdit = new ModifiedLineEdit(lineEdit, isString);
    m_textBoxes.insert(settingID, modifiedLineEdit);
    
    // Set initial value from settings
    if (isString) {
        stdstr value;
        g_Settings->LoadStringVal(settingID, value);
        modifiedLineEdit->setText(QString::fromStdString(value));
        modifiedLineEdit->setDefaultText(QString::fromStdString(value));
    } else {
        uint32_t value = 0;
        g_Settings->LoadDword(settingID, value);
        modifiedLineEdit->setText(QString::number(value));
        modifiedLineEdit->setDefaultText(QString::number(value));
    }
    
    modifiedLineEdit->setModified(false);
    return modifiedLineEdit;
}

ModifiedComboBox* SettingsPage::addComboBox(QComboBox* comboBox, SettingID settingID)
{
    // Create and configure the modified combo box
    ModifiedComboBox* modifiedComboBox = new ModifiedComboBox(comboBox);
    m_comboBoxes.insert(settingID, modifiedComboBox);
    
    // Set initial value from settings
    uint32_t value = 0;
    bool exists = g_Settings->LoadDword(settingID, value);
    
    // Find the matching index
    for (int i = 0; i < comboBox->count(); i++) {
        QVariant itemData = comboBox->itemData(i);
        if (itemData.toUInt() == value) {
            modifiedComboBox->setCurrentIndex(i);
            modifiedComboBox->setDefaultIndex(i);
            break;
        }
    }
    
    modifiedComboBox->setModified(false);
    return modifiedComboBox;
}

ModifiedComboBox* SettingsPage::addComboBoxTextBased(QComboBox* comboBox, SettingID settingID)
{
    // Create and configure the text-based modified combo box
    ModifiedComboBox* modifiedComboBox = new ModifiedComboBox(comboBox);
    m_comboBoxes.insert(settingID, modifiedComboBox);
    
    // Set initial value from settings
    stdstr value;
    bool exists = g_Settings->LoadStringVal(settingID, value);
    
    // Find the matching index
    for (int i = 0; i < comboBox->count(); i++) {
        QString itemText = comboBox->itemText(i);
        if (itemText.toStdString() == value) {
            modifiedComboBox->setCurrentIndex(i);
            modifiedComboBox->setDefaultIndex(i);
            break;
        }
    }
    
    modifiedComboBox->setModified(false);
    return modifiedComboBox;
}

void SettingsPage::applySettings()
{
    applyCheckBoxes();
    applyTextBoxes();
    applyComboBoxes();
}

void SettingsPage::applyCheckBoxes()
{
    for (auto it = m_checkBoxes.begin(); it != m_checkBoxes.end(); ++it) {
        SettingID settingID = it.key();
        ModifiedCheckBox* checkBox = it.value();
        
        if (checkBox->isModified()) {
            g_Settings->SaveBool(settingID, checkBox->isChecked());
            checkBox->setModified(false);
        }
        
        if (checkBox->isReset()) {
            g_Settings->DeleteSetting(settingID);
            checkBox->setReset(false);
        }
    }
}

void SettingsPage::applyTextBoxes()
{
    for (auto it = m_textBoxes.begin(); it != m_textBoxes.end(); ++it) {
        SettingID settingID = it.key();
        ModifiedLineEdit* textBox = it.value();
        
        if (textBox->isModified()) {
            if (textBox->isString()) {
                g_Settings->SaveString(settingID, textBox->text().toStdString().c_str());
            } else {
                bool ok;
                uint32_t value = textBox->text().toUInt(&ok);
                if (ok) {
                    g_Settings->SaveDword(settingID, value);
                }
            }
            textBox->setModified(false);
        }
        
        if (textBox->isReset()) {
            g_Settings->DeleteSetting(settingID);
            textBox->setReset(false);
        }
    }
}

void SettingsPage::applyComboBoxes()
{
    for (auto it = m_comboBoxes.begin(); it != m_comboBoxes.end(); ++it) {
        SettingID settingID = it.key();
        ModifiedComboBox* comboBox = it.value();
        
        if (comboBox->isModified()) {
            int index = comboBox->currentIndex();
            if (index >= 0) {
                QVariant data = comboBox->itemData(index);
                if (data.type() == QVariant::String) {
                    g_Settings->SaveString(settingID, data.toString().toStdString().c_str());
                } else {
                    g_Settings->SaveDword(settingID, data.toUInt());
                }
            }
            comboBox->setModified(false);
        }
        
        if (comboBox->isReset()) {
            g_Settings->DeleteSetting(settingID);
            comboBox->setReset(false);
        }
    }
}

void SettingsPage::resetSettings()
{
    // Reset all settings to their default values
    for (auto it = m_checkBoxes.begin(); it != m_checkBoxes.end(); ++it) {
        ModifiedCheckBox* checkBox = it.value();
        SettingID settingID = it.key();
        
        bool defaultValue = false;
        g_Settings->LoadDefaultBool(settingID, defaultValue);
        checkBox->setChecked(defaultValue);
        checkBox->setReset(true);
    }
    
    for (auto it = m_textBoxes.begin(); it != m_textBoxes.end(); ++it) {
        ModifiedLineEdit* textBox = it.value();
        SettingID settingID = it.key();
        
        if (textBox->isString()) {
            stdstr defaultValue;
            g_Settings->LoadDefaultString(settingID, defaultValue);
            textBox->setText(QString::fromStdString(defaultValue));
        } else {
            uint32_t defaultValue = 0;
            g_Settings->LoadDefaultDword(settingID, defaultValue);
            textBox->setText(QString::number(defaultValue));
        }
        textBox->setReset(true);
    }
    
    for (auto it = m_comboBoxes.begin(); it != m_comboBoxes.end(); ++it) {
        ModifiedComboBox* comboBox = it.value();
        SettingID settingID = it.key();
        
        // Reset to default based on whether it's string or dword
        QVariant firstItemData = comboBox->itemData(0);
        if (firstItemData.type() == QVariant::String) {
            stdstr defaultValue;
            g_Settings->LoadDefaultString(settingID, defaultValue);
            
            // Find matching item
            for (int i = 0; i < comboBox->currentIndex(); i++) {
                QVariant data = comboBox->itemData(i);
                if (data.toString().toStdString() == defaultValue) {
                    comboBox->setCurrentIndex(i);
                    break;
                }
            }
        } else {
            uint32_t defaultValue = 0;
            g_Settings->LoadDefaultDword(settingID, defaultValue);
            
            // Find matching item
            for (int i = 0; i < comboBox->currentIndex(); i++) {
                QVariant data = comboBox->itemData(i);
                if (data.toUInt() == defaultValue) {
                    comboBox->setCurrentIndex(i);
                    break;
                }
            }
        }
        comboBox->setReset(true);
    }
}

void SettingsPage::loadSettings()
{
    // Update all controls with current settings values
    m_updatingUI = true;
    
    for (auto it = m_checkBoxes.begin(); it != m_checkBoxes.end(); ++it) {
        SettingID settingID = it.key();
        ModifiedCheckBox* checkBox = it.value();
        
        bool value = false;
        bool exists = g_Settings->LoadBool(settingID, value);
        checkBox->setChecked(value);
        checkBox->setModified(false);
    }
    
    for (auto it = m_textBoxes.begin(); it != m_textBoxes.end(); ++it) {
        SettingID settingID = it.key();
        ModifiedLineEdit* textBox = it.value();
        
        if (textBox->isString()) {
            stdstr value;
            bool exists = g_Settings->LoadStringVal(settingID, value);
            textBox->setText(QString::fromStdString(value));
        } else {
            uint32_t value = 0;
            bool exists = g_Settings->LoadDword(settingID, value);
            textBox->setText(QString::number(value));
        }
        textBox->setModified(false);
    }
    
    for (auto it = m_comboBoxes.begin(); it != m_comboBoxes.end(); ++it) {
        SettingID settingID = it.key();
        ModifiedComboBox* comboBox = it.value();
        
        // Determine type based on first item's data type
        QVariant firstItemData = comboBox->itemData(0);
        if (firstItemData.type() == QVariant::String) {
            stdstr value;
            bool exists = g_Settings->LoadStringVal(settingID, value);
            
            // Find matching item
            for (int i = 0; i < comboBox->currentIndex(); i++) {
                QVariant data = comboBox->itemData(i);
                if (data.toString().toStdString() == value) {
                    comboBox->setCurrentIndex(i);
                    break;
                }
            }
        } else {
            uint32_t value = 0;
            bool exists = g_Settings->LoadDword(settingID, value);
            
            // Find matching item
            for (int i = 0; i < comboBox->currentIndex(); i++) {
                QVariant data = comboBox->itemData(i);
                if (data.toUInt() == value) {
                    comboBox->setCurrentIndex(i);
                    break;
                }
            }
        }
        comboBox->setModified(false);
    }
    
    m_updatingUI = false;
}

// ModifiedCheckBox implementation
ModifiedCheckBox::ModifiedCheckBox(QCheckBox* checkBox)
    : QObject(checkBox),
      m_checkBox(checkBox),
      m_modified(false),
      m_reset(false),
      m_defaultState(false)
{
    connect(checkBox, &QCheckBox::stateChanged, this, &ModifiedCheckBox::onStateChanged);
}

bool ModifiedCheckBox::isChecked() const
{
    return m_checkBox->isChecked();
}

void ModifiedCheckBox::setChecked(bool checked)
{
    m_checkBox->setChecked(checked);
}

void ModifiedCheckBox::setDefaultChecked(bool checked)
{
    m_defaultState = checked;
    m_checkBox->setChecked(checked);
}

void ModifiedCheckBox::onStateChanged(int state)
{
    m_modified = (state == Qt::Checked) != m_defaultState;
}

// ModifiedLineEdit implementation
ModifiedLineEdit::ModifiedLineEdit(QLineEdit* lineEdit, bool isString)
    : QObject(lineEdit),
      m_lineEdit(lineEdit),
      m_modified(false),
      m_reset(false),
      m_isString(isString),
      m_defaultText("")
{
    connect(lineEdit, &QLineEdit::textChanged, this, &ModifiedLineEdit::onTextChanged);
}

QString ModifiedLineEdit::text() const
{
    return m_lineEdit->text();
}

void ModifiedLineEdit::setText(const QString& text)
{
    m_lineEdit->setText(text);
}

void ModifiedLineEdit::setDefaultText(const QString& text)
{
    m_defaultText = text;
    m_lineEdit->setText(text);
}

void ModifiedLineEdit::onTextChanged(const QString& text)
{
    m_modified = (text != m_defaultText);
}

// ModifiedComboBox implementation
ModifiedComboBox::ModifiedComboBox(QComboBox* comboBox)
    : QObject(comboBox),
      m_comboBox(comboBox),
      m_modified(false),
      m_reset(false),
      m_defaultIndex(-1)
{
    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ModifiedComboBox::onCurrentIndexChanged);
}

int ModifiedComboBox::currentIndex() const
{
    return m_comboBox->currentIndex();
}

void ModifiedComboBox::setCurrentIndex(int index)
{
    m_comboBox->setCurrentIndex(index);
}

void ModifiedComboBox::setDefaultIndex(int index)
{
    m_defaultIndex = index;
    m_comboBox->setCurrentIndex(index);
}

void ModifiedComboBox::setItemData(int index, const QVariant& value)
{
    m_comboBox->setItemData(index, value);
}

QVariant ModifiedComboBox::itemData(int index) const
{
    return m_comboBox->itemData(index);
}

void ModifiedComboBox::onCurrentIndexChanged(int index)
{
    m_modified = (index != m_defaultIndex);
}

} // namespace QT_UI
