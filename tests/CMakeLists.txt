cmake_minimum_required (VERSION 3.0)
project (test_workflow)

set (CMAKE_CXX_STANDARD 11)


INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} .)

file(GLOB files *.cpp)
file(GLOB common common/*.cpp common/actions/*.cpp common/contexts/*.cpp)

foreach(f ${files})
	get_filename_component(filename ${f} NAME_WE)
	add_executable("${filename}_test" ${f} ${common})
	TARGET_LINK_LIBRARIES("${filename}_test" libworkflow)
endforeach()

