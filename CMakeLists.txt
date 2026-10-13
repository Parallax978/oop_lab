cmake_minimum_required(VERSION 3.20)
project(RoboGame CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(SOURCES
        src/main.cpp
        src/core/Point.cpp
        src/core/Cell.cpp
        src/core/Field.cpp
        src/entities/Robot.cpp
        src/entities/RobotFactory.cpp
        src/game/ConsoleRenderer.cpp
        src/game/GameManager.cpp
)

add_executable(RoboGame ${SOURCES})

target_include_directories(RoboGame PRIVATE
        src/core
        src/entities
        src/game
)