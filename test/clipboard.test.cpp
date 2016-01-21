
#include <sdl++/clipboard.hpp>

#include <gtest/gtest.h>

#include <cstdlib>

namespace {

constexpr const char test_str[] = u8"SDL Clipboard test \u2713";

void set_test_text() {
    int r = SDL_SetClipboardText(test_str);
    if (r != 0) { throw std::runtime_error(SDL_GetError()); }
}

} // end anonymous namespace

struct clipboard : ::testing::Test {
    clipboard() { SDL_Init(SDL_INIT_VIDEO); }
    ~clipboard() { SDL_Quit(); }
};

TEST_F(clipboard, get) {
    set_test_text();
    EXPECT_EQ(test_str, sdl::get_clipboard_text());
}

TEST_F(clipboard, has) {
    set_test_text();
    EXPECT_TRUE(sdl::has_clipboard_text());
}

TEST_F(clipboard, set) {
    ASSERT_NE(SDL_TRUE, SDL_HasClipboardText());
    sdl::set_clipboard_text(test_str);
    EXPECT_STREQ(test_str, SDL_GetClipboardText());
}
