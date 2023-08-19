cmake_minimum_required(VERSION 3.20)
project(Low-Latency-Trader)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CORE_DIR ${CMAKE_SOURCE_DIR}/core)
set(COMMON_INCLUDE_DIR ${CORE_DIR}/common)
set(COMMON_LIB_DIR ${COMMON_INCLUDE_DIR}/lib)

add_subdirectory(core)