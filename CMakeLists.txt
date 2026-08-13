cmake_minimum_required(VERSION 3.24)

project(trdr LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(trdr_project INTERFACE)

target_include_directories(
    trdr_project
    INTERFACE 
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_features(trdr_project INTERFACE cxx_std_20)

target_compile_options(
    trdr_project
    INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -fno-exceptions
)

add_executable(
    trdr
    src/trdr_main.cpp
)

target_link_libraries(
    trdr
    PRIVATE
        trdr_project
)

set_target_properties(
    trdr
    PROPERTIES
        OUTPUT_NAME "trdr"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src"
)

add_executable(
    trdr_terminal
    tools/trdr_terminal.cpp
)

target_link_libraries(
    trdr_terminal
    PRIVATE
        trdr_project
)

set_target_properties(
    trdr_terminal
    PROPERTIES
        OUTPUT_NAME "trdr_terminal"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
)

add_executable(
    liquidity_bot
    tools/liquidity_bot.cpp
)

target_link_libraries(
    liquidity_bot
    PRIVATE
        trdr_project
)

set_target_properties(
    liquidity_bot
    PROPERTIES
        OUTPUT_NAME "liquidity_bot"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
)



