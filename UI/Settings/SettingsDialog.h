#pragma once

#include <QDialog>
#include <QTabWidget>
#include <QPushButton>
#include <QWidget>

namespace QT_UI {

// Define SettingsPlaceholderWidget only in the header
class SettingsPlaceholderWidget : public QWidget
{
    Q_OBJECT
public:
    SettingsPlaceholderWidget(const QString& title, QWidget* parent = nullptr);

private:
    QString m_title;
};

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog();
    
    // Add missing method declarations
    int exec(int initialTab = 0);

private slots:
    void onApply();
    void onOk();
    void onCancel();
    void onReset();
    void onTabChanged(int index);

private:
    void setupUi();
    void applySettings();
    
    QTabWidget* m_tabWidget;
    QPushButton* m_applyButton;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    QPushButton* m_resetButton;
    
    // Settings page widgets
    QWidget* m_generalSettings;
    QWidget* m_graphicsSettings;
    QWidget* m_audioSettings;
    QWidget* m_inputSettings;
    QWidget* m_advancedSettings;
};

} // namespace QT_UI
