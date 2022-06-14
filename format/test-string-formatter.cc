#include "format"
#include <gtest/gtest.h>

using namespace yuc::string_literals;

TEST(format, printf_tests){
#include "generated-printf-tests.inc"
}

TEST(format, support_cxx_string)
{
  std::string word = "world";
  EXPECT_EQ("Hello world", "Hello %s"_fmt(word));
}
