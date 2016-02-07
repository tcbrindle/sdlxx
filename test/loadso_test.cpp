
#include <sdl++/loadso.hpp>

#include "catch.hpp"

extern "C" {
typedef int (*test_func_sig)(int);
}

TEST_CASE("sdl::shared_object_loader constructor handles NULL", "[loadso]") {
    REQUIRE_THROWS_AS(sdl::shared_object_handle(nullptr), sdl::error);
}

TEST_CASE("sdl::shared_object_loader constructor handles unknown files",
          "[loadso]") {
    REQUIRE_THROWS_AS(sdl::shared_object_handle("non existant file"),
                      sdl::error);
}

TEST_CASE("sdl::shared_object_loader constructs objects correctly",
          "[loadso]") {
    auto handle = sdl::shared_object_handle(TEST_SO_PATH);
    REQUIRE(handle);

    SECTION("Function pointers can be loaded") {
        auto func =
            reinterpret_cast<test_func_sig>(handle.load_function("test_func"));
        REQUIRE(func(1000) == 1000);

        SECTION("...and moving maintains pointers") {
            auto handle2 = std::move(handle);
            REQUIRE(handle2);
            REQUIRE_FALSE(handle);
            REQUIRE(func(1000) == 1000);
        }
    }

    SECTION("Unknown function pointer names are handled") {
        REQUIRE_THROWS_AS(handle.load_function("unknown"), sdl::error);
    }
}
