#include "config"
#include "gtest/gtest.h"
#include <sstream>

using namespace yuc;

TEST(config, string_escape) {
#define __escape_test(in, out)                                                 \
  EXPECT_EQ(__config_detail::string_escape(in), out)
  __escape_test("abc", "abc");
  __escape_test("abc\\", "abc\\\\");
  __escape_test("\abc\\", "\\abc\\\\");
  __escape_test("\"abc\\", "\\\"abc\\\\");
  __escape_test("\tabc\\", "\\tabc\\\\");
  __escape_test("a\\bc\\", "a\\\\bc\\\\");
  __escape_test("a\\\bc\\", "a\\\\\\bc\\\\");
  __escape_test("a\"b\"c", "a\\\"b\\\"c");
  __escape_test("ab\\c", "ab\\\\c");

  __escape_test("[\\]", "[\\\\]");
  __escape_test("[\"]", "[\\\"]");
  __escape_test("[\a]", "[\\a]");
  __escape_test("[\b]", "[\\b]");
  __escape_test("[\f]", "[\\f]");
  __escape_test("[\n]", "[\\n]");
  __escape_test("[\r]", "[\\r]");
  __escape_test("[\t]", "[\\t]");
  __escape_test("[\v]", "[\\v]");

  __escape_test("[\3]", "[\\x03]");
#undef __escape_test
}

TEST(config, string_unescape_expand) {
  std::map<std::string, std::string> d = {{"key1", "val1"},
                                          {"key2", "val2"},
                                          {"keyn", "val\\n"}};
  std::string nonexist_env = "lIbYuC_nOnExIsT_eNv";
  while (std::getenv(nonexist_env.c_str())) {
      nonexist_env += '_';
  }
#define __expand_test(in, out)                                                 \
  EXPECT_EQ(__config_detail::string_unescape(in, d), out)
  __expand_test("abc", "abc");
  __expand_test("$(key1)", "val1");
  __expand_test("abc\\$(key1)", "abc$(key1)");
  __expand_test("$(key1)$(key2)", "val1val2");
  __expand_test("($(key1)$(key2)$(keyn))", "(val1val2val\\n)");

  __expand_test("${HOME}", std::getenv("HOME"));
  __expand_test("[${" + nonexist_env + "}]", "[]");
#undef __expand_test
}

TEST(config, string_unescape) {
#define __unescape_test(in, out)                                               \
  EXPECT_EQ(__config_detail::string_unescape(in), out)
  __unescape_test("abc", "abc");
  __unescape_test("abc\\", "abc\\");
  __unescape_test("\\abc\\", "\abc\\");
  __unescape_test("\\\"abc\\", "\"abc\\");
  __unescape_test("\\tabc\\", "\tabc\\");
  __unescape_test("a\\\\bc\\", "a\\bc\\");
  __unescape_test("a\\\\\\bc\\", "a\\\bc\\");
  __unescape_test("a\\\"b\\\"c", "a\"b\"c");
  __unescape_test("ab\\c", "ab\\c");

  __unescape_test("[\\\\]", "[\\]");
  __unescape_test("[\\\"]", "[\"]");
  __unescape_test("[\\a]", "[\a]");
  __unescape_test("[\\b]", "[\b]");
  __unescape_test("[\\f]", "[\f]");
  __unescape_test("[\\n]", "[\n]");
  __unescape_test("[\\r]", "[\r]");
  __unescape_test("[\\t]", "[\t]");
  __unescape_test("[\\v]", "[\v]");
  __unescape_test("[\\y]", "[\\y]");
  __unescape_test("[\\z]", "[\\z]");
#undef __unescape_test
}

TEST(config, read_qouted) {
  using namespace std::string_literals;
  std::stringstream ss;
#define __read_quoted_test(in, out, nextc)                                     \
  ss.clear(), ss.str(in);                                                      \
  EXPECT_EQ(__config_detail::read_quoted(ss), out);                            \
  EXPECT_EQ(ss.peek(), nextc);
  __read_quoted_test("\"@", "", '@');
  __read_quoted_test("abcdef\"@", "abcdef", '@');
  __read_quoted_test("abc\"@def", "abc", '@');
  __read_quoted_test("abc\\\"def\"@", "abc\\\"def", '@');
#undef __get_qouted_test
}

TEST(config, read_word) {
  std::stringstream ss;
#define __read_word_test(in, out, nextc, ...)                                  \
  ss.clear(), ss.str(in);                                                      \
  EXPECT_EQ(__config_detail::read_word(ss, __VA_ARGS__), out);   \
  EXPECT_EQ(ss.peek(), nextc);
  __read_word_test("", "", EOF, EOF);
  __read_word_test("abc ", "abc", ' ', EOF);
  __read_word_test("abc. ", "abc.", ' ', EOF);
  __read_word_test("abc. ", "abc", '.', '.');
  __read_word_test(R"~~(a"b \"c.d"e.f)~~", R"~~(ab "c.de.f)~~", EOF, EOF);
  __read_word_test("a\"b \\\"c.d\"e.f", "ab \"c.de.f", EOF, EOF);
  __read_word_test("a\"b \\\"c.d\"e.f", "ab \"c.de", '.', '.');
  __read_word_test("a'b \\\"c\\'d e\"f", "ab \\\"c\\d", ' ', EOF);
  __read_word_test("a\"b \\\"c\\\"d e\"f ", "ab \"c\"d ef", ' ', EOF);
#undef __read_word_test
}

TEST(config, string_split) {
  using __config_detail::string_split;
  std::vector<std::string_view> vvec;
  std::vector<std::string> svec;
#define __expect_vector_eq()                                                   \
  EXPECT_EQ(svec.size(), vvec.size());                                         \
  if (svec.size() == vvec.size()) {                                            \
    for (size_t ivec = 0; ivec < vvec.size(); ++ivec) {                        \
      EXPECT_EQ(svec[ivec], vvec[ivec]);                                       \
    }                                                                          \
  }

  vvec = string_split(""), svec = {};
  __expect_vector_eq();

  vvec = string_split("  "), svec = {};
  __expect_vector_eq();

  vvec = string_split("  ", "", false), svec = {"", "", ""};
  __expect_vector_eq();

  vvec = string_split("abcdefgh");
  svec = {"abcdefgh"};
  __expect_vector_eq();

  vvec = string_split(" abcdefgh");
  svec = {"abcdefgh"};
  __expect_vector_eq();

  vvec = string_split(" abcdefgh", "", false);
  svec = {"", "abcdefgh"};
  __expect_vector_eq();

  vvec = string_split("ab cd  ef\tgh");
  svec = {"ab", "cd", "ef", "gh"};
  __expect_vector_eq();

  vvec = string_split("ab cd  ef\tgh ", "", false);
  svec = {"ab", "cd", "", "ef", "gh", ""};
  __expect_vector_eq();

  vvec = string_split("sep", "sep"), svec = {};
  __expect_vector_eq();

  vvec = string_split("sep", "sep", false), svec = {"", ""};
  __expect_vector_eq();

  // string_view may not be zero terminated
  std::string str = "ab:::cd::::ef::gh::";
  std::string_view sv(str.data(), str.size() - 1);

  vvec = string_split(str, "::");
  svec = {"ab", ":cd", "ef", "gh"};
  __expect_vector_eq();

  vvec = string_split(sv, "::");
  svec = {"ab", ":cd", "ef", "gh:"};
  __expect_vector_eq();

  vvec = string_split(str, "::", false);
  svec = {"ab", ":cd", "", "ef", "gh", ""};
  __expect_vector_eq();

  vvec = string_split(sv, "::", false);
  svec = {"ab", ":cd", "", "ef", "gh:"};
  __expect_vector_eq();
#undef __expect_vector_eq
}

TEST(config, stream_trim) {
  using __config_detail::stream_trim;
  std::stringstream ss;

  ss.clear();
  ss.str("@abc"), stream_trim(ss, "#");
  EXPECT_TRUE(ss.good());
  EXPECT_EQ(ss.peek(), '@');
  EXPECT_EQ(ss.tellg(), 0);

  ss.clear();
  ss.str("#@abc"), stream_trim(ss, "#");
  EXPECT_TRUE(ss.eof());
  EXPECT_EQ(ss.tellg(), -1);

  ss.clear();
  ss.str("\t \n\t@abc"), stream_trim(ss, "#");
  EXPECT_TRUE(ss.good());
  EXPECT_EQ(ss.peek(), '@');
  EXPECT_EQ(ss.tellg(), 4);

  ss.clear();
  ss.str("\t #def \n    #ghi\n\t@abc"), stream_trim(ss, "#");
  EXPECT_TRUE(ss.good());
  EXPECT_EQ(ss.peek(), '@');
  EXPECT_EQ(ss.tellg(), 18);

  ss.clear();
  ss.str("  // abc\n\t\n  /"), stream_trim(ss, "//");
  EXPECT_TRUE(ss.good());
  EXPECT_EQ(ss.peek(), '/');
  EXPECT_EQ(ss.tellg(), 13);
}

TEST(config, string_trim) {
#define __string_trim_test(dir, in, out)                                       \
  EXPECT_EQ(__config_detail::string_trim(in, dir), out)
  __string_trim_test(-1, "", "");
  __string_trim_test(+0, "", "");
  __string_trim_test(+1, "", "");

  __string_trim_test(-1, " \t \n ", "");
  __string_trim_test(+0, " \t \n ", "");
  __string_trim_test(+1, " \t \n ", "");

  __string_trim_test(-1, "ab\tc", "ab\tc");
  __string_trim_test(+0, "ab\tc", "ab\tc");
  __string_trim_test(+1, "ab\tc", "ab\tc");

  __string_trim_test(-1, " \t \n ab\tc", " \t \n ab\tc");
  __string_trim_test(+0, " \t \n ab\tc", "ab\tc");
  __string_trim_test(+1, " \t \n ab\tc", "ab\tc");

  __string_trim_test(-1, "a\nbc \t \n ", "a\nbc");
  __string_trim_test(+0, "a\nbc \t \n ", "a\nbc");
  __string_trim_test(+1, "a\nbc \t \n ", "a\nbc \t \n ");

  __string_trim_test(-1, "\nab\tc \t \n ", "\nab\tc");
  __string_trim_test(+0, "\nab\tc \t \n ", "ab\tc");
  __string_trim_test(+1, "\nab\tc \t \n ", "ab\tc \t \n ");
#undef __string_trim_test
}
