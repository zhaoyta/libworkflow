cmake_minimum_required (VERSION 3.3)
project (libworkflow)

set (CMAKE_CXX_STANDARD 11)

set(Boost_INCLUDE_DIR /usr/local/include)
set(Boost_LIBRARY_DIR /usr/local/lib)
FIND_PACKAGE( Boost 1.55 COMPONENTS system timer serialization regex log iostreams chrono REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} .)

file(GLOB files core/*.cpp actions/*.cpp contexts/*.cpp tools/*.cpp service/*.cpp)


add_library(libworkflow ${files})
TARGET_LINK_LIBRARIES(libworkflow ${Boost_LIBRARIES} )
