#include <gtest/gtest.h>
#include "ulib.h"

//typedef int(__stdcall* _ret)(int);
typedef int(* _ret)(int);

_ret hook_location;
::ul::IHook* my_hook = nullptr;

// 'ret' alt function
auto retHook(int dummy) -> int
{
  my_hook->unhook();
  auto r = (*hook_location)(dummy);
  my_hook->hook();
  return r * 2;
}

struct Hook : ::testing::Test
{
    HMODULE module;

    Hook() {
        #if _WIN64
        module = LoadLibraryA(R"(build\x64\dll_sample_injectee\Debug\dll_sample_injectee.dll)");
        #else
        module = LoadLibraryA(R"(build\x86\dll_sample_injectee\Debug\dll_sample_injectee.dll)");
        #endif
        // The 'ret' export returns the value passed as parameter
        hook_location = reinterpret_cast<_ret>(GetProcAddress(module, "ret"));
    }

    ~Hook() override {
        FreeLibrary(module);
    }

    void test() {
        ASSERT_TRUE(hook_location != nullptr);
        ASSERT_TRUE(my_hook != nullptr);
        ASSERT_FALSE(my_hook->hooked());
        ASSERT_EQ((*hook_location)(42), 42);
        my_hook->hook();
        ASSERT_EQ((*hook_location)(42), 84);
        my_hook->unhook();
    }
};

#if _WIN64
TEST_F(Hook, test_hook_x64_FF25)
{
    my_hook = new ::ul::x64::HookFF25((FARPROC)hook_location, (PROC)retHook);
    test();
    delete my_hook;
}

TEST_F(Hook, test_hook_x64_48B8)
{
    my_hook = new ::ul::x64::Hook48B8((FARPROC)hook_location, (PROC)retHook);
    test();
    delete my_hook;
}

TEST_F(Hook, test_hook_x64_68)
{
    my_hook = new ::ul::x64::Hook68((FARPROC)hook_location, (PROC)retHook);
    test();
    delete my_hook;
}
#else
TEST_F(Hook, test_hook_x86_E9)
{
    my_hook = new ::ul::x86::HookE9((FARPROC)hook_location, (PROC)retHook);
    test();
    delete my_hook;    
}
#endif