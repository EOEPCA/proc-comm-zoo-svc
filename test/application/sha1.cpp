#include <memory>
#include <iostream>
#include <utils/sha1.hpp>

#include "gtest/gtest.h"

TEST(sha1, t01) {
  std::string testString("the string");
  auto out = sha1::parseString(testString);
  std::cout << out << "\n";
  EXPECT_EQ(out, "e8cc75db52457e014d354b54a2c44c30dd96cbd5");
}
