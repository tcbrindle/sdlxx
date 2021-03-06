
cmake_minimum_required(VERSION 2.8)

project(sdl++ C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${sdl++_SOURCE_DIR}/cmake")

include(CTest)
enable_testing()

find_package(SDL2)

if (NOT SDL2_FOUND)
    add_subdirectory(external)
    set(SDL2_LIBRARY SDL2-static SDL2main)
    set(SDL2_INCLUDE_DIR "${sdl++_SOURCE_DIR}/external/SDL2/include")
endif()

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror")
    set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")
elseif(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Og")
    set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")
endif()

include_directories(include)
include_directories(${SDL2_INCLUDE_DIR})

add_subdirectory(doc)
add_subdirectory(example)
add_subdirectory(test)

