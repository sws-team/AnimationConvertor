cmake_minimum_required(VERSION 3.5)

set(NAME AnimationConvertor)
project(${NAME} LANGUAGES CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(${NAME}
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    res.qrc
    )

target_link_libraries(${NAME} PRIVATE Qt5::Widgets)
