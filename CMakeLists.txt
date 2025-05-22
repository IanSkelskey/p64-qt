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

# Include resources
set(QT_UI_RESOURCES
    Source/UI/Resources/resources.qrc
)

# Add the component subdirectories
add_subdirectory(Source/Core)
add_subdirectory(Source/UI)

# Define minimal main source
set(MAIN_SOURCES
    Source/main.cpp
    Source/UIAbstractionLayer.cpp
    Source/UIAbstractionLayer.h
)

qt_add_executable(QtUI
    MANUAL_FINALIZATION
    ${MAIN_SOURCES}
    ${QT_UI_RESOURCES}
)

target_link_libraries(QtUI PRIVATE 
    CoreLib
    UILib
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