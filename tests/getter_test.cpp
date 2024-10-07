// Copyright (c) 2024 Zhichen (Joshua) Wen
#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <string>

#include "miniJSON/miniJSON.h"

bool double_equal(double a, double b) {
  return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
}

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
    EXPECT_THROW(
        {
          auto json = miniJSON::parse(R"({"name": "Alicia"})");
          json["name1"];
        },
        std::out_of_range);
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
    EXPECT_THROW(
        {
          auto json = miniJSON::parse(R"({"friends": ["Alicia", "David"]})");
          json["friends"][2];
        },
        std::out_of_range);
  }
}
