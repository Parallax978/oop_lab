cmake_minimum_required(VERSION 3.20)
project(RoboGame CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем предупреждения компилятора для чистоты кода
if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Исходные файлы проекта (по 1 классу на файл)
set(SOURCES
        Point.cpp
        Cell.cpp
        Robot.cpp
        RobotFactory.cpp
        Field.cpp
        ConsoleRenderer.cpp
        GameManager.cpp
        main.cpp
)

add_executable(RoboGame ${SOURCES})
target_include_directories(RoboGame PRIVATE src)