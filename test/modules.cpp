#include <gtest/gtest.h>
#include <windows.h>

#include "ul.h"


TEST(modules, test_modules_found)
{
  auto i = 0;
  ::ul::walk_modules_using_ntquerysysteminformation([&i](::ul::Module const *module) -> ::ul::walk_t {
    i++;
    // ::ul::show_module(module);
    return ::ul::walk_t::WALK_CONTINUE;
  });

  ASSERT_TRUE(i > 0);
}

#if _WIN64
TEST(modules, test_first_module_is_ntoskrnl)
{
  auto name = std::string{};
  ::ul::walk_modules_using_ntquerysysteminformation([&](::ul::Module const *module) -> ::ul::walk_t {
    name = module->name;
    return ::ul::walk_t::WALK_STOP;
  });

  ASSERT_EQ(name, "ntoskrnl.exe");
}
#endif