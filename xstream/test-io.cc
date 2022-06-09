#include "xstream"
#include <gtest/gtest.h>
#include <string>

TEST(xstream, IO) {
  std::string xfilename = "test-xstream.xdat";

  std::vector<std::vector<std::string>> ovstr = {{"abcd", "alwkjlkanboejlakw"},
                                                 {"awlkjjlkbjlkjawe", "lkwe"},
                                                 {"lakwjlklkbw", "alwkjlkbk"}};
  std::vector<double> ovdbl = {1.2, -3.4, 5.6, 7.8};
  std::valarray<long> ovint = {1, 3, -5, 7};

  decltype(ovstr) ivstr;
  decltype(ovdbl) ivdbl;
  decltype(ovint) ivint;

  yuc::oxstream(xfilename) << ovstr << ovdbl << ovint;
  yuc::ixstream(xfilename) >> ivstr >> ivdbl >> ivint;

  EXPECT_EQ(ovstr.size(), ivstr.size());
  for (size_t i = 0; i < ovstr.size(); ++i) {
    EXPECT_EQ(ovstr[i].size(), ivstr[i].size());
    for (size_t j = 0; j < ovstr[i].size(); ++j) {
      EXPECT_TRUE(ovstr[i][j] == ivstr[i][j])
          << i << " " << j << " " << ovstr[i][j] << " " << ivstr[i][j]
          << std::endl;
    }
  }

  EXPECT_EQ(ovdbl.size(), ivdbl.size());
  for (size_t i = 0; i < ovdbl.size(); ++i) {
    EXPECT_EQ(ovdbl[i], ivdbl[i]);
  }

  EXPECT_EQ(ovint.size(), ivint.size());
  for (size_t i = 0; i < ovint.size(); ++i) {
    EXPECT_EQ(ovint[i], ivint[i]);
  }
}
