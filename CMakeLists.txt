cmake_minimum_required(VERSION 3.18)
project(CCMagicPocket VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Qml Quick)

qt_standard_project_setup()

# dependency
add_subdirectory(modules/CCQuickStartup)
# third party
add_subdirectory(3rdparty)
# source
add_subdirectory(src)
add_subdirectory(examples)
