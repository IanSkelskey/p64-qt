#pragma once

#include "BaseSettingsPage.h"
#include <QCheckBox>
#include <QListWidget>
#include <QPushButton>
#include <QVector>
#include <QComboBox>
#include <QDoubleSpinBox>

namespace QT_UI {

class RomBrowserSettingsPage : public BaseSettingsPage
{
    Q_OBJECT

public:
    explicit RomBrowserSettingsPage(QWidget* parent = nullptr);
    ~RomBrowserSettingsPage() override = default;

    void loadSettings() override;
    void saveSettings() override;
    void resetSettings() override;
    QString pageTitle() const override { return tr("ROM Browser"); }

private slots:
    void addSelectedColumn();
    void removeSelectedColumn();
    void moveColumnUp();
    void moveColumnDown();
    void updateColumnButtonStates();
    void romBrowserEnabledChanged(bool enabled);
    void viewModeChanged(int index);

signals:
    void columnSettingsChanged();

private:
    void setupUI();
    void populateColumnLists();
    
    // Column info structure to track columns and their states
    struct ColumnInfo {
        int columnId;
        QString name;
        bool selected;
        int position;
    };
    
    // ROM Browser options
    QCheckBox* m_useRomBrowserCheck;
    QCheckBox* m_recursionCheck;
    QCheckBox* m_showExtensionsCheck;
    
    // Column management
    QListWidget* m_availableColumnsList;
    QListWidget* m_shownColumnsList;
    QPushButton* m_addColumnButton;
    QPushButton* m_removeColumnButton;
    QPushButton* m_moveUpButton;
    QPushButton* m_moveDownButton;
    QVector<ColumnInfo> m_columnInfo;
    
    // View mode settings
    QComboBox* m_viewModeCombo;
    QCheckBox* m_showTitlesCheck;
    QDoubleSpinBox* m_coverScaleSpinBox;
};

} // namespace QT_UI
