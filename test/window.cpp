#include <gtest/gtest.h>
#include "ulib.h"
#include "test.h"

TEST(window, test_notepad_windows_count)
{
    auto count_without_notepad = ::ul::count_windows();
    auto count_with_notepad = count_without_notepad;
    ::ul::test::with_process(std::string{"notepad.exe"}, [&](PROCESS_INFORMATION const *pi){
        Sleep(500);
        count_with_notepad = ::ul::count_windows();
    });

    ASSERT_TRUE(count_with_notepad != count_without_notepad);
    ASSERT_TRUE(count_with_notepad > count_without_notepad);
    ASSERT_TRUE(count_with_notepad == 4 + count_without_notepad);
}