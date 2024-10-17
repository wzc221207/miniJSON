// Copyright (c) 2024 Zhichen (Joshua) Wen
#pragma once
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

#include "../errors.h"

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
  /*
    Parse JSON string. It will throw error if the JSON string format is invalid.
  */
  void parse() {
    auto type = parse_value(m_result);
    if (type != json_parse_type::json_value || remaining_parse_length() > 0) {
      throw json_parse_error("invalid JSON string format");
    }
  }

 private:
  enum class json_parse_type {
    error,
    json_value,
    whitespace,
    reach_array_end,
    reach_array_separator
  };

  json_parse_type parse_array_item(JSONNode *array) {
    parse_whitespace();
    JSONNode *value = new JSONNode();
    if (parse_value(value) != json_parse_type::json_value) {
      delete value;
      return json_parse_type::error;
    }
    array->m_value.array->push_back(value);
    parse_whitespace();
    if (!parse_comma(true)) {
      return json_parse_type::error;
    }
    return json_parse_type::json_value;
  }

  json_parse_type parse_array(JSONNode *result) {
    result->set_array();
    while (remaining_parse_length() > 0) {
      if (current_character() == ']') {
        m_parse_index++;
        break;
      }
      if (parse_array_item(result) != json_parse_type::json_value) {
        return json_parse_type::error;
      }
    }
    return json_parse_type::json_value;
  }

  json_parse_type parse_object_item(JSONNode *object) {
    parse_whitespace();
    if (current_character() != '\"') {
      return json_parse_type::error;
    }
    m_parse_index++;

    JSONNode *key = new JSONNode();
    if (parse_string(key) != json_parse_type::json_value) {
      delete key;
      return json_parse_type::error;
    }

    parse_whitespace();
    if (current_character() != ':') {
      delete key;
      return json_parse_type::error;
    }
    m_parse_index++;
    parse_whitespace();

    JSONNode *value = new JSONNode();
    if (parse_value(value) != json_parse_type::json_value) {
      delete key;
      delete value;
      return json_parse_type::error;
    }

    (*object->m_value.object)[*key->m_value.str] = value;
    delete key;  // object key string will be copied so this dynamically
                 // allocated string json_node can be deleted
    parse_whitespace();

    if (!parse_comma()) {
      return json_parse_type::error;
    }
    return json_parse_type::json_value;
  }

  json_parse_type parse_object(JSONNode *result) {
    result->set_object();
    while (remaining_parse_length() > 0) {
      if (current_character() == '}') {
        m_parse_index++;
        break;
      }
      if (parse_object_item(result) != json_parse_type::json_value) {
        return json_parse_type::error;
      }
    }
    return json_parse_type::json_value;
  }

  bool parse_escape_sequence(JSONNode *result) {
    if (remaining_parse_length() == 0) {
      return false;
    }
    bool valid_sequence = true;
    switch (current_character()) {
      case '"':
        *result->m_value.str += R"(\")";
        break;
      case '\\':
        *result->m_value.str += R"(\\)";
        break;
      case '/':
        *result->m_value.str += R"(/)";
        break;
      case 'b':
        *result->m_value.str += R"(\b)";
        break;
      case 'f':
        *result->m_value.str += R"(\f)";
        break;
      case 'n':
        *result->m_value.str += R"(\n)";
        break;
      case 'r':
        *result->m_value.str += R"(\r)";
        break;
      case 't':
        *result->m_value.str += R"(\t)";
        break;
      default:
        valid_sequence = false;
        break;
    }
    if (valid_sequence) {
      m_parse_index++;
    }
    return valid_sequence;
  }

  json_parse_type parse_string(JSONNode *result) {
    result->set_string();
    bool string_closed = false;
    while (remaining_parse_length() > 0) {
      if (parse_and_compare('\"')) {
        m_parse_index++;
        string_closed = true;
        break;
      }
      if (parse_and_compare('\\')) {
        m_parse_index++;
        if (parse_escape_sequence(result)) {
          continue;
        }
        return json_parse_type::error;
      }
      *result->m_value.str += current_character();
      m_parse_index++;
    }
    return string_closed ? json_parse_type::json_value : json_parse_type::error;
  }

  void parse_whitespace() {
    while (remaining_parse_length() >= 1 && isspace(current_character())) {
      m_parse_index++;
    }
  }

  bool parse_comma(bool is_array = false) {
    if (current_character() == ',') {
      m_parse_index++;
      return true;
    }
    int idx = m_parse_index;
    if (!is_array && m_json_s[idx] == '}') {
      return true;
    }
    if (is_array && m_json_s[idx] == ']') {
      return true;
    }
    return false;
  }

  json_parse_type parse_number(JSONNode *result) {
    std::string number_s;
    while (remaining_parse_length() > 0) {
      if (isdigit(current_character()) || current_character() == '-' ||
          current_character() == '+' || current_character() == 'e' ||
          current_character() == 'E' || current_character() == '.') {
        number_s += current_character();
        m_parse_index++;
      } else {
        break;
      }
    }

    // check if number is valid
    char *end_ptr = nullptr;
    double d = strtod(number_s.c_str(), &end_ptr);
    if (strlen(end_ptr) > 0) {
      return json_parse_type::error;
    }
    // must hvae digits after dot
    if (number_s[number_s.length() - 1] == '.') {
      return json_parse_type::error;
    }
    // check it's double or integer
    double integral_part;
    bool isDouble = (modf(d, &integral_part) != 0.0);

    if (d > INT64_MAX || d < INT64_MIN) {
      result->set_number_double(d);
    } else if (isDouble) {
      result->set_number_double(d);
    } else {
      result->set_number_int(static_cast<uint64_t>(d));
    }

    return json_parse_type::json_value;
  }

  /*
    Main parsing logic
  */
  json_parse_type parse_value(JSONNode *result) {
    parse_whitespace();
    json_parse_type type = json_parse_type::error;
    if (remaining_parse_length() >= 4 && parse_n_and_compare(4, "true")) {
      result->set_boolean(true);
      m_parse_index += 4;
      type = json_parse_type::json_value;
    } else if (remaining_parse_length() >= 5 &&
               parse_n_and_compare(5, "false")) {
      result->set_boolean(false);
      m_parse_index += 5;
      type = json_parse_type::json_value;
    } else if (remaining_parse_length() >= 4 &&
               parse_n_and_compare(4, "null")) {
      m_parse_index += 4;
      type = json_parse_type::json_value;
    } else if (remaining_parse_length() > 1 && parse_and_compare('[')) {
      m_parse_index++;
      type = parse_array(result);
    } else if (remaining_parse_length() > 1 && parse_and_compare('{')) {
      m_parse_index++;
      type = parse_object(result);
    } else if (remaining_parse_length() > 0 && parse_and_compare(']')) {
      m_parse_index++;
      type = json_parse_type::reach_array_end;
    } else if (remaining_parse_length() > 0 && parse_and_compare(',')) {
      m_parse_index++;
      type = json_parse_type::reach_array_separator;
    } else if (remaining_parse_length() > 1 && parse_and_compare('\"')) {
      m_parse_index++;
      type = parse_string(result);
    } else if (remaining_parse_length() > 0 &&
               (isdigit(current_character()) || current_character() == '-')) {
      type = parse_number(result);
    }
    parse_whitespace();
    return type;
  }

 private:
  size_t remaining_parse_length() { return m_json_s.length() - m_parse_index; }
  bool parse_n_and_compare(size_t n, const char *s) {
    return strcmp(m_json_s.substr(m_parse_index, n).c_str(), s) == 0;
  }
  bool parse_and_compare(const char ch) { return current_character() == ch; }
  const char current_character() { return m_json_s[m_parse_index]; }

 private:
  JSONNode *m_result;
  const std::string &m_json_s;
  unsigned int m_parse_index = 0;
};
}  // namespace detail
}  // namespace miniJSON
