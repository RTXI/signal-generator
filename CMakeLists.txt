cmake_minimum_required(VERSION 3.14)

project(
    signal-generator
    VERSION 0.1.0
    DESCRIPTION "Generate simple waveforms"
    HOMEPAGE_URL "https://rtxi.org/"
    LANGUAGES CXX
)

# These lines help with third-party tooling integration
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(RTXI_CMAKE_SCRIPTS "" CACHE PATH "In development mode, RTXI uses this to expose depenendencies")
set(CMAKE_PREFIX_PATH "${RTXI_CMAKE_SCRIPTS}")
set(RTXI_PACKAGE_PATH "/usr/local/" CACHE PATH "Path hint to RTXI package information")
set(CMAKE_INSTALL_RPATH "${RTXI_PACKAGE_PATH}/lib/rtxi")
list(APPEND CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

# ---- find libraries ----
find_package(rtxi REQUIRED HINTS ${RTXI_PACKAGE_PATH})
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets HINTS ${RTXI_CMAKE_SCRIPTS})
find_package(fmt REQUIRED)
find_package(Boost REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_library(
    signal-generator MODULE
    widget.cpp
    widget.hpp
)

target_link_libraries(signal-generator PUBLIC 
    rtxi::rtxi rtxi::rtdsp rtxi::rtgen rtxi::xfifo Qt5::Core Qt5::Gui Qt5::Widgets 
    Boost::headers dl fmt::fmt
)

# We need to tell cmake to use the c++ version used to compile the dependent library or else...
get_target_property(REQUIRED_COMPILE_FEATURE rtxi::rtxi INTERFACE_COMPILE_FEATURES)
target_compile_features(signal-generator PRIVATE ${REQUIRED_COMPILE_FEATURE})

install(
    TARGETS signal-generator
    DESTINATION ${RTXI_PACKAGE_PATH}/bin/rtxi_modules
)

