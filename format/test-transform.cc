#include "format"
#include <gtest/gtest.h>

TEST(format, upper_lower)
{
  EXPECT_EQ("ABC", yuc::toupper("aBc"));
  EXPECT_EQ("abc", yuc::tolower("aBc"));
}
