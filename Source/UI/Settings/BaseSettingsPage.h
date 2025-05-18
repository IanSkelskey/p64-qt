#pragma once

#include <QWidget>

namespace QT_UI {

/**
 * @brief Base class for all settings pages
 * 
 * This abstract class provides a common interface for all settings pages
 * in the configuration dialog.
 */
class BaseSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit BaseSettingsPage(QWidget* parent = nullptr) : QWidget(parent) {}
    virtual ~BaseSettingsPage() = default;

    /**
     * @brief Load settings from the application settings
     */
    virtual void loadSettings() = 0;

    /**
     * @brief Save current settings to the application settings
     */
    virtual void saveSettings() = 0;

    /**
     * @brief Reset settings to defaults
     */
    virtual void resetSettings() = 0;

    /**
     * @brief Get the title for this settings page
     */
    virtual QString pageTitle() const = 0;

signals:
    /**
     * @brief Signal emitted when settings are changed
     */
    void settingsChanged();
};

} // namespace QT_UI
