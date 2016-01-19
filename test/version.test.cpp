
#include <sdl++/version.hpp>

#include <gtest/gtest.h>

TEST(version, constructors) {
    constexpr sdl::version v1{};

    EXPECT_EQ(v1.major, 0);
    EXPECT_EQ(v1.minor, 0);
    EXPECT_EQ(v1.patch, 0);

    constexpr sdl::version v2{1, 2, 3};

    EXPECT_EQ(v2.major, 1);
    EXPECT_EQ(v2.minor, 2);
    EXPECT_EQ(v2.patch, 3);
}

TEST(version, rel_ops) {
    constexpr auto v1 = sdl::version{1, 2, 3};
    constexpr auto v2 = sdl::version{3, 2, 1};

    constexpr bool same_eq = (v1 == v1);
    constexpr bool diff_eq = (v1 == v2);
    constexpr bool neq = (v1 != v2);
    constexpr bool lt = (v1 < v2);
    constexpr bool le = (v1 <= v2);
    constexpr bool gt = (v1 > v2);
    constexpr bool ge = (v1 >= v2);

    EXPECT_TRUE(same_eq);
    EXPECT_FALSE(diff_eq);
    EXPECT_TRUE(neq);
    EXPECT_TRUE(lt);
    EXPECT_TRUE(le);
    EXPECT_FALSE(gt);
    EXPECT_FALSE(ge);
}

TEST(version, compiled) {
    constexpr auto version = sdl::get_compiled_version();

    EXPECT_EQ(SDL_MAJOR_VERSION, version.major);
    EXPECT_EQ(SDL_MINOR_VERSION, version.minor);
    EXPECT_EQ(SDL_PATCHLEVEL, version.patch);
}

TEST(version, linked) {
    const auto cpp_ver = sdl::get_linked_version();
    SDL_version c_ver;
    SDL_GetVersion(&c_ver);

    EXPECT_EQ(c_ver.major, cpp_ver.major);
    EXPECT_EQ(c_ver.minor, cpp_ver.minor);
    EXPECT_EQ(c_ver.patch, cpp_ver.patch);
}

TEST(version, get_revision) {
    const auto cpp_rev = sdl::get_revision();
    const auto c_rev = SDL_GetRevision();

    EXPECT_STREQ(c_rev, cpp_rev);
}

TEST(version, get_revision_num) {
    const auto cpp_rev_no = sdl::get_revision_number();
    const auto c_rev_no = SDL_GetRevisionNumber();

    EXPECT_EQ(c_rev_no, cpp_rev_no);
}
