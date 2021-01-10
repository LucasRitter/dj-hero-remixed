# CMake Metadata
cmake_minimum_required(VERSION 3.17)
project(dj_hero_remixed)
add_definitions(-DCMAKE_GENERATOR-PLATFORM=x64)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message("${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# Project Settings
set(USE_DISCORD 1)

set(GAME_NAME "DJ Hero Remixed")
set(GAME_NAME_SHORT "DJR")
set(GAME_VERSION 1)

# C++ Version 20
set(CMAKE_CXX_STANDARD 20)

# Windows definition
add_definitions(-DWIN32=1)
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")

# Game Name Constants for FSGCore
add_definitions(-DGAME_NAME="${GAME_NAME}" -DGAME_NAME_SHORT="${GAME_NAME_SHORT}" -DGAME_VERSION=${GAME_VERSION})

# FSGCore Sub-project
add_subdirectory(FSGCore)
include_directories(FSGCore/Include)

# Discord GameSDK
if (USE_DISCORD AND MSVC)
        add_definitions(-DDISCORD_APPID=${793510916905107456})
        include_directories("${CMAKE_SOURCE_DIR}/Libraries/DiscordGameSDK/Include/")
        file(GLOB DISCORD_SOURCE ${CMAKE_SOURCE_DIR} "Libraries/DiscordGameSDK/Include/**/*.cpp")
endif()

# Includes
include_directories(DJ)

# Files
file(GLOB DJROOT ${CMAKE_SOURCE_DIR} "DJ/*.cpp")
file(GLOB_RECURSE DJ ${CMAKE_SOURCE_DIR} "DJ/**/*.cpp")

# Building
add_executable(dj_hero_remixed DJ/main.cpp ${DJROOT} ${DJ} ${DISCORD_SOURCE})

# Discord GameSDK Target (Win32 x86_64 only right now)
if (USE_DISCORD AND MSVC)
        find_library(
                DISCORD_LIBRARY
                NAMES discord_game_sdk
                PATHS ${CMAKE_SOURCE_DIR}/Libraries/DiscordGameSDK/Lib/x86_64
        )
        target_sources(dj_hero_remixed PUBLIC "${CMAKE_SOURCE_DIR}/Libraries/DiscordGameSDK/Include/Discord")
        target_link_directories(dj_hero_remixed PUBLIC ${DISCORD_LIBRARY})
        target_link_libraries(dj_hero_remixed PUBLIC ${DISCORD_LIBRARY}.lib)

        add_custom_command(TARGET dj_hero_remixed POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${DISCORD_LIBRARY}" "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/"
        )
endif()

# FSGCore Linking
target_link_libraries(dj_hero_remixed PUBLIC fsgcore)

# Output File Name
set_target_properties(dj_hero_remixed PROPERTIES OUTPUT_NAME "DJHeroRemixed")



