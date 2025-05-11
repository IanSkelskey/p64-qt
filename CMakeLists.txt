cmake_minimum_required(VERSION 3.16)

project(QtUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Remove AUTOUIC since we're not using UI files anymore
# set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg OpenGL OpenGLWidgets Network)

# Define source folders with their respective files
set(QT_UI_MAIN_SOURCES
    main.cpp
    UI/mainwindow.cpp
    UI/mainwindow.h
    UIAbstractionLayer.cpp
    UIAbstractionLayer.h
)

set(QT_UI_ROMBROWSER_SOURCES
    UI/RomBrowser/RomListModel.h
    UI/RomBrowser/RomListModel.cpp
    UI/RomBrowser/RomBrowserWidget.h
    UI/RomBrowser/RomBrowserWidget.cpp
)

set(QT_UI_CORE_SOURCES
    Core/RomInfoProvider.h
    Core/RomInfoProvider.cpp
)

set(QT_UI_EMULATION_SOURCES
    UI/Emulation/EmulationViewport.h
    UI/Emulation/EmulationViewport.cpp
)

set(QT_UI_SETTINGS_SOURCES
    UI/Settings/SettingsDialog.h
    UI/Settings/SettingsDialog.cpp
    UI/Settings/ConfigDialog.h
    UI/Settings/ConfigDialog.cpp
)

set(QT_UI_ABOUT_SOURCES
    UI/About/AboutDialog.h
    UI/About/AboutDialog.cpp
)

set(QT_UI_TOOLS_SOURCES
    UI/Tools/CoverDownloader.h
    UI/Tools/CoverDownloader.cpp
)

set(QT_UI_ENHANCEMENT_SOURCES
    UI/Enhancement/EnhancementUI.h
    UI/Enhancement/EnhancementUI.cpp
)

# Include resources - Update the path to the correct location
set(QT_UI_RESOURCES
    UI/resources.qrc
)

# Remove UI file from the build
set(UI_FILES
    # mainwindow.ui - Remove this line
    # ...other UI files...
)

# Add new theme files
set(THEME_SOURCES
    UI/Theme/ThemeManager.cpp
    UI/Theme/ThemeManager.h
)

# Combine all sources
set(PROJECT_SOURCES
    ${QT_UI_MAIN_SOURCES}
    ${QT_UI_ROMBROWSER_SOURCES}
    ${QT_UI_CORE_SOURCES}
    ${QT_UI_EMULATION_SOURCES}
    ${QT_UI_ENHANCEMENT_SOURCES}
    ${QT_UI_SETTINGS_SOURCES}
    ${QT_UI_ABOUT_SOURCES}
    ${QT_UI_TOOLS_SOURCES}
    ${QT_UI_RESOURCES}
    ${THEME_SOURCES}
)

qt_add_executable(QtUI
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(QtUI PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    Qt${QT_VERSION_MAJOR}::Network
)

# Set include directories
target_include_directories(QtUI PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(QtUI PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS QtUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(QtUI)
