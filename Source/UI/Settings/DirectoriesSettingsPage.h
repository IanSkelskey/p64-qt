#pragma once

#include "BaseSettingsPage.h"
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

namespace QT_UI {

class DirectoriesSettingsPage : public BaseSettingsPage
{
    Q_OBJECT

public:
    explicit DirectoriesSettingsPage(QWidget* parent = nullptr);
    ~DirectoriesSettingsPage() override = default;

    void loadSettings() override;
    void saveSettings() override;
    void resetSettings() override;
    QString pageTitle() const override { return tr("Directories"); }

private slots:
    void browsePluginDir();
    void browseSavesDir();
    void browseScreenshotsDir();
    void browseCoversDir();
    void toggleUseDefaultPluginDir(bool checked);
    void toggleUseDefaultSavesDir(bool checked);

private:
    void setupUI();

    // Plugin directory
    QLineEdit* m_pluginDirEdit;
    QPushButton* m_browsePluginButton;
    QCheckBox* m_useDefaultPluginDirCheck;
    
    // Saves directory
    QLineEdit* m_savesDirEdit;
    QPushButton* m_browseSavesButton;
    QCheckBox* m_useDefaultSavesDirCheck;
    
    // Screenshots directory
    QLineEdit* m_screenshotsDirEdit;
    QPushButton* m_browseScreenshotsButton;
    QCheckBox* m_useDefaultScreenshotsDirCheck;
    
    // Covers directory
    QLineEdit* m_coversDirEdit;
    QPushButton* m_browseCoversButton;
    QCheckBox* m_useDefaultCoversDirCheck;
};

} // namespace QT_UI
