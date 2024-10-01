// Copyright (c) 2024 Zhichen (Joshua) Wen
#include <gtest/gtest.h>

#include "miniJSON/miniJSON.h"

TEST(BasicParseTest, Boolean) {
  {
    auto json = miniJSON::parse("true");
    EXPECT_EQ(*json.get_boolean(), true);
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, "true");
  }
  {
    auto json = miniJSON::parse("false");
    EXPECT_EQ(*json.get_boolean(), false);
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, "false");
  }
}

TEST(BasicParseTest, Null) {
  {
    auto json = miniJSON::parse(" null ");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, "null");
  }
}

TEST(BasicParseTest, Array) {
  {
    auto json = miniJSON::parse(R"([true, null, [false, null]])");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"([true,null,[false,null]])");
  }
}