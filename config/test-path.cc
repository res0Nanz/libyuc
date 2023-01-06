#include "config"
#include "gtest/gtest.h"

yuc::config c = yuc::config::obj_t{
    {"bol", true},
    {"num", 42},
    {"str", "word"},
    {"obj", yuc::config::obj_t{{"a", 3}, {"b", 4}}},
    {"arr", yuc::config::arr_t{10, 11, 12}},
};

TEST(config, path_simple)
{
  EXPECT_EQ(c["bol"], true);
  EXPECT_EQ(c["num"], 42);
  EXPECT_EQ(c["str"], "word");
  EXPECT_EQ(c["nil"], yuc::config::nil);
  const auto& cc = c;
  EXPECT_EQ(cc["bol"], true);
  EXPECT_EQ(cc["num"], 42);
  EXPECT_EQ(cc["str"], "word");
  EXPECT_EQ(cc["nil"], yuc::config::nil);
}

TEST(config, path_obj)
{
  EXPECT_EQ(c["obj.a"], 3);
  EXPECT_EQ(c["obj['b']"], 4);
  EXPECT_EQ(c["obj.c"], yuc::config::nil);
  const auto& cc = c;
  EXPECT_EQ(cc["obj.a"], 3);
  EXPECT_EQ(cc["obj['b']"], 4);
  EXPECT_EQ(cc["obj.c"], yuc::config::nil);
}

TEST(config, path_arr)
{
  EXPECT_EQ(c["arr[0]"], 10);
  EXPECT_EQ(c["arr[1]"], 11);
  EXPECT_EQ(c["arr[-1]"], 12);
  const auto& cc = c;
  EXPECT_EQ(cc["arr[0]"], 10);
  EXPECT_EQ(cc["arr[1]"], 11);
  EXPECT_EQ(cc["arr[-1]"], 12);
}

TEST(config, path_maniac)
{
  yuc::config c;
  c["a b"]["c d"]["e f"] = yuc::config::arr_t{1, 2, 3};
  EXPECT_EQ(c["a b['c d[\"e f[-1]\"]']"], 3);
  const auto& cc = c;
  EXPECT_EQ(cc["a b['c d[\"e f[-1]\"]']"], 3);
}
