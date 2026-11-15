cmake_minimum_required(VERSION 3.28)
project(RoboGame CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CORE_SOURCES
        src/abilities/AreaAttack.cpp
        src/abilities/RangedAttack.cpp
        src/abilities/Healing.cpp
        src/abilities/Teleportation.cpp
        src/statuses/Slowness.cpp
        src/statuses/Overload.cpp
        src/statuses/Burning.cpp
        src/statuses/Shield.cpp
        src/core/Point.cpp
        src/core/Cell.cpp
        src/core/Field.cpp
        src/entities/Robot.cpp
        src/entities/RobotFactory.cpp
        src/inventory/IInventory.h
        src/game/StartingField.cpp
)

add_library(RoboGameCore STATIC ${CORE_SOURCES})

target_include_directories(RoboGameCore PUBLIC
        src/inventory
        src/statuses
        src/abilities
        src/core
        src/entities
        src/game
)

include(FetchContent)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build dependencies as static libraries")
set(SFML_BUILD_AUDIO OFF CACHE BOOL "Build SFML audio" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "Build SFML networking" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "Build SFML examples" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "Build SFML tests" FORCE)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SFML)

add_executable(RoboGame src/main.cpp src/game/GameManager.cpp
    src/graphics/GraphicsRenderer.cpp src/graphics/SfmlGameView.cpp)
target_include_directories(RoboGame PRIVATE src/graphics)
target_link_libraries(RoboGame PRIVATE RoboGameCore SFML::Graphics)

if (WIN32)
    set_target_properties(RoboGame PROPERTIES WIN32_EXECUTABLE TRUE)
    target_link_libraries(RoboGame PRIVATE SFML::Main)
endif()
