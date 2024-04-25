cmake_minimum_required(VERSION 3.28)
project(tower-defense-online)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} src/main.cpp
        src/ErrorUtils.cpp
        src/ErrorUtils.h
        src/Animation.cpp
        src/Animation.h
        src/LogUtils.cpp
        src/LogUtils.h
        src/Events.cpp
        src/Events.h
        src/Entity.cpp
        src/Entity.h
        src/Constants.cpp
        src/Constants.h
        src/framerate/SDL2_framerate.cpp
        src/framerate/SDL2_framerate.h)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(SDL2_PATH "SDL2-2.30.2/x86_64-w64-mingw32")
set(SDL2_IMAGE_PATH "SDL2_image-2.8.2/x86_64-w64-mingw32")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY})
target_link_libraries(${PROJECT_NAME} -static)