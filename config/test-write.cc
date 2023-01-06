#include "config"
#include "gtest/gtest.h"
#include <sstream>

using namespace yuc;

TEST(config, write) {
  std::stringstream ss;
  config c;

  c.write_json_inline(ss);
  EXPECT_EQ(ss.str(), "null");
  ss.str("");

  c.unset(), c.arr();
  c.write_json_inline(ss);
  EXPECT_EQ(ss.str(), "[]");
  ss.str("");

  c.unset(), c.obj();
  c.write_json_inline(ss);
  EXPECT_EQ(ss.str(), "{}");
  ss.str("");

  c["path.to"]["val"] = M_PI;
  c["path.to"]["arr"].arr() = {1.00, 2, "3.0f", 4.5, {}};
  c["path.to"]["obj"].obj() = {
      {"rose", "red"}, {"violet", "blue"}, {"temperature", 25.}};
  c["path.to"]["empty_arr"].arr();
  c["path.to"]["empty_obj"].obj();
  c["path.to"]["deep.null"] = {};
  c["path.to"]["true"] = true;
  c["path.to"]["false"] = false;

  c.write_json_inline(ss);
  EXPECT_EQ(ss.str(),
            "{\"path\":{\"to\":{"
            "\"val\":3.14159,"
            "\"arr\":[1,2,\"3.0f\",4.5,null],"
            "\"obj\":{\"rose\":\"red\",\"violet\":\"blue\",\"temperature\":25},"
            "\"empty_arr\":[],"
            "\"empty_obj\":{},"
            "\"deep\":{\"null\":null},"
            "\"true\":true,\"false\":false"
            "}}}");
  ss.str("");

  c.write_toml_inline(ss);
  EXPECT_EQ(ss.str(),
            "{\"path\"={\"to\"={"
            "\"val\"=3.14159,"
            "\"arr\"=[1,2,\"3.0f\",4.5,{}],"
            "\"obj\"={\"rose\"=\"red\",\"violet\"=\"blue\",\"temperature\"=25},"
            "\"empty_arr\"=[],"
            "\"true\"=true,\"false\"=false"
            "}}}");
  ss.str("");
}
