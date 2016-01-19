
add_library(sdlxx_main main.cpp)

function(add_gtest name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} SDL2 SDL2main gtest sdlxx_main)
    add_test(${name} ${name} --gtest_color=yes)
endfunction(add_gtest)

add_gtest(cpuinfo.test)
add_gtest(init.test)
add_gtest(platform.test)
add_gtest(power.test)
add_gtest(timer.test)
add_gtest(version.test)
