
function(add_example name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} ${SDL2_LIBRARY})
endfunction(add_example)

add_example(example1)
add_example(example2)

# Add the C example too
add_executable(c_example1 example1.c)
target_link_libraries(c_example1 ${SDL2_LIBRARY})
