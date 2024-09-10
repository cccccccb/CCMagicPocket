cmake_minimum_required(VERSION 3.16)
project(toklive VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

add_subdirectory(preload)
add_subdirectory(entity)

include(../../src/controls/cmake/MPBuildPackage.cmake)
mp_build_package(CCMagicPocketExample
    NAME MagicPocketExample
    ICON ${CMAKE_CURRENT_SOURCE_DIR}/entity/res/live.png
    PRELOAD TokLive-DesktopPreload
    ENTITY TokLive-DesktopEntity
    VERSION "v0.0.1"
    DESCRIPTION "The Example Component"
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/mpk
)
