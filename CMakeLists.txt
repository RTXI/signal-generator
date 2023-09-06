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
set(RTXI_PACKAGE_PATH "/usr/local/" CACHE PATH "Path hint to RTXI package information")
list(APPEND CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

# ---- find libraries ----
find_package(rtxi REQUIRED HINTS ${RTXI_PACKAGE_PATH})
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Boost REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_library(
    signal_generator MODULE
    signal-generator.cpp
    signal-generator.h
)

target_link_libraries(signal_generator PUBLIC 
    rtxi::rtxi rtxi::rtdsp rtxi::rtgen rtxi::xfifo Qt5::Core Qt5::Gui Qt5::Widgets 
    Boost::headers backtrace dl 
)

# We need to tell cmake to use the c++ version used to compile the dependent library or else...
get_target_property(REQUIRED_COMPILE_FEATURE rtxi::rtxi INTERFACE_COMPILE_FEATURES)
target_compile_features(signal_generator PRIVATE ${REQUIRED_COMPILE_FEATURE})

target_compile_definitions(signal_generator PRIVATE BOOST_STACKTRACE_USE_ADDR2LINE)
target_compile_definitions(signal_generator PRIVATE BOOST_STACKTRACE_USE_BACKTRACE)
