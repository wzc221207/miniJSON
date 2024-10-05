// Copyright (c) 2024 Zhichen (Joshua) Wen
#include <gtest/gtest.h>

#include "miniJSON/miniJSON.h"

TEST(BasicParseTest, Empty) {
  {
    // empty string
    EXPECT_THROW(
        { auto json = miniJSON::parse(""); }, miniJSON::json_parse_error);
  }
  {
    // string with whitespace
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
    // incomplete true
    EXPECT_THROW(
        { auto json = miniJSON::parse("tru"); }, miniJSON::json_parse_error);
  }
  {
    // incomplete false
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
    // invalid character
    EXPECT_THROW(
        { auto json = miniJSON::parse(" null ."); },
        miniJSON::json_parse_error);
  }
}

TEST(BasicParseTest, Array) {
  {
    auto json = miniJSON::parse(
        R"([true, null, "sdljf", 12, [false, null, 300, "asdim", 116e20]])");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str,
              R"([true,null,"sdljf",12,[false,null,300,"asdim",1.16e+22]])");
  }
  {
    // string not enclosed with quotation mark
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
    // invalid escape sequence
    EXPECT_THROW(
        { auto json = miniJSON::parse(R"("abc\xdef")"); },
        miniJSON::json_parse_error);
  }
}

TEST(BasicParseTest, Object) {
  {
    auto json = miniJSON::parse(R"({"name": "Alicia"})");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"({"name":"Alicia"})");
  }
  {
    // insertion order should be maintained
    auto json = miniJSON::parse(R"({"name": "Alicia" , "likes": ["tennis",
        "sushi"]})");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"({"name":"Alicia","likes":["tennis","sushi"]})");
  }
  {
    // nested object
    auto json = miniJSON::parse(
        R"({"name": "Alicia", "height": 1.68, "likes": ["tennis", "sushi", null], "friends":
        [{"name": "David", "male": true}]})");
    auto json_str = json.to_string();
    EXPECT_EQ(
        json_str,
        R"({"name":"Alicia","height":1.68,"likes":["tennis","sushi",null],"friends":[{"name":"David","male":true}]})");
  }
  {
    // key not enclosed by quotation mark
    EXPECT_THROW(
        {
          auto json = miniJSON::parse(
              R"({"name": "Alicia", "likes": ["tennis", "sushi", null],
              friends: [{"name": "David", "male": true}]})");
        },
        miniJSON::json_parse_error);
  }
  {
    // no colon after key
    EXPECT_THROW(
        {
          auto json = miniJSON::parse(
              R"({"name": "Alicia", "likes": ["tennis", "sushi", null],
              "friends" [{"name": "David", "male": true}]})");
        },
        miniJSON::json_parse_error);
  }
  {
    // no comma between key value pairs
    EXPECT_THROW(
        {
          auto json = miniJSON::parse(
              R"({"name": "Alicia" "likes": ["tennis", "sushi", null],
              "friends": [{"name": "David", "male": true}]})");
        },
        miniJSON::json_parse_error);
  }
  {
    // does not start with {
    EXPECT_THROW(
        {
          auto json = miniJSON::parse(
              R"("name": "Alicia" "likes": ["tennis", "sushi", null],
              "friends": [{"name": "David", "male": true}]})");
        },
        miniJSON::json_parse_error);
  }
  {
    // does not end with }
    EXPECT_THROW(
        {
          auto json = miniJSON::parse(
              R"({"name": "Alicia" "likes": ["tennis", "sushi", null],
              "friends": [{"name": "David", "male": true}])");
        },
        miniJSON::json_parse_error);
  }
}

TEST(BasicParseTest, Number) {
  {
    auto json = miniJSON::parse(R"(1)");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"(1)");
  }
  {
    auto json = miniJSON::parse(R"(123)");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"(123)");
  }
  {
    // negative value
    auto json = miniJSON::parse(R"(-456213)");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"(-456213)");
  }
  {
    // double value
    auto json = miniJSON::parse(R"(-456.213)");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"(-456.213)");
  }
  {
    // double with exponent
    auto json = miniJSON::parse(R"(2.18e+20)");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"(2.18e+20)");
  }
  {
    // double with exponent
    auto json = miniJSON::parse(R"(-2.18E20)");
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, R"(-2.18e+20)");
  }
  {
    // invalid: two minus signs
    EXPECT_THROW(
        { auto json = miniJSON::parse(R"(--1.2)"); },
        miniJSON::json_parse_error);
  }
  {
    // invalid: two dots
    EXPECT_THROW(
        { auto json = miniJSON::parse(R"(-1..2)"); },
        miniJSON::json_parse_error);
  }
  {
    // invalid: no digit after the dot
    EXPECT_THROW(
        { auto json = miniJSON::parse(R"(-1.)"); }, miniJSON::json_parse_error);
  }
}
