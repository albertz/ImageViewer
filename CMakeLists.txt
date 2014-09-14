cmake_minimum_required(VERSION 2.8.4)
project(ImageViewer)

set ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" )

find_package ( SDL2 COMPONENTS image)

if ( NOT SDL_FOUND )
    message ( FATAL_ERROR "Can't find SDL which is required" )
else ()
    include_directories ( ${SDL_INCLUDE_DIR} )
    message ( "found SDL ${SDL_MAJOR}.${SDL_MINOR}.${SDL_MICRO} (${SDL_LIBRARY_DIRS}:${SDL_INCLUDE_DIR})" )
endif ()


set(SOURCE_FILES src/main.cpp)
add_executable(ImageViewer ${SOURCE_FILES})

