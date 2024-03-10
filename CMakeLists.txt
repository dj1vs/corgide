cmake_minimum_required(VERSION 3.5)

project(corgide VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/app/ui")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

find_package(LibXml2 REQUIRED)

file(GLOB_RECURSE app_inc "${CMAKE_CURRENT_SOURCE_DIR}/app/cpp/include/*.hpp")
file(GLOB_RECURSE app_src "${CMAKE_CURRENT_SOURCE_DIR}/app/cpp/src/*.cpp")
file(GLOB_RECURSE app_ui  "${CMAKE_CURRENT_SOURCE_DIR}/app/cpp/*.ui")

file(GLOB_RECURSE tp_src "${CMAKE_CURRENT_SOURCE_DIR}/app/third-party/*.cpp")

set(PROJECT_SOURCES
        ${app_inc}
        ${app_src}
        ${app_ui}

        ${tp_src}
)


add_executable(corgide
    ${PROJECT_SOURCES}
    )

target_include_directories(${PROJECT_NAME} PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/app/cpp/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/ui"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/third-party/QSourceHighlite"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/third-party/QLightTerminal"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/third-party/qhtmlparser"
)

target_link_libraries(corgide PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network ${LIBXML2_LIBRARIES})
# Include libxml2 headers
target_include_directories(corgide PRIVATE ${LIBXML2_INCLUDE_DIR})

# Link against libxml2 library
set_target_properties(corgide PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS corgide
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(corgide)
endif()
