cmake_minimum_required(VERSION 2.8.4)
project(ImageViewer)

set ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" )
list( APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}" )

find_package(SDL2 COMPONENTS image ttf)

if ( NOT SDL_FOUND )
    message ( FATAL_ERROR "Can't find SDL which is required" )
else ()
    include_directories ( ${SDL_INCLUDE_DIR} ${SDLIMAGE_INCLUDE_DIR} ${SDLTTF_INCLUDE_DIR} )
    message ( "found SDL ${SDL_MAJOR}.${SDL_MINOR}.${SDL_MICRO} (${SDL_LIBRARY_DIRS}:${SDL_INCLUDE_DIR})" )
endif ()


set(SOURCE_FILES
    src/main.cpp
    src/SurfaceTexture.cpp
    src/SurfaceTexture.h
    src/SmartPointer.h
    src/SmartPointer.cpp
    src/Font.cpp
    src/Font.h
    )

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})


add_executable(ImageViewer ${SOURCE_FILES})
target_link_libraries(ImageViewer ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY} ${SDL_LIBRARY}  ${Boost_LIBRARIES})

