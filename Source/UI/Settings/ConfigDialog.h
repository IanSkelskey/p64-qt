#ifndef CONFIG_DIALOG_H
#define CONFIG_DIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QMap>
#include <QString>

namespace QT_UI {

class BaseSettingsPage;

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = nullptr);
    ~ConfigDialog();

private slots:
    void applySettings();
    void resetSettings();
    void tabChanged(int index);
    
signals:
    // Signal emitted when column settings have been changed
    void columnSettingsChanged();

private:
    void setupUi();
    void addSettingsPage(BaseSettingsPage* page);
    
    QTabWidget* m_tabWidget;
    QDialogButtonBox* m_buttonBox;
    QMap<QString, BaseSettingsPage*> m_settingsPages;
    int m_currentTab;
};

} // namespace QT_UI

#endif // CONFIG_DIALOG_H
