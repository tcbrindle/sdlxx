
add_executable(test-sdl++
    catch_main.cpp
    blendmode_test.cpp
    clipboard_test.cpp
    cpuinfo_test.cpp
    init_test.cpp
    platform_test.cpp
    power_test.cpp
    timer_test.cpp
    version_test.cpp
    )

target_link_libraries(test-sdl++ SDL2 SDL2main)

add_test(test-sdl++ test-sdl++)
