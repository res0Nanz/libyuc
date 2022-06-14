#include "format"
#include <gtest/gtest.h>

TEST(format, string_join)
{
  using yuc::string_join;
  EXPECT_EQ("1,abc,0.2", string_join(',', 1, "abc", 1. / 5));
}

TEST(format, stream_join)
{
  using yuc::stream_join;
  std::stringstream ss;
  ss.str("");
  stream_join(ss, ", ", 1, "abc", 1. / 5);
  EXPECT_EQ("1, abc, 0.2", ss.str());
}

TEST(format, string_iterate_empty)
{
  using vec_t = std::vector<int>;
  EXPECT_EQ("", yuc::string_iterate(", ", vec_t{}));
}

TEST(format, string_iterate)
{
  using vec_t = std::vector<int>;
  EXPECT_EQ("1, 2, 3", yuc::string_iterate(", ", vec_t{1, 2, 3}));
}
