# UI library CMakeLists.txt

# Define UI library sources by component
set(UI_MAIN_SOURCES
    mainwindow.cpp
    mainwindow.h
)

set(UI_ROMBROWSER_SOURCES
    RomBrowser/RomListModel.h
    RomBrowser/RomListModel.cpp
    RomBrowser/RomBrowserWidget.h
    RomBrowser/RomBrowserWidget.cpp
)

set(UI_EMULATION_SOURCES
    Emulation/EmulationViewport.h
    Emulation/EmulationViewport.cpp
)

set(UI_SETTINGS_SOURCES
    Settings/ConfigDialog.h
    Settings/ConfigDialog.cpp
    Settings/BaseSettingsPage.h
    Settings/DirectoriesSettingsPage.h
    Settings/DirectoriesSettingsPage.cpp
    Settings/GeneralSettingsPage.h
    Settings/GeneralSettingsPage.cpp
    Settings/RomBrowserSettingsPage.h
    Settings/RomBrowserSettingsPage.cpp
    Settings/GraphicsSettingsDialog.h
    Settings/GraphicsSettingsDialog.cpp
)

set(UI_ABOUT_SOURCES
    About/AboutDialog.h
    About/AboutDialog.cpp
)

set(UI_TOOLS_SOURCES
    Tools/CoverDownloader.h
    Tools/CoverDownloader.cpp
)

set(UI_ENHANCEMENT_SOURCES
    Enhancement/EnhancementUI.h
    Enhancement/EnhancementUI.cpp
)

set(UI_THEME_SOURCES
    Theme/ThemeManager.cpp
    Theme/ThemeManager.h
    Theme/IconHelper.h
)

# Combine all UI sources
set(UI_SOURCES
    ${UI_MAIN_SOURCES}
    ${UI_ROMBROWSER_SOURCES}
    ${UI_EMULATION_SOURCES}
    ${UI_SETTINGS_SOURCES}
    ${UI_ABOUT_SOURCES}
    ${UI_TOOLS_SOURCES}
    ${UI_ENHANCEMENT_SOURCES}
    ${UI_THEME_SOURCES}
)

# Create a static library from the sources
add_library(UILib STATIC ${UI_SOURCES})

# Link against Qt dependencies and our Core library
target_link_libraries(UILib PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    Qt${QT_VERSION_MAJOR}::Network
    CoreLib
)

# Set include directories for this library
target_include_directories(UILib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..  # This makes #include <UI/...> work
)
