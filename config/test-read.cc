#include "config"
#include "gtest/gtest.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <variant>

using namespace yuc;

#define __check_eof(is)                                                        \
  EXPECT_FALSE(is.fail());                                                     \
  EXPECT_FALSE(is.eof());                                                      \
  EXPECT_EQ(is.get(), EOF);                                                    \
  EXPECT_TRUE(is.fail());                                                      \
  EXPECT_TRUE(is.eof());

TEST(config, parse_json) {
  config c;
  std::istringstream iss;
  std::ostringstream oss;

#define __test_parse_json(raw, dig)                                             \
  iss.clear(), iss.str(raw), c.parse_json(iss);                                 \
  oss.clear(), oss.str(""), c.write_json_inline(oss);                          \
  EXPECT_EQ(dig, oss.str());

  c = 1.5;
  __test_parse_json("", "1.5");
  EXPECT_TRUE(c == 1.5);
  EXPECT_TRUE(iss.fail());
  EXPECT_TRUE(iss.eof());

  __test_parse_json("false", "false");
  EXPECT_TRUE(c.holds<config::bol_t>());
  EXPECT_EQ(c, false);

  __test_parse_json("null", "null");
  EXPECT_EQ(c, config::nil);
  __check_eof(iss);

  __test_parse_json("true", "true");
  EXPECT_TRUE(c.holds<config::bol_t>());
  EXPECT_EQ(c, true);
  __check_eof(iss);

  __test_parse_json(".162500e2", "16.25");
  EXPECT_TRUE(c.holds<config::dbl_t>());
  EXPECT_EQ(c, 16.25);
  __check_eof(iss);

#undef __test_parse_json
}

#define __test_toml_s(type, raw)                                               \
  iss.clear(), iss.str(raw), c.unset(), c.parse_toml_inline(iss);               \
  EXPECT_TRUE(c.holds<type>());

#define __test_toml(type, raw, dig)                                            \
  iss.clear(), iss.str(raw), c.unset(), c.parse_toml_inline(iss);               \
  EXPECT_TRUE(c.holds<type>());                                                \
  EXPECT_EQ(dig, c.get<type>());                                               \
  __check_eof(iss);

TEST(config, parse_toml_inline_string) {
  using namespace __config_detail;
  config c;
  std::istringstream iss;

  __test_toml(str_t, "\"abc\"", "abc");
  __test_toml(str_t, "\"\"\"abc\"\"\"", "abc");
  __test_toml(str_t, "\"\"\"a\"b\"\"c\"\"\"", "a\"b\"\"c");
  __test_toml(str_t, "\"\"\"\nabc\"\"\"", "abc");
  __test_toml(str_t, "\"\"\"\n\nabc\"\"\"", "\nabc");
  __test_toml(str_t, "\"\"\"\\n\nabc\"\"\"", "\n\nabc");
  __test_toml(str_t, "\"\"\"\\\n\n  \t abc\"\"\"", "abc");
  __test_toml(str_t, "\"\"\"\r\n\\\r\n \ta\\\r\n\tbc\"\"\"", "abc");

  __test_toml(str_t, "'abc'", "abc");
  __test_toml(str_t, "'''abc'''", "abc");
  __test_toml(str_t, "'''a'b''c'''", "a'b''c");
  __test_toml(str_t, "'''\nabc'''", "abc");
  __test_toml(str_t, "'''\n\nabc'''", "\nabc");
  __test_toml(str_t, "'''\\n\nabc'''", "\\n\nabc");
  __test_toml(str_t, "'''\\\n\n  \t abc'''", "\\\n\n  \t abc");
  __test_toml(str_t, "'''\r\n\\\r\n \ta\\\r\n\tbc'''", "\\\r\n \ta\\\r\n\tbc");
}

TEST(config, parse_toml_inline_number) {
  using namespace __config_detail;
  config c;
  std::istringstream iss;

  // bool
  __test_toml(bol_t, "true", true);
  __test_toml(bol_t, "false", false);

  // hex
  __test_toml(int_t, "0xDEADBEEF", 0xDEADBEEF);
  __test_toml(int_t, "0xdeadbeef", 0xdeadbeef);
  __test_toml(int_t, "0xdead_beef", 0xdeadbeef);

  // oct
  __test_toml(int_t, "0o01234567", 01234567);
  __test_toml(int_t, "0o755", 0755);

  // bin
  __test_toml(int_t, "0b11010110", 0xd6);

  // regular integer
  __test_toml(int_t, "+99", +99);
  __test_toml(int_t, "42", +42);
  __test_toml(int_t, "+0", +0);
  __test_toml(int_t, "-0", +0);
  __test_toml(int_t, "-17", -17);

  // groupped integer
  __test_toml(int_t, "1_000", 1000);
  __test_toml(int_t, "5_349_221", 5349221);
  __test_toml(int_t, "53_49_221", 5349221);
  __test_toml(int_t, "1_2_3_4_5", 12345);

  // float
  __test_toml(dbl_t, "+1.0", 1.0);
  __test_toml(dbl_t, "3.1415", 3.1415);
  __test_toml(dbl_t, "15.19", 15.19);
  __test_toml(dbl_t, "-0.01", -0.01);
  __test_toml(dbl_t, "5e+22", 5e22);
  __test_toml(dbl_t, "1e06", 1e6);
  __test_toml(dbl_t, "-2E-2", -0.02);
  __test_toml(dbl_t, "224_617.445_991_228", 224617.445991228);
  // floating point error consistency
  __test_toml(dbl_t, "662.6e-36", 6.626e-34);

  // special values
  __test_toml(dbl_t, "inf", HUGE_VAL);
  __test_toml(dbl_t, "+inf", +HUGE_VAL);
  __test_toml(dbl_t, "-inf", -HUGE_VAL);
  __test_toml_s(dbl_t, "nan");
  EXPECT_TRUE(std::isnan(c.get<dbl_t>()));
  __test_toml_s(dbl_t, "+nan");
  EXPECT_TRUE(std::isnan(c.get<dbl_t>()));
  __test_toml_s(dbl_t, "-nan");
  EXPECT_TRUE(std::isnan(c.get<dbl_t>()));

  // non-standard numeric behaviors
  __test_toml(int_t, "+9223372036854775807", (long)0x7fffffffffffffff);
  __test_toml(int_t, "-9223372036854775808", (long)0x8000000000000000);
  __test_toml(dbl_t, "+9223372036854775808", +9.2233720368547758e+18);
  __test_toml(dbl_t, "-9223372036854775809", -9.2233720368547758e+18);
  __test_toml(dbl_t, "1.7e308", 1.7e308);
  __test_toml(dbl_t, "1.8e308", HUGE_VAL);
  __test_toml(dbl_t, "2.5e-324", 2.5e-324);
  __test_toml(dbl_t, "2.4e-324", 0.0);

  // cross time compare
  __test_toml(int_t, "120000", 1.2e5);
  __test_toml(dbl_t, "1.2e5", 120000);

  // // date
  // "2000-01-01";
  // "2000-W01-1";
  // "2000-001";
  //
  // // local time
  // "13:30";
  // "13:30:25";
  // "13:30:25.329403";
  //
  // // global time
  // "13:30Z";
  // "13:30:25-02";
  // "13:30:25+0800";
  // "13:30:25.329403+08:00";
  //
  // // date time
  // "2020-01-02T02:30:43";
  // "2020-01-01 18:30:43Z";
  // "2020-01-02 02:30:43+0800";
}

TEST(config, parse_file) {
  for (size_t i = 1; i <= 99; ++i) {
    std::stringstream fss;
    fss << (TEST_SRC_DIR "/output-inline-");
    fss << std::setw(2) << std::setfill('0') << i << ".json";
    std::ifstream ifs(fss.str());
    if (!ifs) {
      break;
    }
    std::string full_output;
    std::getline(ifs, full_output, (char)EOF);
    ifs.close();

    fss.str("");
    fss.clear();
    fss << (TEST_SRC_DIR "/input-");
    fss << std::setw(2) << std::setfill('0') << i;

    if (auto fn = fss.str() + ".json"; ifs.open(fn), ifs) {
      config c;
      std::stringstream oss;
      c.parse_auto(fn);
      c.write_json_inline(oss) << std::endl;
      EXPECT_EQ(full_output, oss.str()) << fn;
    }
    ifs.close();

    if (auto fn = fss.str() + ".toml"; ifs.open(fn), ifs) {
      config c;
      std::stringstream oss;
      c.parse_auto(fn);
      c.write_json_inline(oss) << std::endl;
      EXPECT_EQ(full_output, oss.str()) << fn;
    }
    ifs.close();
  }
}

TEST(config, read_string) {
  config c;
  c["a"] = 13;
  c.read_string("a = 15");
  c.read_string("b = 'B'");
  c.read_string("c = []");
  EXPECT_EQ((int)c["a"], 15);
  EXPECT_EQ(c["b"].str(), "B");
  EXPECT_EQ(c["b"].size(), 0);
}

#undef __check_eof
