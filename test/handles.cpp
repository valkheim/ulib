#include <gtest/gtest.h>
#include <windows.h>

#include "ul.h"

TEST(handles, test_handles_found)
{
  auto i = 0;
  ::ul::walk_handles([&i](PSYSTEM_HANDLE_TABLE_ENTRY_INFO const handle) -> ::ul::walk_t {
    i++;
    //::ul::show_handle(handle);
    return ::ul::walk_t::WALK_CONTINUE;
  });

  ASSERT_TRUE(i > 0);
}

TEST(handles, test_self_handle_found)
{
  auto pid = (USHORT)GetCurrentProcessId();
  ASSERT_TRUE(::ul::with_handle(pid, [&](PSYSTEM_HANDLE_TABLE_ENTRY_INFO handle) -> ::ul::walk_t {
    //::ul::show_handle(handle);
    return ::ul::walk_t::WALK_CONTINUE;
  }));
}