#include <gtest/gtest.h>

#include "ul.h"

TEST(utils, test_base_name)
{
    auto base_name = std::string(R"(VCRUNTIME140D.dll)");
    auto path = std::string(R"(C:\Windows\SYSTEM32\VCRUNTIME140D.dll)");
    ASSERT_EQ(::ul::base_name(path), base_name);
}

TEST(utils, test_from_ansi_wstring_to_string)
{
    auto wstring = std::wstring(L"foobar");
    auto string = std::string("foobar");
    ASSERT_EQ(::ul::from_ansi_wstring_to_string(wstring), string);
}

TEST(utils, test_from_string_to_wstring)
{
    auto wstring = std::wstring(L"foobar");
    auto string = std::string("foobar");
    ASSERT_EQ(::ul::from_string_to_wstring(string), wstring);
}