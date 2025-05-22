# Core library CMakeLists.txt

# Define the Core library source files
set(CORE_SOURCES
    RomInfoProvider.h
    RomInfoProvider.cpp
    RomParser.h
    RomParser.cpp
    DatabaseManager.h
    DatabaseManager.cpp
    Settings/SettingsManager.h
    Settings/SettingsManager.cpp
    Settings/ApplicationSettings.h
    Settings/ApplicationSettings.cpp
    Settings/AudioSettings.h
    Settings/AudioSettings.cpp
    Settings/DebugSettings.h
    Settings/DebugSettings.cpp
    Settings/EmulationSettings.h
    Settings/EmulationSettings.cpp
    Settings/GraphicsSettings.h
    Settings/GraphicsSettings.cpp
    Settings/InputSettings.h
    Settings/InputSettings.cpp
    Settings/PluginSettings.h
    Settings/PluginSettings.cpp
    Settings/RomBrowserSettings.h
    Settings/RomBrowserSettings.cpp
    Settings/SaveSettings.h
    Settings/SaveSettings.cpp
)

# Create a static library from the sources
add_library(CoreLib STATIC ${CORE_SOURCES})

# Link against Qt dependencies
target_link_libraries(CoreLib PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Widgets  # Added Widgets for QApplication
)

# Set include directories for this library
target_include_directories(CoreLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..  # This makes #include <Core/...> work
)
