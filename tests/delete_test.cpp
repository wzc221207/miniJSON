// Copyright (c) 2024 Zhichen (Joshua) Wen
#include <gtest/gtest.h>

#include <string>

#include "miniJSON/miniJSON.h"

TEST(DeleteTest, Object) {
  {
    miniJSON::json_node json = {{"name", "Alicia"}, {"age", 32}};
    EXPECT_EQ(json.to_string(), R"({"name":"Alicia","age":32})");
    json.erase("age");
    EXPECT_EQ(json.to_string(), R"({"name":"Alicia"})");
    json.erase("name");
    EXPECT_EQ(json.to_string(), R"({})");
  }
}

TEST(DeleteTest, Array) {
  {
    miniJSON::json_node json = {"volleyball", 1, nullptr, 1.2, false};
    EXPECT_EQ(json.to_string(), R"(["volleyball",1,null,1.2,false])");
    json.erase(2);
    EXPECT_EQ(json.to_string(), R"(["volleyball",1,1.2,false])");
    json.erase(3);
    EXPECT_EQ(json.to_string(), R"(["volleyball",1,1.2])");
    EXPECT_THROW({ json.erase(3); }, std::out_of_range);
  }
}
