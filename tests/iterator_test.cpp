// Copyright (c) 2024 Zhichen (Joshua) Wen
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "miniJSON/miniJSON.h"

TEST(IteratorTest, Array) {
  {
    // array
    auto json = miniJSON::parse("[1,2,3]");
    std::vector<int> res;
    std::vector<int> expected{1, 2, 3};
    for (auto &j : json) {
      res.push_back(j.get_integer());
    }
    EXPECT_TRUE(res == expected);
  }
}

TEST(IteratorTest, Object) {
  {
    // object values
    auto json = miniJSON::parse(R"({"a":1,"b":2,"c":3})");
    std::vector<int> res1;
    std::vector<int> res2;
    std::vector<int> expected{1, 2, 3};
    for (auto j : json) {
      res1.push_back(j.get_integer());
    }
    EXPECT_TRUE(res1 == expected);
    for (auto it = json.begin(); it != json.end(); it++) {
      res2.push_back(it.value()->get_integer());
    }
    EXPECT_TRUE(res1 == expected);
  }
  {
    // object keys
    auto json = miniJSON::parse(R"({"a":1,"b":2,"c":3})");
    std::vector<std::string> res;
    std::vector<std::string> expected{"a", "b", "c"};
    for (auto it = json.begin(); it != json.end(); it++) {
      res.push_back(it.key());
    }
    EXPECT_TRUE(res == expected);
  }
}
