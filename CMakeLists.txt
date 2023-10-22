cmake_minimum_required(VERSION 3.14)

project(MiniSpotify VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick Network Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick Network Gui Svg Widgets)

set(PROJECT_SOURCES
        main.cpp
        qml.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MiniSpotify
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET MiniSpotify APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MiniSpotify SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MiniSpotify
          ${PROJECT_SOURCES}
	  main.h
        )
    endif()
endif()

target_link_libraries(MiniSpotify
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
	  Qt${QT_VERSION_MAJOR}::Quick
	  Qt${QT_VERSION_MAJOR}::Network
	  Qt${QT_VERSION_MAJOR}::Gui
	  Qt${QT_VERSION_MAJOR}::Svg
	  Qt${QT_VERSION_MAJOR}::Widgets)



      # App related definitions
target_compile_definitions(MiniSpotify PRIVATE PROJECT_NAME="MiniSpotify")
target_compile_definitions(MiniSpotify PRIVATE PROJECT_VERSION="1.0")
target_compile_definitions(MiniSpotify PRIVATE APP_VERSION="v${PROJECT_VERSION}")
target_compile_definitions(MiniSpotify PRIVATE APP_NAME="${PROJECT_NAME}")
target_compile_definitions(MiniSpotify PRIVATE APP_ICON="${PROJECT_NAME}")
target_compile_definitions(MiniSpotify PRIVATE ORG_NAME="Qt Software Solution")
target_compile_definitions(MiniSpotify PRIVATE PKG_NAME="com.blogspot.dabreha")
target_compile_definitions(MiniSpotify PRIVATE BUILD_TYPE="${CMAKE_BUILD_TYPE}")


# spotify-qt-lib
set(LIB_QT_IMPL ON)
# Collect source files
add_subdirectory(src)

# Headers are in src/
target_include_directories(MiniSpotify PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_subdirectory(lib)
target_link_libraries(MiniSpotify PRIVATE spotify-qt-lib)


# Enable all compiler warnings under GCC
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(MiniSpotify PRIVATE -Wall -Wextra)
endif ()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.MiniSpotify)
endif()
set_target_properties(MiniSpotify PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS MiniSpotify
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(MiniSpotify)
    qt_finalize_executable(MiniSpotify)
endif()
