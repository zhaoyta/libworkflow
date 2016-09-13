cmake_minimum_required (VERSION 3.0)
project (libworkflow)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set (CMAKE_CXX_STANDARD 11)

add_definitions(-Wno-deprecated-register)
set(Boost_INCLUDE_DIR /usr/local/include)
set(Boost_LIBRARY_DIR /usr/local/lib)
FIND_PACKAGE( Boost 1.55 COMPONENTS system filesystem thread date_time timer serialization regex log iostreams chrono log_setup REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} .)

file(GLOB_RECURSE files core/*.cpp actions/*.cpp contexts/*.cpp tools/*.cpp service/*.cpp)

add_library(libworkflow ${files})
TARGET_LINK_LIBRARIES(libworkflow ${Boost_LIBRARIES} )

add_subdirectory(tests)

