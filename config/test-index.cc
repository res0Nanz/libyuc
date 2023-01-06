#include "config"
#include "gtest/gtest.h"

#include <cmath>
#include <valarray>

using namespace yuc;

struct S {
  std::string str;
  S(const std::string &s) : str('[' + s + ']') {}
};

TEST(config, indexing) {

  config c;

  EXPECT_FALSE(c.is_set());
  EXPECT_EQ(c, c);
  EXPECT_EQ(c, config::nil);

  c["abc.def.ghi.jkl"];
  EXPECT_TRUE(c.is_set());
  EXPECT_FALSE(c.is_deep_set());

  std::vector<int> emptyv = c["emptyvec"].or_get({});

  c = "abc";
  EXPECT_EQ(c, "abc");
  EXPECT_EQ(std::get<std::string>(c), "abc");
  std::string str = c;
  EXPECT_EQ(str, "abc");
  S s = c;
  EXPECT_EQ(s.str, "[abc]");
  s.str = c;
  EXPECT_EQ(s.str, "abc");

  EXPECT_TRUE(config(15.) == config(15l));
  EXPECT_FALSE(config(15.) == config("15."));

  c.unset();
  c.or_set(4, M_PI_2);
  EXPECT_TRUE(c.holds<config::arr_t>());
  EXPECT_EQ(c.size(), 4ul);
  for (auto &e : c.arr()) {
    EXPECT_EQ(e, M_PI_2);
  }

  std::vector<config> v = {1, M_PI, -3};
  c = v;
  std::valarray<int> vi = c;
  c = {1, M_PI, -3};
  c.or_set({1, M_PI, +3});
  std::vector<float> vf;
  vf = c;

  EXPECT_EQ(1, c[0].num());
  EXPECT_EQ(1, vi[0]);
  EXPECT_FLOAT_EQ(1, vf[0]);

  EXPECT_EQ(M_PI, c[1].num());
  EXPECT_EQ((int)(M_PI), vi[1]);
  EXPECT_FLOAT_EQ((float)M_PI, vf[1]);

  EXPECT_EQ(-3, c[2].num());
  EXPECT_EQ(-3, vi[2]);
  EXPECT_FLOAT_EQ(-3, vf[2]);

  c.unset();
  c["abc.def.ghi.jkl..."] = M_E;

  EXPECT_EQ(M_E, c["abc.def.ghi.jkl"].num(M_PI));
  EXPECT_EQ(M_E, c[".abc.def"]["."]["ghi"]["jkl"].num(M_PI));
  EXPECT_EQ(M_PI, c[".abc.def"]["Z"]["ghi"]["jkl"].num(M_PI));

  EXPECT_EQ(M_E, c["abc.def.Z.ghi.jkl"].or_get(M_E));
  EXPECT_EQ(config::nil, c["abc.def.Z.ghi.jkl"]);
  EXPECT_EQ(M_PI, c["abc.def.Z.ghi.jkl"].or_set(M_PI));
  EXPECT_EQ(M_PI, c["abc.def.Z.ghi.jkl"].or_set(M_E));
  EXPECT_EQ(M_PI, c["abc.def.Z.ghi.jkl"]);

  EXPECT_EQ(c["abc.def.ghi.str"].or_get("ABC"), "ABC");
  EXPECT_EQ(c["abc.def.ghi.str"], config::nil);
  EXPECT_EQ(c["abc.def.ghi.str"].or_set("abc"), "abc");
  EXPECT_EQ(c["abc.def.ghi.str"].or_get("ABC"), "abc");
  EXPECT_EQ(c["abc.def.ghi.str"].or_set("ABC"), "abc");
  EXPECT_EQ(c["abc.def.ghi.str"], "abc");

  EXPECT_EQ(c["  abc. 'def'.ghi.jkl"], M_E);
  EXPECT_EQ(c["  abc.' def'.ghi.jkl"], config::nil);
  EXPECT_EQ(c["abc.def.ghi.jkL"], config::nil);

  const config cc = c;
  EXPECT_EQ(c, cc);
  EXPECT_EQ(M_E, cc["abc.def.ghi.jkl"].num(M_PI));
  EXPECT_FALSE(cc["abcdef.ghijkl"].is_set());
}
