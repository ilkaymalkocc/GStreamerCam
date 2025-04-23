cmake_minimum_required(VERSION 3.16)
project(Gstreamer_MSVC)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(GSTREAMER_ROOT "C:/Program Files/gstreamer/1.0/msvc_x86_64")
include_directories(
    "${GSTREAMER_ROOT}/include/gstreamer-1.0"
    "${GSTREAMER_ROOT}/include/glib-2.0"
    "${GSTREAMER_ROOT}/lib/glib-2.0/include"
)

link_directories("${GSTREAMER_ROOT}/lib")

set(SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.ui
)

set(HEADERS
    mainwindow.h
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(${PROJECT_NAME}
    Qt6::Widgets
    gstreamer-1.0.lib
    gstapp-1.0.lib
    gstvideo-1.0.lib
    gobject-2.0.lib
    glib-2.0.lib
    ws2_32.lib
    ole32.lib
    uuid.lib
    winmm.lib
)
