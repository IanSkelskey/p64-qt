#include "ThemeManager.h"
#include "IconHelper.h"
#include <QApplication>
#include <QStyle>

namespace QT_UI {

// This could be moved to ThemeManager.cpp if preferred
class IconThemeInitializer {
public:
    IconThemeInitializer() {
        // Initialize the icon theme on application startup
        ThemeManager::instance().updateIconTheme();
        
        // Connect to theme changes to update icon theme
        QObject::connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
                         [](ThemeManager::Theme) {
            ThemeManager::instance().updateIconTheme();
        });
    }
};

// Create a static instance to ensure initialization
static IconThemeInitializer iconThemeInit;

} // namespace QT_UI
