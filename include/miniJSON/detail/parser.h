// Copyright (c) 2024 Zhichen (Joshua) Wen
#pragma once
#include <string>
#include <utility>
#include <vector>

namespace miniJSON {
namespace detail {
template <typename JSONNode>
/*
  Parser is responsible for parsing the JSON string and constructing the JSON
  nodes (tree).
*/
class parser {
 public:
  parser(JSONNode *result, const std::string &s)
      : m_result(result), m_json_s(s) {}

 public:
  void parse() { parse_value(m_result); }

 private:
  enum class json_parse_type {
    error,
    json_value,
    whitespace,
    reach_array_end,
    reach_array_separator
  };

  json_parse_type parse_array(JSONNode *result) {
    result->set_array();
    while (m_parse_index < m_json_s.length()) {
      JSONNode *j = new JSONNode();
      auto parse_type = parse_value(j);
      if (parse_type == json_parse_type::json_value) {
        result->m_value.array->push_back(j);
        continue;
      }
      delete j;  // delete if haven't parsed a valid json value
      if (parse_type == json_parse_type::reach_array_end) {
        break;
      } else if (parse_type == json_parse_type::reach_array_separator) {
        continue;
      }
    }
    return json_parse_type::json_value;
  }
  /*
    Main parsing logic
  */
  json_parse_type parse_value(JSONNode *result) {
    if (remaining_parse_length() >= 1 &&
        (m_json_s[m_parse_index] == ' ' || m_json_s[m_parse_index] == '\n' ||
         m_json_s[m_parse_index] == '\r' || m_json_s[m_parse_index] == '\t')) {
      m_parse_index++;
      return json_parse_type::whitespace;
    } else if (remaining_parse_length() >= 4 &&
               parse_n_and_compare(4, "true")) {
      result->set_boolean(true);
      m_parse_index += 4;
      return json_parse_type::json_value;
    } else if (remaining_parse_length() >= 5 &&
               parse_n_and_compare(5, "false")) {
      result->set_boolean(false);
      m_parse_index += 5;
      return json_parse_type::json_value;
    } else if (remaining_parse_length() >= 4 &&
               parse_n_and_compare(4, "null")) {
      m_parse_index += 4;
      return json_parse_type::json_value;
    } else if (remaining_parse_length() > 0 && m_json_s[m_parse_index] == '[') {
      m_parse_index++;
      return parse_array(result);
    } else if (remaining_parse_length() > 0 && m_json_s[m_parse_index] == ']') {
      m_parse_index++;
      return json_parse_type::reach_array_end;
    } else if (remaining_parse_length() > 0 && m_json_s[m_parse_index] == ',') {
      m_parse_index++;
      return json_parse_type::reach_array_separator;
    }

    return json_parse_type::error;
  }

 private:
  size_t remaining_parse_length() { return m_json_s.length() - m_parse_index; }
  bool parse_n_and_compare(size_t n, const char *s) {
    return strcmp(m_json_s.substr(m_parse_index, n).c_str(), s) == 0;
  }

 private:
  JSONNode *m_result;
  const std::string &m_json_s;
  unsigned int m_parse_index = 0;
};
}  // namespace detail
}  // namespace miniJSON
