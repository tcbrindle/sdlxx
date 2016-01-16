
#include <sdl++/cpuinfo.hpp>

#include <gtest/gtest.h>

TEST(cpuinfo, cpu_count) { EXPECT_EQ(SDL_GetCPUCount(), sdl::get_cpu_count()); }

TEST(cpuinfo, cache_line_size) {
    EXPECT_EQ(SDL_GetCPUCacheLineSize(), sdl::get_cpu_cache_line_size());
}

TEST(cpuinfo, has_rdtsc) { EXPECT_EQ(SDL_HasRDTSC(), sdl::cpu_has_rdtsc()); }

TEST(cpuinfo, has_altivec) {
    EXPECT_EQ(SDL_HasAltiVec(), sdl::cpu_has_altivec());
}

TEST(cpuinfo, has_mmx) { EXPECT_EQ(SDL_HasMMX(), sdl::cpu_has_mmx()); }

TEST(cpuinfo, has_3dnow) { EXPECT_EQ(SDL_Has3DNow(), sdl::cpu_has_3dnow()); }

TEST(cpuinfo, has_sse) { EXPECT_EQ(SDL_HasSSE(), sdl::cpu_has_sse()); }

TEST(cpuinfo, has_sse2) { EXPECT_EQ(SDL_HasSSE2(), sdl::cpu_has_sse2()); }

TEST(cpuinfo, has_sse3) { EXPECT_EQ(SDL_HasSSE3(), sdl::cpu_has_sse3()); }

TEST(cpuinfo, has_sse41) { EXPECT_EQ(SDL_HasSSE41(), sdl::cpu_has_sse41()); }

TEST(cpuinfo, has_sse42) { EXPECT_EQ(SDL_HasSSE42(), sdl::cpu_has_sse42()); }

TEST(cpuinfo, has_avx) { EXPECT_EQ(SDL_HasAVX(), sdl::cpu_has_avx()); }

TEST(cpuinfo, has_avx2) { EXPECT_EQ(SDL_HasAVX2(), sdl::cpu_has_avx2()); }

TEST(cpuinfo, system_ram) {
    EXPECT_EQ(SDL_GetSystemRAM(), sdl::get_system_ram());
}
