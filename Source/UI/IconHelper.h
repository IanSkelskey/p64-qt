#pragma once

#include <QApplication>
#include <QIcon>
#include <QStyle>

namespace QT_UI
{

class IconHelper
{
public:
    // General UI icons
    static QIcon getGridViewIcon()
    {
        return QApplication::style()->standardIcon(QStyle::SP_FileDialogListView);
    }

    static QIcon getDetailViewIcon()
    {
        return QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView);
    }

    static QIcon getShowTitlesIcon()
    {
        return QApplication::style()->standardIcon(QStyle::SP_FileDialogInfoView);
    }

    static QIcon getHideTitlesIcon()
    {
        return QApplication::style()->standardIcon(QStyle::SP_FileDialogContentsView);
    }

    static QIcon getZoomInIcon()
    {
        return QApplication::style()->standardIcon(QStyle::SP_ArrowUp);
    }

    static QIcon getZoomOutIcon()
    {
        return QApplication::style()->standardIcon(QStyle::SP_ArrowDown);
    }

    static QIcon getRomDefaultIcon()
    {
        return QApplication::style()->standardIcon(QStyle::SP_FileIcon);
    }

    // Region flag placeholders - will be replaced with actual flag files later
    static QIcon getUSAFlagIcon()
    {
        return QIcon(":/flags/usa.svg");
    }

    static QIcon getEuropeFlagIcon()
    {
        return QIcon(":/flags/europe.svg");
    }

    static QIcon getJapanFlagIcon()
    {
        return QIcon(":/flags/japan.svg");
    }

    static QIcon getAustraliaFlagIcon()
    {
        return QIcon(":/flags/australia.svg");
    }

    static QIcon getUnknownFlagIcon()
    {
        return QIcon(":/flags/unknown.svg");
    }
};

} // namespace QT_UI
