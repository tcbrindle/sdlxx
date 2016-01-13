
#include "SDL.h"

#include <gtest/gtest.h>

/*
   On windows SDL does very strange things, like using a macro to redefine
   main(). It all works, provided you #include "SDL.h" at the point at which
   you define main() -- which gtest_main does not, of course. So we redefine
   a main function which can then be linked to every test.
*/

// Force C linkage as per SDL documentation
extern "C"
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}