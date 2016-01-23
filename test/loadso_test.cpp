
#include <sdl++/loadso.hpp>

#include "catch.hpp"

#ifdef __APPLE__
constexpr const char libname[] = "libloadso_test_lib.dylib";
#elif __unix__
constexpr const char libname[] = "libloadso_test_lib.so";
#elif _WIN32
constexpr const char libname[] = "loadso_test_lib.lib";
#endif

TEST_CASE("sdl::shared_object_loader constructor handles NULL", "[loadso]") {
    REQUIRE_THROWS(sdl::shared_object_handle(nullptr));
}

TEST_CASE("sdl::shared_object_loader constructor handles unknown files",
          "[loadso]") {
    REQUIRE_THROWS(sdl::shared_object_handle("non existant file"));
}

TEST_CASE("sdl::shared_object_loader constructs objects correctly",
          "[loadso]") {
    auto handle = sdl::shared_object_handle(libname);
    REQUIRE(handle);

    SECTION("Function pointers can be loaded") {
        auto func = handle.load_function<int(int) >("test_func");
        REQUIRE(func(1000) == 1000);

        SECTION("...and moving maintains pointers") {
            auto handle2 = std::move(handle);
            REQUIRE(handle2);
            REQUIRE_FALSE(handle);
            REQUIRE(func(1000) == 1000);
        }
    }

    SECTION("Unknown function pointer names are handled") {
        REQUIRE_THROWS(handle.load_function<void(void) >("unknown"));
    }
}
