#ifndef CONFIG_DIALOG_H
#define CONFIG_DIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QToolBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QVector>

namespace QT_UI {

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = nullptr);
    ~ConfigDialog();

private slots:
    void browsePluginDir();
    void browseSavesDir();
    void applySettings();
    void resetSettings();
    
    // ROM Browser column management slots
    void addSelectedColumn();
    void removeSelectedColumn();
    void moveColumnUp();
    void moveColumnDown();
    void updateColumnButtonStates();
    void romBrowserEnabledChanged(bool enabled);

signals:
    // Signal emitted when column settings have been changed
    void columnSettingsChanged();

private:
    void setupUi();
    void setupRomBrowserTab();
    void loadSettings();
    void saveSettings();
    void populateColumnLists();
    void loadColumnSettings(); // Add this missing function declaration
    
    QTabWidget *m_tabWidget;
    
    // General tab widgets
    QToolBox *m_generalToolBox;
    QCheckBox *m_pauseWhenInactiveCheck;
    QCheckBox *m_enterFullscreenCheck;
    QCheckBox *m_disableScreensaverCheck;
    QCheckBox *m_enableDiscordCheck;
    QCheckBox *m_hideAdvancedSettingsCheck;
    QSpinBox *m_maxRomsSpinBox;
    QSpinBox *m_maxRomDirsSpinBox;
    
    // Directories tab widgets
    QLineEdit *m_pluginDirEdit;
    QPushButton *m_browsePluginButton;
    QCheckBox *m_useDefaultPluginDirCheck;
    QLineEdit *m_savesDirEdit;
    QPushButton *m_browseSavesButton;
    QCheckBox *m_useDefaultSavesDirCheck;
    
    // ROM Browser tab widgets
    QCheckBox *m_useRomBrowserCheck;
    QCheckBox *m_recursionCheck;
    QCheckBox *m_showExtensionsCheck;
    QListWidget *m_availableColumnsList;
    QListWidget *m_shownColumnsList;
    QPushButton *m_addColumnButton;
    QPushButton *m_removeColumnButton;
    QPushButton *m_moveUpButton;
    QPushButton *m_moveDownButton;
    
    // Column info structure to track columns and their states
    struct ColumnInfo {
        int columnId;
        QString name;
        bool selected;
        int position;
    };
    QVector<ColumnInfo> m_columnInfo;
    
    QDialogButtonBox *m_buttonBox;
};

} // namespace QT_UI

#endif // CONFIG_DIALOG_H
