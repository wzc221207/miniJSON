// Copyright (c) 2024 Zhichen (Joshua) Wen
#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "miniJSON/miniJSON.h"

TEST(RandomDataParseTest, Unformatted) {
  std::vector<std::string> filenames = {
      "../tests/data/random-parse-data-1.json",
      "../tests/data/random-parse-data-2.json"};
  for (auto &filename : filenames) {
    std::ifstream in_file(filename);
    std::stringstream ss;
    ss << in_file.rdbuf();
    auto json = miniJSON::parse(ss.str());
    auto json_str = json.to_string();
    EXPECT_EQ(json_str, ss.str());
  }
}
