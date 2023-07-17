cmake_minimum_required(VERSION 3.14)

project(
    signal_generator
    VERSION 0.1.0
    DESCRIPTION "Generate simple waveforms"
    HOMEPAGE_URL "https://rtxi.org/"
    LANGUAGES CXX
)

# These lines help with third-party tooling integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

# ---- find libraries ----
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(rtxi REQUIRED HINTS "../../../build/test")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_library(
    signal_generator SHARED
    signal-generator.cpp
    signal-generator.h
)

target_link_libraries(signal_generator PRIVATE
    rtxi::rtxi rtxi::rtdsp Qt5::Core Qt5::Gui Qt5::Widgets 
)

