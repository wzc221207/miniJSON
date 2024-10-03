// Copyright (c) 2024 Zhichen (Joshua) Wen
#pragma once

#include <exception>
#include <string>

namespace miniJSON {
/*
  This is used to denote JSON parsing error.
*/
class json_parse_error : public std::exception {
 public:
  explicit json_parse_error(std::string message)
      : m_message(template_str + message) {}
  const char *what() const noexcept override { return m_message.c_str(); }

 private:
  std::string template_str = "json parse error: ";
  std::string m_message;
};
}  // namespace miniJSON
