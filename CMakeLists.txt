# © 2020 Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

include("${CMAKE_CURRENT_SOURCE_DIR}/jfc-cmake/jfclib.cmake")

option(JFC_BUILD_DEMO "Build demo" ON)
option(JFC_BUILD_DOCS "Build documentation" ON)
option(JFC_BUILD_TESTS "Build unit tests" ON)

add_subdirectory(thirdparty)

jfc_project(library
    NAME "gdktextrenderer"
    VERSION 0.0
    DESCRIPTION "3d text rendering library for the gdk project"
    C++_STANDARD 17
    C_STANDARD 90
    TYPE STATIC

    PUBLIC_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${gdkgraphics_INCLUDE_DIRECTORIES} 

    PRIVATE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include

    LIBRARIES
        ${gdkgraphics}

    DEPENDENCIES
        "gdkgraphics"

    SOURCE_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dynamic_text_renderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/static_text_renderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/text_map.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/text_renderer.cpp
)

#[[if (JFC_BUILD_DEMO)
add_subdirectory(demo)
endif()

if (JFC_BUILD_TESTS)
add_subdirectory(test)
endif()

if (JFC_BUILD_DOCS)
add_subdirectory(docs)
endif()]]

