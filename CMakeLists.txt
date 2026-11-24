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
    src/Wynik.cpp
)
target_include_directories(saper PRIVATE include)

if(MSVC)
    target_compile_options(saper PRIVATE /W4 /utf-8)
else()
    target_compile_options(saper PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(test_logika
        tests/test_logika.cpp
        src/Plansza.cpp
        src/TrybGry.cpp
        src/Ustawienia.cpp
        src/Wynik.cpp
    )
    target_include_directories(test_logika PRIVATE include)
    if(MSVC)
        target_compile_options(test_logika PRIVATE /W4 /utf-8 /UNDEBUG)
    else()
        target_compile_options(test_logika PRIVATE -Wall -Wextra -Wpedantic -UNDEBUG)
    endif()
    add_test(NAME logika_gry COMMAND test_logika)
endif()
