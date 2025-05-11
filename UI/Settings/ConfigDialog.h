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

private:
    void setupUi();
    void loadSettings();
    void saveSettings();

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
    
    QDialogButtonBox *m_buttonBox;
};

} // namespace QT_UI

#endif // CONFIG_DIALOG_H
