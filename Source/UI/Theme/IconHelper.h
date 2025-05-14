#pragma once

#include <QApplication>
#include <QIcon>
#include <QStyle>
#include "ThemeManager.h"

namespace QT_UI
{

class IconHelper
{
public:
    // File menu icons
    static QIcon getOpenIcon() { return getThemedIcon("folder-open-line"); }
    static QIcon getRomInfoIcon() { return getThemedIcon("file-search-line"); }
    static QIcon getPlayIcon() { return getThemedIcon("play-line"); }
    static QIcon getStopIcon() { return getThemedIcon("shut-down-line"); }
    static QIcon getPauseIcon() { return getThemedIcon("pause-line"); }
    static QIcon getRefreshIcon() { return getThemedIcon("refresh-line"); }
    static QIcon getExitIcon() { return getThemedIcon("door-open-line"); }
    static QIcon getFolderIcon() { return getThemedIcon("folder-line"); }
    static QIcon getFolderSettingsIcon() { return getThemedIcon("folder-settings-line"); }
    static QIcon getRecentIcon() { return getThemedIcon("time-line"); }
    static QIcon getGlobalIcon() { return getThemedIcon("global-line"); }
    
    // Options menu icons
    static QIcon getFullscreenIcon() { return getThemedIcon("fullscreen-line"); }
    static QIcon getPinIcon() { return getThemedIcon("pin-filled"); }
    static QIcon getGraphicsIcon() { return getThemedIcon("brush-line"); }
    static QIcon getAudioIcon() { return getThemedIcon("volume-up-line"); }
    static QIcon getRspIcon() { return getThemedIcon("chip-line"); }
    static QIcon getControllerIcon() { return getThemedIcon("controller-line"); }
    static QIcon getStatisticsIcon() { return getThemedIcon("function-line"); }
    static QIcon getSettingsIcon() { return getThemedIcon("settings-3-line"); }
    
    // View menu icons
    static QIcon getDetailViewIcon() { return getThemedIcon("list-check"); }
    static QIcon getGridViewIcon() { return getThemedIcon("artboard-2-line"); }
    static QIcon getShowTitlesIcon() { return getThemedIcon("price-tag-3-line"); }
    static QIcon getThemeIcon() { return getThemedIcon("brush-line"); }
    
    // Debug menu icons
    static QIcon getDebugIcon() { return getThemedIcon("debugger-line"); }
    static QIcon getStepIntoIcon() { return getThemedIcon("debug-step-into-line"); }
    static QIcon getStepOverIcon() { return getThemedIcon("debug-step-over-line"); }
    static QIcon getStepOutIcon() { return getThemedIcon("debug-step-out-line"); }
    
    // Tools menu icons
    static QIcon getToolsIcon() { return getThemedIcon("tools-line"); }
    static QIcon getDownloadIcon() { return getThemedIcon("download-2-line"); }
    
    // Help menu icons
    static QIcon getHelpIcon() { return getThemedIcon("question-mark"); }
    static QIcon getSupportIcon() { return getThemedIcon("heart-circle-line"); }
    static QIcon getDiscordIcon() { return getThemedIcon("discord"); }
    static QIcon getWebsiteIcon() { return getThemedIcon("global-line"); }
    static QIcon getAboutIcon() { return getThemedIcon("information-line"); }
    
    // File type icons
    static QIcon getRomDefaultIcon() { return getThemedIcon("file-line"); }
    static QIcon getSaveIcon() { return getThemedIcon("save-3-line"); }
    static QIcon getScreenshotIcon() { return getThemedIcon("screenshot-2-line"); }
    
    // Common action icons
    static QIcon getAddIcon() { return getThemedIcon("plus-line"); }
    static QIcon getDeleteIcon() { return getThemedIcon("delete-back-2-line"); }
    static QIcon getEditIcon() { return getThemedIcon("pencil-line"); }
    static QIcon getTrashIcon() { return getThemedIcon("trash-fill"); }
    static QIcon getSearchIcon() { return getThemedIcon("magnifier-line"); }
    static QIcon getLockIcon() { return getThemedIcon("padlock-lock"); }
    static QIcon getUnlockIcon() { return getThemedIcon("padlock-unlock"); }
    static QIcon getImageIcon() { return getThemedIcon("image-fill"); }
    static QIcon getZoomInIcon() { return getThemedIcon("zoom-in-line"); }
    static QIcon getZoomOutIcon() { return getThemedIcon("zoom-out-line"); }

    // Region flag icons - these don't need theming as they are always the same
    static QIcon getUSAFlagIcon() { return QIcon(":/icons/flags/usa.svg"); }
    static QIcon getEuropeFlagIcon() { return QIcon(":/icons/flags/europe.svg"); }
    static QIcon getJapanFlagIcon() { return QIcon(":/icons/flags/japan.svg"); }
    static QIcon getGermanyFlagIcon() { return QIcon(":/icons/flags/germany.svg"); }
    static QIcon getFranceFlagIcon() { return QIcon(":/icons/flags/france.svg"); }
    static QIcon getItalyFlagIcon() { return QIcon(":/icons/flags/italy.svg"); }
    static QIcon getSpainFlagIcon() { return QIcon(":/icons/flags/spain.svg"); }
    static QIcon getAustraliaFlagIcon() { return QIcon(":/icons/flags/australia.svg"); }
    static QIcon getUnknownFlagIcon() { return getThemedIcon("global-line"); }
    
private:
    static QIcon getThemedIcon(const QString& iconName)
    {
        QString themePath = ThemeManager::isCurrentlyDarkTheme() ? ":/icons/white/" : ":/icons/black/";
        return QIcon(themePath + iconName + ".svg");
    }
};

} // namespace QT_UI
