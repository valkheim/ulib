#include <gtest/gtest.h>
#include "ulib.h"

TEST(version, test_ulib_version)
{
    ASSERT_EQ(ul::get_ulib_version(), "1.0.0");
}

TEST(version, test_windows_build_version)
{
    auto build = ::ul::get_windows_build_version();
    ASSERT_TRUE(build.has_value());
    ASSERT_EQ(build->a, 10); // Windows 10
    const auto str = build->to_string();
    ASSERT_EQ(std::count(str.begin(), str.end(), '.'), 3); // e.g. 10.0.19041.928
}