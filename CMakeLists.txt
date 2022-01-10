# Copyright 2022 Haofan Zheng
# See accompanying file LICENSE

cmake_minimum_required(VERSION 3.14)

project(SimpleUtf VERSION 0.0.1 LANGUAGES CXX)

add_subdirectory(include)

OPTION(SIMPLEUTF_TEST "Option to build SimpleUtf test executable." OFF)
#SET(SIMPLEUTF_TEST ON CACHE BOOL "Option to build SimpleUtf test executable." FORCE)

set(ENV{SIMPLEUTF_HOME} ${CMAKE_CURRENT_LIST_DIR})
set(SIMPLEUTF_HOME ${CMAKE_CURRENT_LIST_DIR})

set(ENV{SIMPLEUTF_INCLUDE} $ENV{SIMPLEUTF_HOME}/include)
set(SIMPLEUTF_INCLUDE ${SIMPLEUTF_HOME}/include)

add_library(SimpleUtf INTERFACE)

target_include_directories(SimpleUtf INTERFACE include)

if(${SIMPLEUTF_TEST})
	enable_testing()
	add_subdirectory(test)
endif(${SIMPLEUTF_TEST})
