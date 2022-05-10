#include <gtest/gtest.h>
#include <windows.h>
#include "ulib.h"

TEST(processes, test_using_enumprocess)
{
    auto pids = ul::get_processes_ids_using_enumprocess();
    ASSERT_FALSE(pids.empty());
    ASSERT_TRUE(std::find_if(pids.begin(), pids.end(), [](::ul::Process const &process) { return process.pid == GetCurrentProcessId(); }) != pids.end());
}

TEST(processes, test_using_toolhelp)
{
    auto pids = ul::get_processes_ids_using_toolhelp();
    ASSERT_FALSE(pids.empty());
    ASSERT_TRUE(std::find_if(pids.begin(), pids.end(), [](::ul::Process const &process) { return process.pid == GetCurrentProcessId(); }) != pids.end());
}

TEST(processes, test_using_wts)
{
    auto pids = ul::get_processes_ids_using_wts();
    ASSERT_FALSE(pids.empty());
    ASSERT_TRUE(std::find_if(pids.begin(), pids.end(), [](::ul::Process const &process) { return process.pid == GetCurrentProcessId(); }) != pids.end());
}

TEST(processes, test_compatibility)
{
    auto pids_using_enumprocess = ul::get_processes_ids_using_enumprocess();
    auto pids_using_toolhelp = ul::get_processes_ids_using_toolhelp();
    auto pids_using_wts = ul::get_processes_ids_using_wts();
    ASSERT_EQ(pids_using_enumprocess.size(), pids_using_toolhelp.size());
    ASSERT_EQ(pids_using_enumprocess.size(), pids_using_wts.size());
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