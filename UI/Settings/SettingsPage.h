#pragma once

#include <QWidget>
#include <QMap>
#include <QVariant>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QObject>

#include "../../Settings/SettingType/SettingsType.h"

namespace QT_UI {

// Forward declarations
class ModifiedCheckBox;
class ModifiedLineEdit;
class ModifiedComboBox;

/**
 * @brief Base class for all settings pages
 * 
 * This class provides common functionality for all settings pages
 * including tracking modifications, applying settings, and resetting to defaults
 */
class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget* parent = nullptr);
    virtual ~SettingsPage();

    // Settings page functionality
    virtual void applySettings();
    virtual void resetSettings();
    virtual void loadSettings();
    
    // Check if any setting has been changed
    bool isModified() const;

protected:
    // Helper methods for widgets
    ModifiedCheckBox* addCheckBox(QCheckBox* checkBox, SettingID settingID);
    ModifiedLineEdit* addTextBox(QLineEdit* lineEdit, SettingID settingID, bool isString = true);
    ModifiedComboBox* addComboBox(QComboBox* comboBox, SettingID settingID);
    
    // Specialized add methods for string combo box
    ModifiedComboBox* addComboBoxTextBased(QComboBox* comboBox, SettingID settingID);
    
    // Apply methods for different widget types
    void applyCheckBoxes();
    void applyTextBoxes();
    void applyComboBoxes();

private:
    QMap<SettingID, ModifiedCheckBox*> m_checkBoxes;
    QMap<SettingID, ModifiedLineEdit*> m_textBoxes;
    QMap<SettingID, ModifiedComboBox*> m_comboBoxes;
    bool m_updatingUI;
};

/**
 * @brief Modified checkbox class that tracks changes
 */
class ModifiedCheckBox : public QObject
{
    Q_OBJECT
    
public:
    explicit ModifiedCheckBox(QCheckBox* checkBox);
    
    bool isModified() const { return m_modified; }
    bool isReset() const { return m_reset; }
    bool isChecked() const;
    
    void setReset(bool reset) { m_reset = reset; }
    void setModified(bool modified) { m_modified = modified; }
    void setChecked(bool checked);
    void setDefaultChecked(bool checked);
    
private slots:
    void onStateChanged(int state);
    
private:
    QCheckBox* m_checkBox;
    bool m_modified;
    bool m_reset;
    bool m_defaultState;
};

/**
 * @brief Modified line edit that tracks changes
 */
class ModifiedLineEdit : public QObject
{
    Q_OBJECT
    
public:
    explicit ModifiedLineEdit(QLineEdit* lineEdit, bool isString = true);
    
    bool isModified() const { return m_modified; }
    bool isReset() const { return m_reset; }
    bool isString() const { return m_isString; }
    
    void setReset(bool reset) { m_reset = reset; }
    void setModified(bool modified) { m_modified = modified; }
    QString text() const;
    void setText(const QString& text);
    void setDefaultText(const QString& text);
    
private slots:
    void onTextChanged(const QString& text);
    
private:
    QLineEdit* m_lineEdit;
    bool m_modified;
    bool m_reset;
    bool m_isString;
    QString m_defaultText;
};

/**
 * @brief Modified combo box that tracks changes
 */
class ModifiedComboBox : public QObject
{
    Q_OBJECT
    
public:
    explicit ModifiedComboBox(QComboBox* comboBox);
    
    bool isModified() const { return m_modified; }
    bool isReset() const { return m_reset; }
    
    void setReset(bool reset) { m_reset = reset; }
    void setModified(bool modified) { m_modified = modified; }
    int currentIndex() const;
    void setCurrentIndex(int index);
    void setDefaultIndex(int index);
    
    // For string-based combo boxes
    void setItemData(int index, const QVariant& value);
    QVariant itemData(int index) const;
    
private slots:
    void onCurrentIndexChanged(int index);
    
private:
    QComboBox* m_comboBox;
    bool m_modified;
    bool m_reset;
    int m_defaultIndex;
};

} // namespace QT_UI
