cmake_minimum_required(VERSION 3.16)

project(TreeTableModel VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 QUIET COMPONENTS Core Widgets)

if(Qt6_FOUND)
    set(QT_VERSION_MAJOR 6)
    set(QT_VERSION ${Qt6_VERSION})
    message(STATUS "Found Qt6: ${QT_VERSION}")
else()
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
    set(QT_VERSION_MAJOR 5)
    set(QT_VERSION ${Qt5_VERSION})
    message(STATUS "Found Qt5: ${QT_VERSION}")
endif()

# Определяем версию Qt для отображения
message(STATUS "Using Qt${QT_VERSION_MAJOR}: ${Qt${QT_VERSION_MAJOR}_VERSION}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/exe)
foreach(CFG DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CFG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endforeach()

set(SOURCES
    main.cpp
    MainWindow.cpp
    ValuesModel.cpp
    FormattingProxyModel.cpp
)

set(HEADERS
    MainWindow.h
    ValuesModel.h
    FormattingProxyModel.h
)

set(FORMS
    MainWindow.ui
)

add_executable(TreeTableModel
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
)

target_include_directories(TreeTableModel PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(TreeTableModel PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)