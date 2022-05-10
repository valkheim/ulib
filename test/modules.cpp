#include <gtest/gtest.h>
#include <windows.h>
#include "ulib.h"

TEST(modules, test_modules_found)
{
    auto i = 0;
    ::ul::walk_modules([&i](PSYSTEM_MODULE const module) -> ::ul::walk_t {
        i++;
        //::ul::show_module(module);
        return ::ul::walk_t::WALK_CONTINUE;
    });

    ASSERT_TRUE(i > 0);
}

#if _WIN64
TEST(modules, test_first_module_is_ntoskrnl)
{
    auto name = std::string{};
    ::ul::walk_modules([&](PSYSTEM_MODULE const module) -> ::ul::walk_t {
        name = std::string{module->Name + module->NameOffset};
        return ::ul::walk_t::WALK_STOP;
    });

    ASSERT_EQ(name, "ntoskrnl.exe");
}
#endif