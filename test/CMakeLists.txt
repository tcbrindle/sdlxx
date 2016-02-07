
add_executable(test-sdl++
    catch_main.cpp
    bits_test.cpp
    blendmode_test.cpp
    clipboard_test.cpp
    cpuinfo_test.cpp
    endian_test.cpp
    filesystem_test.cpp
    init_test.cpp
    loadso_test.cpp
    platform_test.cpp
    power_test.cpp
    scancode_test.cpp
    timer_test.cpp
    version_test.cpp
    )

add_library(loadso_test_lib MODULE loadso_test_lib.c)

target_compile_definitions(test-sdl++
    PRIVATE TEST_SO_PATH="${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_SHARED_MODULE_PREFIX}loadso_test_lib${CMAKE_SHARED_MODULE_SUFFIX}"
    )

target_link_libraries(test-sdl++ ${SDL2_LIBRARY})


add_test(test-sdl++ test-sdl++)

