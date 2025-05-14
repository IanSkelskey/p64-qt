#pragma once

#include <QObject>
#include <QApplication>
#include <QString>
#include <QStyle>
#include <QPalette>
#include <QStyleFactory>
#include <QIcon>

namespace QT_UI {

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    enum Theme {
        SystemTheme,
        LightTheme,
        DarkTheme
    };

    static ThemeManager& instance();

    Theme currentTheme() const { return m_currentTheme; }
    bool setTheme(Theme theme);
    bool isDarkMode() const;
    
    // New methods for icon theme management
    void updateIconTheme();
    static bool isCurrentlyDarkTheme();

signals:
    void themeChanged(Theme newTheme);

private:
    explicit ThemeManager(QObject* parent = nullptr);
    void applyLightTheme();
    void applyDarkTheme();
    void applySystemTheme();
    bool isSystemInDarkMode() const;
    void saveThemeSettings(Theme theme);

    Theme m_currentTheme;
    QPalette m_lightPalette;
    QPalette m_darkPalette;
};

} // namespace QT_UI
