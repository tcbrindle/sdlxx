
cmake_minimum_required(VERSION 2.8)

project(sdl++ C CXX)

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

include(CTest)
enable_testing()

add_subdirectory(external)

include_directories(external/googletest/googletest/include)
include_directories(external/SDL2/include)
include_directories(include)

option(SDLXX_USE_EXCEPTIONS "Whether to use exceptions for tests" ON)
if (NOT SDLXX_USE_EXCEPTIONS)
    message(STATUS "Not using exceptions for tests")
    add_definitions(-DSDLXX_NO_EXCEPTIONS)
endif()

add_subdirectory(doc)
add_subdirectory(example)
add_subdirectory(test)

