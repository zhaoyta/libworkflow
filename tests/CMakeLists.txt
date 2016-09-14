cmake_minimum_required (VERSION 3.0)
project (test_workflow)
add_subdirectory(common)


set (CMAKE_CXX_STANDARD 11)
add_definitions(-Wno-deprecated-register)


INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} .)

file(GLOB files *.cpp)

foreach(f ${files})
	get_filename_component(filename ${f} NAME_WE)
	add_executable("${filename}" ${f} )
	TARGET_LINK_LIBRARIES("${filename}" libworkflow)
	TARGET_LINK_LIBRARIES("${filename}" test_common)
endforeach()

