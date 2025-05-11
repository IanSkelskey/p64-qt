#pragma once

#include <QDialog>
#include <QTreeWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <vector>
#include <string>

// Forward declarations
class QSplitter;

namespace QT_UI {

// Simple data structure to hold enhancement data
class EnhancementData {
public:
    // Data structures
    struct CodeEntry {
        uint32_t Command;
        QString Value;
    };
    
    typedef std::vector<CodeEntry> CodeEntries;
    typedef std::vector<QString> PluginList;
    
    // Constructor
    EnhancementData() : 
        name(""), 
        author(""),
        notes(""),
        enabledByDefault(false),
        overclock(false),
        overclockModifier(1)
    {}
    
    // Public members for simplicity during development
    QString name;
    QString author;
    QString notes;
    bool enabledByDefault;
    bool overclock;
    int overclockModifier;
    CodeEntries codeEntries;
    PluginList pluginList;
};

// Main dialog to display and manage enhancements
class EnhancementDialog : public QDialog {
    Q_OBJECT
public:
    EnhancementDialog(QWidget* parent = nullptr);
    ~EnhancementDialog();

    // Accessor for testing
    std::vector<EnhancementData*>& getEnhancements() { return m_enhancements; }

private slots:
    void onItemChanged(QTreeWidgetItem* item, int column);
    void onItemSelectionChanged();
    void onEditEnhancement();
    void onAddEnhancement();
    void onRemoveEnhancement();

private:
    void loadEnhancements() {} // Stub - will be implemented later
    void saveEnhancements() {} // Stub - will be implemented later
    void refreshList() {} // Stub - will be implemented later
    void updateEnhancementState(QTreeWidgetItem* item, bool checked) {} // Stub

    QTreeWidget* m_treeWidget;
    QTextEdit* m_notesEdit;
    QPushButton* m_addButton;
    QPushButton* m_editButton;
    QPushButton* m_removeButton;

    std::vector<EnhancementData*> m_enhancements;
};

// Dialog to edit an enhancement
class EditEnhancementDialog : public QDialog {
    Q_OBJECT
public:
    EditEnhancementDialog(EnhancementDialog& parent, EnhancementData* enhancement = nullptr);
    ~EditEnhancementDialog();

private slots:
    void onGamesharkToggled(bool checked);
    void onOverClockToggled(bool checked);
    void onEditGameshark();
    void onAccept();

private:
    EnhancementDialog& m_parent;
    EnhancementData* m_enhancement;
    EnhancementData::CodeEntries m_entries;
    EnhancementData::PluginList m_pluginList;

    QLineEdit* m_nameEdit;
    QLineEdit* m_authorEdit;
    QTextEdit* m_notesEdit;
    QCheckBox* m_gamesharkCheckbox;
    QCheckBox* m_overclockCheckbox;
    QLineEdit* m_overclockModifierEdit;
    QPushButton* m_editGamesharkButton;
    QCheckBox* m_autoOnCheckbox;
};

// Dialog to edit GameShark codes
class EditGamesharkDialog : public QDialog {
    Q_OBJECT
public:
    EditGamesharkDialog(EnhancementData::CodeEntries& entries, 
                      EnhancementData::PluginList& pluginList, 
                      QWidget* parent = nullptr);
    ~EditGamesharkDialog() {}

private slots:
    void onLimitPluginsToggled(bool checked);
    void onEditPlugins();
    void onAccept();

private:
    bool parseCheatCodes(EnhancementData::CodeEntries& entries);

    EnhancementData::CodeEntries& m_entries;
    EnhancementData::PluginList& m_pluginList;
    
    QPlainTextEdit* m_codesEdit;
    QCheckBox* m_limitPluginsCheckbox;
    QPushButton* m_editPluginsButton;
};

} // namespace QT_UI
