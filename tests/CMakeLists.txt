
add_subdirectory(googletest)

function(add_gtest name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} ${SDL2_LIBRARY} gtest gtest_main pthread)
    add_test(${name} ${name} --gtest_color=yes)
endfunction(add_gtest)

add_gtest(version.test)