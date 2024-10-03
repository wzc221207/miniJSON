// Copyright (c) 2024 Zhichen (Joshua) Wen
#include <gtest/gtest.h>

#include "miniJSON/miniJSON.h"

TEST(BasicParseTest, Empty) {
  {
    EXPECT_THROW(
        { auto json = miniJSON::parse("  "); }, miniJSON::json_parse_error);
  }
}

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
  {
    EXPECT_THROW(
        { auto json = miniJSON::parse("tru"); }, miniJSON::json_parse_error);
  }
  {
    EXPECT_THROW(
        { auto json = miniJSON::parse("fals"); }, miniJSON::json_parse_error);
  }
}

TEST(BasicParseTest, Null) {
  {
    auto json = miniJSON::parse(" null ");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, "null");
  }
  {
    EXPECT_THROW(
        { auto json = miniJSON::parse(" null ."); },
        miniJSON::json_parse_error);
  }
}

TEST(BasicParseTest, Array) {
  {
    auto json =
        miniJSON::parse(R"([true, null, "sdljf", [false, null, "asdim"]])");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"([true,null,"sdljf",[false,null,"asdim"]])");
  }
  {
    EXPECT_THROW(
        {
          auto json = miniJSON::parse(
              R"([true, null, "sdljf", [false, null, "asdim]])");
        },
        miniJSON::json_parse_error);
  }
}

TEST(BasicParseTest, String) {
  {
    auto json = miniJSON::parse(R"("abc")");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"("abc")");
  }
  {
    auto json = miniJSON::parse(R"("  a bc  d")");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"("  a bc  d")");
  }
  {
    auto json = miniJSON::parse(R"("abc\"def")");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"("abc\"def")");
  }
  {
    auto json = miniJSON::parse(R"("abc\\def")");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"("abc\\def")");
  }
  {
    auto json = miniJSON::parse(R"("abc\/def")");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"("abc/def")");
  }
  {
    auto json = miniJSON::parse(R"("abc\fdef")");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"("abc\fdef")");
  }
  {
    auto json = miniJSON::parse(R"("abc\ndef")");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"("abc\ndef")");
  }
  {
    auto json = miniJSON::parse(R"("abc\rdef")");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"("abc\rdef")");
  }
  {
    auto json = miniJSON::parse(R"("abc\tdef")");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"("abc\tdef")");
  }
  {
    EXPECT_THROW(
        { auto json = miniJSON::parse(R"("abc\xdef")"); },
        miniJSON::json_parse_error);
  }
}
