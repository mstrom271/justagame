cmake_minimum_required(VERSION 3.24.2)

project($ENV{PROJECT_NAME} VERSION $ENV{PROJECT_VERSION} LANGUAGES CXX)

include("$ENV{Qt6_DIR}/lib/cmake/Qt6/qt.toolchain.cmake")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file (GLOB PROJECT_SOURCES
    src/*.h
    src/*.cpp
    rcc/rcc.qrc
)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS OpenGL)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS OpenGLWidgets)

qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::OpenGLWidgets)

# pass variables from cmake to our code
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h @ONLY)
if (ANDROID)

    set_target_properties(${PROJECT_NAME} PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android)

    string(REPLACE "." ".a" ANDROID_PROJECT_NAME ${PROJECT_NAME})
    # configure_file(${CMAKE_CURRENT_SOURCE_DIR}/build/android/android/AndroidManifest.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/build/android/android/AndroidManifest.xml @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml @ONLY)
endif()

qt_finalize_executable(${PROJECT_NAME})
