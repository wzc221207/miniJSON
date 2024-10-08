// Copyright (c) 2024 Zhichen (Joshua) Wen
#include <gtest/gtest.h>

#include <string>

#include "./test-utils.h"
#include "miniJSON/miniJSON.h"

TEST(GetterTest, Boolean) {
  {
    auto json = miniJSON::parse("true");
    EXPECT_EQ(json.get_boolean(), true);
  }
  {
    auto json = miniJSON::parse("false");
    EXPECT_EQ(json.get_boolean(), false);
  }
  {
    // trying to access unmatched data type value
    EXPECT_THROW(
        {
          auto json = miniJSON::parse("false");
          json.get_integer();
        },
        miniJSON::json_type_error);
  }
}

TEST(GetterTest, String) {
  {
    auto json = miniJSON::parse(R"("abc")");
    EXPECT_EQ(json.get_string(), std::string{"abc"});
  }
  {
    // trying to access unmatched data type value
    EXPECT_THROW(
        {
          auto json = miniJSON::parse(R"("sgdf")");
          json.get_boolean();
        },
        miniJSON::json_type_error);
  }
}

TEST(GetterTest, Number) {
  {
    auto json = miniJSON::parse(R"(123)");
    EXPECT_EQ(json.get_integer(), 123);
  }
  {
    auto json = miniJSON::parse(R"(67.12)");
    EXPECT_TRUE(double_equal(json.get_double(), 67.12));
  }
  {
    // trying to access unmatched data type value
    EXPECT_THROW(
        {
          auto json = miniJSON::parse("1390");
          json.get_string();
        },
        miniJSON::json_type_error);
  }
}

TEST(GetterTest, Object) {
  {
    auto json = miniJSON::parse(R"({"name": "Alicia"})");
    EXPECT_EQ(json["name"].get_string(), std::string{"Alicia"});
  }
  {
    // trying to access unmatched data type value
    EXPECT_THROW(
        {
          auto json = miniJSON::parse(R"({"name": "Alicia"})");
          json.get_string();
        },
        miniJSON::json_type_error);
  }
  {
    // non-existent key
    auto json = miniJSON::parse(R"({"name": "Alicia"})");
    EXPECT_EQ(json["name1"].get_type(),
              miniJSON::json_value_type::indeterminate);
    EXPECT_THROW({ json.to_string(); }, miniJSON::json_type_error);
  }
}

TEST(GetterTest, Array) {
  {
    auto json = miniJSON::parse(R"({"friends": ["Alicia", "David"]})");
    EXPECT_EQ(json["friends"][0].get_string(), std::string{"Alicia"});
    EXPECT_EQ(json["friends"][1].get_string(), std::string{"David"});
  }
  {
    // trying to access unmatched data type value
    EXPECT_THROW(
        {
          auto json = miniJSON::parse(R"({"friends": ["Alicia", "David"]})");
          json.get_string();
        },
        miniJSON::json_type_error);
  }
  {
    // invalid index
    auto json = miniJSON::parse(R"({"friends": ["Alicia", "David"]})");
    EXPECT_EQ(json["friends"][2].get_type(),
              miniJSON::json_value_type::indeterminate);
    EXPECT_THROW({ json.to_string(); }, miniJSON::json_type_error);
  }
}
