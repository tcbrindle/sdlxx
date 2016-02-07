
#ifdef __WIN32__
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC
#endif

DECLSPEC int test_func(int i) { return i; }
