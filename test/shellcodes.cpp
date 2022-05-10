#include <gtest/gtest.h>
#include "ulib.h"

TEST(shellcodes, test_sizes)
{
    ASSERT_EQ(sizeof(ul::shellcodes::x86::pop_calc), 201);
}