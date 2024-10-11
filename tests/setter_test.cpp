// Copyright (c) 2024 Zhichen (Joshua) Wen
#include <gtest/gtest.h>

#include <string>

#include "./test-utils.h"
#include "miniJSON/miniJSON.h"

TEST(SetterTest, NULL) {
  {
    auto json = miniJSON::parse("true");
    json = nullptr;
    EXPECT_EQ(json.get_type(), miniJSON::json_value_type::null);
  }
}

TEST(SetterTest, Boolean) {
  {
    auto json = miniJSON::parse("true");
    json = false;
    EXPECT_EQ(json.get_boolean(), false);
  }
}

TEST(SetterTest, Number) {
  {
    auto json = miniJSON::parse(R"(123)");
    json = 8929;
    EXPECT_EQ(json.get_integer(), 8929);
  }
  {
    auto json = miniJSON::parse(R"(67.12)");
    json = 1242.12;
    EXPECT_TRUE(double_equal(json.get_double(), 1242.12));
  }
}

TEST(SetterTest, String) {
  {
    auto json = miniJSON::parse(R"("abc")");
    json = "def";
    EXPECT_EQ(json.get_string(), std::string{"def"});
  }
  {
    auto json = miniJSON::parse(R"("abc")");
    std::string s{"def"};
    json = s;
    EXPECT_EQ(json.get_string(), std::string{"def"});
  }
}

TEST(SetterTest, Object) {
  {
    auto json = miniJSON::parse(R"({"name": "Alicia"})");
    json["name"] = "David";
    json["age"] = 32;
    EXPECT_EQ(json["name"].get_string(), std::string{"David"});
    EXPECT_EQ(json["age"].get_integer(), 32);
    EXPECT_EQ(json.to_string(), R"({"name":"David","age":32})");
    json = nullptr;
    EXPECT_EQ(json.to_string(), R"(null)");
  }
  {
    // initializer_list
    miniJSON::json_node json = {{"name", "Alicia"}, {"age", 32}};
    EXPECT_EQ(json.to_string(), R"({"name":"Alicia","age":32})");
  }
  {
    // initializer_list: mixed with array
    miniJSON::json_node json = {
        {"name", "Alicia"}, {"age", 32}, {"friends", {"David", "Michael"}}};
    EXPECT_EQ(json.to_string(),
              R"({"name":"Alicia","age":32,"friends":["David","Michael"]})");
  }
}

TEST(SetterTest, Array) {
  {
    auto json = miniJSON::parse(R"({"friends": ["Alicia", "David"]})");
    json["friends"][1] = "John";
    json["friends"][2] = "David";
    EXPECT_EQ(json["friends"][1].get_string(), std::string{"John"});
    EXPECT_EQ(json["friends"][2].get_string(), std::string{"David"});
    EXPECT_EQ(json.to_string(), R"({"friends":["Alicia","John","David"]})");
    json["friends"] = nullptr;
    EXPECT_EQ(json["friends"].to_string(), R"(null)");
  }
  {
    // initializer_list: one dimensional
    miniJSON::json_node json = {"volleyball", 1, nullptr, 1.2, false};
    EXPECT_EQ(json.to_string(), R"(["volleyball",1,null,1.2,false])");
  }
  {
    // initializer_list: two dimensional
    miniJSON::json_node json = {{1, "abc", false}, {nullptr, 4, true}};
    EXPECT_EQ(json.to_string(), R"([[1,"abc",false],[null,4,true]])");
  }
  {
    // initializer_list: mixed with object
    miniJSON::json_node json = {{1, "abc", false},
                                {nullptr, 4, true, {{"a", 1}}}};
    EXPECT_EQ(json.to_string(), R"([[1,"abc",false],[null,4,true,{"a":1}]])");
  }
}
