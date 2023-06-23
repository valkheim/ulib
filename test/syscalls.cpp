#include <gtest/gtest.h>

#include <algorithm>

#include "test.h"
#include "ul.h"


TEST(syscalls, with_NtOpenFile)
{
  ASSERT_TRUE(::ul::with_syscall("NtOpenFile", [](::ul::Syscall const &syscall) {
    // e.g. NtOpenFile, 51, 0x33
    auto const str = syscall.to_string();
    ASSERT_TRUE(str.starts_with("NtOpenFile"));
    ASSERT_EQ(std::count(str.begin(), str.end(), ','), 2);
  }));
}

TEST(syscalls, test_list)
{
  auto syscalls = ::ul::get_syscalls();
  std::sort(syscalls.begin(), syscalls.end(), [](::ul::Syscall const &lhs, ::ul::Syscall const &rhs) { return lhs.name < rhs.name; });

  // for(auto&s:syscalls) printf("%s %d\n",s.name.c_str(),s.number);

  auto build_version = ::ul::get_windows_build_version();
  ASSERT_TRUE(build_version.has_value());
  auto build_number = build_version->c;
  auto filename = std::to_string(build_version->a) + "." + std::to_string(build_version->b) + "." + std::to_string(build_version->c) + "." + std::to_string(build_version->d);
  // printf("filename is %s\n", filename.c_str());
#ifdef _WIN64
  auto lines = ::ul::test::get_lines(R"(.\ulib\test\golden\syscalls\x64\)" + filename + R"(.txt)");
#else
  auto lines = ::ul::test::get_lines(R"(.\ulib\test\golden\syscalls\x86\)" + filename + R"(.txt)");
#endif
  ASSERT_EQ(lines.size(), syscalls.size());
  for (unsigned i = 0; i < lines.size(); ++i) {
    auto expected = lines[i];
    auto got = syscalls[i].name + " " + std::to_string(syscalls[i].number);
    ASSERT_EQ(expected, got);
  }
}