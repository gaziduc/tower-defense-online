cmake_minimum_required(VERSION 3.28)
project(tower-defense-online)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME}
        resources.rc
        src/Main.cpp
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
        src/framerate/SDL2_framerate.h
        src/AnimationEntity.cpp
        src/AnimationEntity.h
        src/Player.cpp
        src/Player.h
        src/RenderUtils.cpp
        src/RenderUtils.h
        src/IPv4.cpp
        src/IPv4.h
        src/Colors.cpp
        src/Colors.h
        src/game/Game.cpp
        src/game/Game.h
)

add_executable(${PROJECT_NAME}-server
        resources.rc
        src/server/Main.cpp
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
        src/framerate/SDL2_framerate.h
        src/AnimationEntity.cpp
        src/AnimationEntity.h
        src/Player.cpp
        src/Player.h
        src/RenderUtils.cpp
        src/RenderUtils.h
        src/IPv4.cpp
        src/IPv4.h
        src/Colors.cpp
        src/Colors.h
        src/game/Game.cpp
        src/game/Game.h
        src/server/ServerConstants.h
)

set(CMAKE_PREFIX_PATH
        ${CMAKE_SOURCE_DIR}/SDL2-2.30.2/cmake
        ${CMAKE_SOURCE_DIR}/SDL2_image-2.8.2/cmake
        ${CMAKE_SOURCE_DIR}/SDL2_mixer-2.8.0/cmake
        ${CMAKE_SOURCE_DIR}/SDL2_net-2.2.0/cmake
        ${CMAKE_SOURCE_DIR}/SDL2_ttf-2.22.0/cmake
)

# To build tower-defense-online-server, uncomment to following line (but let it commented for tower-defense-online):
set(SDL2_NO_MWINDOWS true)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_net REQUIRED)
find_package(SDL2_ttf REQUIRED)

if (TARGET SDL2::SDL2main)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
    target_link_libraries(${PROJECT_NAME}-server PRIVATE SDL2::SDL2main)
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        SDL2_net::SDL2_net
        SDL2_ttf::SDL2_ttf
)

target_link_libraries(${PROJECT_NAME}-server PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        SDL2_net::SDL2_net
        SDL2_ttf::SDL2_ttf
)