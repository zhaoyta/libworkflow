cmake_minimum_required (VERSION 3.0)
project (test_common)

set (CMAKE_CXX_STANDARD 11)

add_definitions(-Wno-deprecated-register)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} . ../../)

file(GLOB common *.cpp actions/*.cpp contexts/*.cpp)

	add_library(test_common ${common})
	TARGET_LINK_LIBRARIES(libworkflow)

