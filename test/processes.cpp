#include <gtest/gtest.h>
#include <windows.h>

#include "ul.h"


TEST(processes, test_using_enumprocess)
{
  auto processes = ul::get_processes_using_enumprocess();
  ASSERT_FALSE(processes.empty());
  ASSERT_TRUE(std::find_if(processes.begin(), processes.end(), [](::ul::Process const &process) { return process.pid == GetCurrentProcessId(); }) !=
              processes.end());
}

TEST(processes, test_using_toolhelp)
{
  auto processes = ul::get_processes_using_toolhelp();
  ASSERT_FALSE(processes.empty());
  ASSERT_TRUE(std::find_if(processes.begin(), processes.end(), [](::ul::Process const &process) { return process.pid == GetCurrentProcessId(); }) !=
              processes.end());
}

TEST(processes, test_using_wts)
{
  auto processes = ul::get_processes_using_wts();
  ASSERT_FALSE(processes.empty());
  ASSERT_TRUE(std::find_if(processes.begin(), processes.end(), [](::ul::Process const &process) { return process.pid == GetCurrentProcessId(); }) !=
              processes.end());
}

TEST(processes, test_using_ntgetnextprocess)
{
  auto processes = ul::get_processes_using_ntgetnextprocess();
  ASSERT_FALSE(processes.empty());
  ASSERT_TRUE(std::find_if(processes.begin(), processes.end(), [](::ul::Process const &process) { return process.pid == GetCurrentProcessId(); }) !=
              processes.end());
}

TEST(processes, test_compatibility)
{
  auto pusing_enumprocess = ul::get_processes_using_enumprocess();
  auto pusing_toolhelp = ul::get_processes_using_toolhelp();
  auto pusing_wts = ul::get_processes_using_wts();
  // auto pusing_ntgetnextprocess = ul::get_processes_using_ntgetnextprocess();
  ASSERT_EQ(pusing_enumprocess.size(), pusing_toolhelp.size());
  ASSERT_EQ(pusing_enumprocess.size(), pusing_wts.size());
  // ASSERT_EQ(pusing_enumprocess.size(), pusing_ntgetnextprocess.size());
}

TEST(processes, test_with_process_using_enumprocess)
{
  auto test_process = ::ul::Process{};
  ASSERT_TRUE(::ul::with_process_using_enumprocess("ulib_test.exe", [&](::ul::Process const &process) -> ::ul::walk_t {
    test_process = process;
    return ::ul::walk_t::WALK_STOP;
  }));
  ASSERT_TRUE(test_process.pid != 4);
  ASSERT_TRUE(test_process.name);
  ASSERT_TRUE(test_process.path);
  ASSERT_EQ(*test_process.name, "ulib_test.exe");
}

TEST(processes, test_with_process_using_toolhelp)
{
  auto test_process = ::ul::Process{};
  ASSERT_TRUE(::ul::with_process_using_toolhelp("ulib_test.exe", [&](::ul::Process const &process) -> ::ul::walk_t {
    test_process = process;
    return ::ul::walk_t::WALK_STOP;
  }));
  ASSERT_TRUE(test_process.pid != 4);
  ASSERT_TRUE(test_process.name);
  ASSERT_TRUE(test_process.path);
  ASSERT_EQ(*test_process.name, "ulib_test.exe");
}

TEST(processes, test_with_process_using_wts)
{
  auto test_process = ::ul::Process{};
  ASSERT_TRUE(::ul::with_process_using_wts("ulib_test.exe", [&](::ul::Process const &process) -> ::ul::walk_t {
    test_process = process;
    return ::ul::walk_t::WALK_STOP;
  }));
  ASSERT_TRUE(test_process.pid != 4);
  ASSERT_TRUE(test_process.name);
  ASSERT_TRUE(test_process.path);
  ASSERT_EQ(*test_process.name, "ulib_test.exe");
}

TEST(processes, test_with_process_using_ntgetnextprocess)
{
  auto test_process = ::ul::Process{};
  ASSERT_TRUE(::ul::with_process_using_wts("ulib_test.exe", [&](::ul::Process const &process) -> ::ul::walk_t {
    test_process = process;
    return ::ul::walk_t::WALK_STOP;
  }));
  ASSERT_TRUE(test_process.pid != 4);
  ASSERT_TRUE(test_process.name);
  ASSERT_TRUE(test_process.path);
  ASSERT_EQ(*test_process.name, "ulib_test.exe");
}