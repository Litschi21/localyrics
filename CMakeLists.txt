cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME localyrics)
set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/resources/${PROJECT_NAME}.qrc")
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG 0.11.24
)

FetchContent_MakeAvailable(miniaudio)
find_package(Qt6 REQUIRED COMPONENTS Widgets Svg)
add_executable(${PROJECT_NAME}
  ${app_icon_resource_windows}
  src/main.cpp
  src/MainWindow.cpp
  src/MainWindow.ui
  src/AddSongDialog.cpp
  src/AddSongDialog.ui
  src/SongWidget.cpp
  src/SongWidget.ui
  src/SongBar.cpp
  src/SongBar.ui
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Wnull-dereference -Wformat=2 -Werror)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Svg miniaudio)
