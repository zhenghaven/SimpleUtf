# Copyright 2022 Haofan Zheng
# See accompanying file LICENSE

cmake_minimum_required(VERSION 3.14)

project(SimpleUtf_test VERSION 0.1 LANGUAGES CXX)

################################################################################
# Set compile options
################################################################################

if(MSVC)
	set(COMMON_OPTIONS /W3 /wd4996 /we4239 /we4002 /we4700 /we4305 /EHsc /MP)
	set(DEBUG_OPTIONS /MTd /Od /Zi /DDEBUG)
	set(RELEASE_OPTIONS /MT /Ox /Oi /Ob2 /fp:fast /GR-)# /DNDEBUG
else()
	set(COMMON_OPTIONS -pthread)
	set(DEBUG_OPTIONS -O0 -g -DDEBUG)
	set(RELEASE_OPTIONS -O2) #-DNDEBUG defined by default
endif()

set(DEBUG_OPTIONS ${COMMON_OPTIONS} ${DEBUG_OPTIONS})
set(RELEASE_OPTIONS ${COMMON_OPTIONS} ${RELEASE_OPTIONS})

if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")
endif()

add_compile_options(
	"$<$<CONFIG:Debug>:${DEBUG_OPTIONS}>"
	"$<$<CONFIG:Release>:${RELEASE_OPTIONS}>"
)

OPTION(SIMPLEUTF_TEST_CXX_STANDARD
	"C++ standard version used to build SimpleUtf test executable." 11)

################################################################################
# Fetching dependencise
################################################################################

include(FetchContent)

FetchContent_Declare(
  git_googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)
FetchContent_MakeAvailable(git_googletest)

################################################################################
# Adding testing executable
################################################################################

set(SOURCES_DIR_PATH ${CMAKE_CURRENT_LIST_DIR}/src)

file(GLOB_RECURSE SOURCES ${SOURCES_DIR_PATH}/*.[ch]*)

add_executable(SimpleUtf_test ${SOURCES})

target_link_libraries(SimpleUtf_test SimpleUtf gtest)

add_test(NAME SimpleUtf_test
	COMMAND SimpleUtf_test)

set_property(TARGET SimpleUtf_test
	PROPERTY CXX_STANDARD ${SIMPLEUTF_TEST_CXX_STANDARD})