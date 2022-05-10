#include <gtest/gtest.h>
#include <windows.h>
#include "ulib.h"

TEST(threads, test_walk_threads_using_toolhelp)
{
    auto using_process_name = ::ul::Threads{};
    ASSERT_TRUE(::ul::walk_threads_using_toolhelp("ulib_test.exe", [&](::ul::Thread const &thread) -> ::ul::walk_t {
        using_process_name.emplace_back(thread);
        return ::ul::walk_t::WALK_CONTINUE;
    }));

    auto using_process_id = ::ul::Threads{};
    ASSERT_TRUE(::ul::with_process_using_enumprocess("ulib_test.exe", [&](::ul::Process const &process) -> ::ul::walk_t { // retrieve PID
        ::ul::walk_threads_using_toolhelp(process.pid, [&](::ul::Thread const &thread) -> ::ul::walk_t {
            using_process_id.emplace_back(thread);
            return ::ul::walk_t::WALK_CONTINUE;
        });

        return ::ul::walk_t::WALK_CONTINUE;
    }));

    ASSERT_TRUE(using_process_name.size() >= 1);
    ASSERT_EQ(using_process_name.size(), using_process_id.size());
    for(unsigned i = 0 ; i < using_process_id.size() ; ++i)
        ASSERT_EQ(using_process_name[i].tid, using_process_id[i].tid);
}
