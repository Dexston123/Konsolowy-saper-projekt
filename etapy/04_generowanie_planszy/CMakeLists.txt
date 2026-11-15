cmake_minimum_required(VERSION 3.16)
project(Saper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(saper
    src/main.cpp
    src/Ustawienia.cpp
    src/Plansza.cpp
    src/TrybGry.cpp
    src/Gra.cpp
)
target_include_directories(saper PRIVATE include)

if(MSVC)
    target_compile_options(saper PRIVATE /W4 /utf-8)
else()
    target_compile_options(saper PRIVATE -Wall -Wextra -Wpedantic)
endif()
