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

#define __test_toml_s(type, raw)                                               \
  iss.clear(), iss.str(raw), c.unset(), c.parse_toml_inline(iss);               \
  EXPECT_TRUE(c.holds<type>());

#define __test_toml(type, raw, dig)                                            \
  iss.clear(), iss.str(raw), c.unset(), c.parse_toml_inline(iss);               \
  EXPECT_TRUE(c.holds<type>());                                                \
  EXPECT_EQ(dig, c.get<type>());                                               \
  __check_eof(iss);

void throw_wrapper(const std::string &input, bool il) {
  try {
    std::stringstream ss(input);
    il ? config().parse_toml_inline(ss) : config().parse_toml(ss);
  } catch (config::parse_error &e) {
    e.what();
    throw;
  }
}
#define __bad_toml_inline(input)                                               \
  EXPECT_THROW(throw_wrapper(input, true), config::parse_error)
#define __bad_toml(input)                                                      \
  EXPECT_THROW(throw_wrapper(input, false), config::parse_error)

TEST(config, exception) {
  __bad_toml_inline("''' abc \n");
  __bad_toml_inline("[\n\n1 2 3 ]");
  __bad_toml_inline("[1 2 3 \n");
  __bad_toml_inline("{ a = 12 b = 13}");
  __bad_toml_inline("{ a = 12, b }");
  __bad_toml_inline("{ a = 12");
  __bad_toml_inline("abc ");
  __bad_toml_inline("truue ");
  __bad_toml_inline("falss ");
  __bad_toml_inline("naan ");
  __bad_toml_inline("innfs ");
  __bad_toml_inline("+naan ");
  __bad_toml_inline("-naan ");
  __bad_toml_inline("+innfs ");
  __bad_toml_inline("-innfs ");
  // __bad_toml_inline("++naan ");
  // __bad_toml_inline("--naan ");
  // __bad_toml_inline("++innfs ");
  // __bad_toml_inline("--innfs ");

  // __bad_toml("[abc def]");
  // __bad_toml("[[abc def]]");
  __bad_toml("[abc] def true");
  __bad_toml("[abc] def =");
  __bad_toml("[[abc]] def =");
  __bad_toml("a=b, c=d");
}
