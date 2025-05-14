#include "ThemeManager.h"
#include "../../Core/SettingsManager.h"
#include <QSettings>
#include <QFile>
#include <QStyleFactory>
#include <QApplication>
#include <QWidget> // Add this include for QWidget class definition
#include <QStyle> // Add this include for style methods

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

namespace QT_UI {

ThemeManager& ThemeManager::instance()
{
    static ThemeManager instance;
    return instance;
}

ThemeManager::ThemeManager(QObject* parent)
    : QObject(parent)
    , m_currentTheme(SystemTheme)
{
    // Initialize light palette (default Qt)
    m_lightPalette = QApplication::style()->standardPalette();
    
    // Initialize dark palette
    m_darkPalette = m_lightPalette;
    
    // Dark palette colors
    m_darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    m_darkPalette.setColor(QPalette::WindowText, Qt::white);
    m_darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
    m_darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
    m_darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
    m_darkPalette.setColor(QPalette::ToolTipBase, QColor(53, 53, 53));
    m_darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    m_darkPalette.setColor(QPalette::Text, Qt::white);
    m_darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    m_darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    m_darkPalette.setColor(QPalette::ButtonText, Qt::white);
    m_darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
    m_darkPalette.setColor(QPalette::BrightText, Qt::red);
    m_darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    m_darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    m_darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    m_darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
    
    // Load saved theme
    QSettings settings("Project64", "QtUI");
    int theme = settings.value("Theme/CurrentTheme", static_cast<int>(SystemTheme)).toInt();
    m_currentTheme = static_cast<Theme>(theme);

    // Apply the current theme on startup
    switch (m_currentTheme) {
    case LightTheme:
        applyLightTheme();
        break;
    case DarkTheme:
        applyDarkTheme();
        break;
    case SystemTheme:
    default:
        applySystemTheme();
        break;
    }
}

bool ThemeManager::setTheme(Theme theme)
{
    if (m_currentTheme == theme) {
        return false; // Theme is already set
    }
    
    m_currentTheme = theme;
    
    switch (theme) {
    case LightTheme:
        applyLightTheme();
        break;
    case DarkTheme:
        applyDarkTheme();
        break;
    case SystemTheme:
    default:
        applySystemTheme();
        break;
    }
    
    saveThemeSettings(theme);
    emit themeChanged(theme);
    return true;
}

bool ThemeManager::isDarkMode() const
{
    if (m_currentTheme == DarkTheme) {
        return true;
    }
    else if (m_currentTheme == LightTheme) {
        return false;
    }
    else { // SystemTheme
        return isSystemInDarkMode();
    }
}

void ThemeManager::applyLightTheme()
{
    QApplication* app = qApp;
    app->setStyle(QStyleFactory::create("Fusion"));
    app->setPalette(m_lightPalette);
    
    // Apply light style sheets
    QFile styleFile(":/styles/light.qss");
    if (styleFile.exists() && styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString style = QLatin1String(styleFile.readAll());
        app->setStyleSheet(style);
        styleFile.close();
    } else {
        app->setStyleSheet("");
    }
}

void ThemeManager::applyDarkTheme()
{
    QApplication* app = qApp;
    app->setStyle(QStyleFactory::create("Fusion"));
    app->setPalette(m_darkPalette);
    
    // Apply dark style sheets
    QFile styleFile(":/styles/dark.qss");
    if (styleFile.exists() && styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString style = QLatin1String(styleFile.readAll());
        app->setStyleSheet(style);
        styleFile.close();
    } else {
        // Basic dark theme stylesheet if the file is not available
        app->setStyleSheet(R"(
            QToolTip {
                color: #ffffff;
                background-color: #2a2a2a;
                border: 1px solid #767676;
            }
            
            QMenu {
                background-color: #2d2d30;
                color: white;
            }
            
            QMenu::item:selected {
                background-color: #3e3e42;
            }
            
            QMenuBar {
                background-color: #2d2d30;
                color: white;
            }
            
            QMenuBar::item:selected {
                background-color: #3e3e42;
            }
        )");
    }
}

void ThemeManager::applySystemTheme()
{
    QApplication* app = qApp;

    // Check the system theme and apply the corresponding theme
    if (isSystemInDarkMode()) {
        app->setStyle(QStyleFactory::create("Fusion"));
        app->setPalette(m_darkPalette);
        
        // Apply dark style sheets
        QFile styleFile(":/styles/dark.qss");
        if (styleFile.exists() && styleFile.open(QFile::ReadOnly | QFile::Text)) {
            QString style = QLatin1String(styleFile.readAll());
            app->setStyleSheet(style);
            styleFile.close();
        }
    } else {
        app->setStyle(QStyleFactory::create("Fusion"));
        app->setPalette(m_lightPalette);
        
        // Apply light style sheets
        QFile styleFile(":/styles/light.qss");
        if (styleFile.exists() && styleFile.open(QFile::ReadOnly | QFile::Text)) {
            QString style = QLatin1String(styleFile.readAll());
            app->setStyleSheet(style);
            styleFile.close();
        } else {
            app->setStyleSheet("");
        }
    }
}

bool ThemeManager::isSystemInDarkMode() const
{
#ifdef Q_OS_WIN
    // Windows 10+ dark mode detection
    HKEY hKey;
    DWORD value = 0;
    DWORD size = sizeof(value);
    
    // Check if Windows is using dark mode
    if (RegOpenKeyEx(HKEY_CURRENT_USER, 
                    TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"), 
                    0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueEx(hKey, TEXT("AppsUseLightTheme"), NULL, NULL, 
                          reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return value == 0; // 0 means dark mode is enabled
        }
        RegCloseKey(hKey);
    }
#endif

#ifdef Q_OS_MACOS
    // macOS dark mode detection
    // This is a simplified approach; for macOS, you might want to use native APIs
    QSettings settings(QSettings::UserScope, "Apple", "General");
    return settings.value("AppleInterfaceStyle", "").toString() == "Dark";
#endif

    // Default to light mode on other platforms
    return false;
}

void ThemeManager::saveThemeSettings(Theme theme)
{
    // Use centralized SettingsManager instead of direct QSettings
    QT_UI::SettingsManager::instance().setTheme(
        static_cast<QT_UI::SettingsManager::Theme>(theme));
}

} // namespace QT_UI
