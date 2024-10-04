// Copyright (c) 2024 Zhichen (Joshua) Wen
#pragma once

#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "./detail/ordered_map.h"
#include "./detail/parser.h"
#include "./errors.h"
#include "./json_types.h"

#define MINIJSON_VERSION_MAJOR 0
#define MINIJSON_VERSION_MINOR 1
#define MINIJSON_VERSION_PATCH 2

namespace miniJSON {
/*
  A JSON node can hold the value of any JSON type
  (object/array/string/number/boolean/null).
*/
class json_node {
 public:
  json_node() {}
  json_node(const json_node &other) = delete;
  json_node &operator=(const json_node &other) = delete;
  json_node(json_node &&other)
      : m_value(std::move(other.m_value)), m_type(other.m_type) {
    other.m_value = {};
    other.m_type = json_value_type::null;
  }
  json_node &operator=(json_node &&other) = delete;
  ~json_node() {
    if (m_type == json_value_type::array) {
      for (int i = 0; i < m_value.array->size(); i++) {
        json_node *j = (*m_value.array)[i];
        delete j;
      }
      delete m_value.array;
    } else if (m_type == json_value_type::string) {
      delete m_value.str;
    } else if (m_type == json_value_type::object) {
      for (auto key : *m_value.object) {
        delete (*m_value.object)[key];
      }
      delete m_value.object;
    }
  }

 public:
  /*
    Convert JSON node into JSON string (Serialization)
  */
  std::string to_string() const {
    if (m_type == json_value_type::boolean) {
      return m_value.boolean ? "true" : "false";
    }
    if (m_type == json_value_type::null) {
      return "null";
    }
    if (m_type == json_value_type::string) {
      return *m_value.str;
    }
    if (m_type == json_value_type::array) {
      std::string s{"["};
      for (int i = 0; i < m_value.array->size(); i++) {
        s += (*m_value.array)[i]->to_string();
        if (i != m_value.array->size() - 1) {
          s += ",";
        }
      }
      s += "]";
      return s;
    }
    if (m_type == json_value_type::object) {
      std::string s{"{"};
      size_t sz = (*m_value.object).size();
      int i = 0;
      for (auto key : *m_value.object) {
        s += key;
        s += ":";
        s += (*m_value.object)[key]->to_string();
        if (i != sz - 1) {
          s += ",";
        }
        i++;
      }
      s += "}";
      return s;
    }
    return "";
  }

 public:
  /*
    Get the current JSON value type
  */
  json_value_type get_type() const { return m_type; }
  /*
    Get the current boolean value
  */
  const bool *get_boolean() const {
    if (m_type == json_value_type::boolean) {
      return &(m_value.boolean);
    }
    return nullptr;
  }

 public:
  /*
    Initialize the current boolean value
   */
  void set_boolean(bool b) {
    m_type = json_value_type::boolean;
    m_value.boolean = b;
  }
  /*
    Initialize the current array value
   */
  void set_array() {
    m_value.array = new json_array_t;
    m_type = json_value_type::array;
  }
  /*
    Initialize the current string value
   */
  void set_string() {
    m_value.str = new json_string_t;
    m_type = json_value_type::string;
  }
  /*
    Initialize the current object value
   */
  void set_object() {
    m_value.object = new json_object_t;
    m_type = json_value_type::object;
  }

 public:
  friend class detail::parser<json_node>;

 private:
  /*
    Define basic json value types:
    - object: associative array {string: value}
    - array: ordered list of value
    - value: object/array/string/number/boolean/null
    - string
    - number
    - boolean: true/false
    - null
  */
  using json_object_t = ordered_map<std::string, json_node *>;
  using json_array_t = std::vector<json_node *>;
  using json_string_t = std::string;
  using json_number_t = int;
  using json_boolean_t = bool;
  union json_value {
    json_object_t *object;
    json_array_t *array;
    json_string_t *str;
    json_number_t number;
    json_boolean_t boolean;
    json_value() = default;
    json_value(json_value &&other) = default;
    json_value &operator=(json_value &&other) = default;
  };

 private:
  json_value m_value = {};
  json_value_type m_type = json_value_type::null;
};

/*
  Parse JSON string into JSON node (Deserialization)
*/
json_node parse(const std::string &s) {
  json_node j;
  detail::parser<json_node> parser{&j, s};
  parser.parse();
  return j;
}

}  // namespace miniJSON
