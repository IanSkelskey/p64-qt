cmake_minimum_required(VERSION 3.16)

project(QtUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Remove AUTOUIC since we're not using UI files anymore
# set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg OpenGL OpenGLWidgets Network Sql)

# Define source folders with their respective files
set(QT_UI_MAIN_SOURCES
    Source/main.cpp
    Source/UI/mainwindow.cpp
    Source/UI/mainwindow.h
    Source/UIAbstractionLayer.cpp
    Source/UIAbstractionLayer.h
)

set(QT_UI_ROMBROWSER_SOURCES
    Source/UI/RomBrowser/RomListModel.h
    Source/UI/RomBrowser/RomListModel.cpp
    Source/UI/RomBrowser/RomBrowserWidget.h
    Source/UI/RomBrowser/RomBrowserWidget.cpp
)

set(QT_UI_CORE_SOURCES
    Source/Core/RomInfoProvider.h
    Source/Core/RomInfoProvider.cpp
    Source/Core/RomParser.h
    Source/Core/RomParser.cpp
    Source/Core/DatabaseManager.h
    Source/Core/DatabaseManager.cpp
    Source/Core/SettingsManager.h
    Source/Core/SettingsManager.cpp
)

set(QT_UI_EMULATION_SOURCES
    Source/UI/Emulation/EmulationViewport.h
    Source/UI/Emulation/EmulationViewport.cpp
)

set(QT_UI_SETTINGS_SOURCES
    Source/UI/Settings/ConfigDialog.h
    Source/UI/Settings/ConfigDialog.cpp
)

set(QT_UI_ABOUT_SOURCES
    Source/UI/About/AboutDialog.h
    Source/UI/About/AboutDialog.cpp
)

set(QT_UI_TOOLS_SOURCES
    Source/UI/Tools/CoverDownloader.h
    Source/UI/Tools/CoverDownloader.cpp
)

set(QT_UI_ENHANCEMENT_SOURCES
    Source/UI/Enhancement/EnhancementUI.h
    Source/UI/Enhancement/EnhancementUI.cpp
)

# Include resources - Update the path to the correct location
set(QT_UI_RESOURCES
    Source/UI/Resources/resources.qrc
)

# Remove UI file from the build
set(UI_FILES
    # mainwindow.ui - Remove this line
    # ...other UI files...
)

# Add new theme files
set(THEME_SOURCES
    Source/UI/Theme/ThemeManager.cpp
    Source/UI/Theme/ThemeManager.h
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
    Qt${QT_VERSION_MAJOR}::Sql
)

# Set include directories
target_include_directories(QtUI PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Copy the database file to the build directory
add_custom_command(
    TARGET QtUI POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/database.sqlite
        $<TARGET_FILE_DIR:QtUI>/database.sqlite
    COMMENT "Copying database.sqlite to build directory"
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

# Also install the database file
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/database.sqlite
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(QtUI)
