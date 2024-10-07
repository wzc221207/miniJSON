// Copyright (c) 2024 Zhichen (Joshua) Wen
#pragma once

#include <cstring>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "./detail/ordered_map.h"
#include "./detail/parser.h"
#include "./errors.h"
#include "./json_types.h"

#define MINIJSON_VERSION_MAJOR 0
#define MINIJSON_VERSION_MINOR 1
#define MINIJSON_VERSION_PATCH 4

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
      return '\"' + *m_value.str + '\"';
    }
    if (m_type == json_value_type::number_int) {
      return std::to_string(m_value.number_int);
    }
    if (m_type == json_value_type::number_double) {
      std::ostringstream sstream;
      sstream << m_value.number_double;
      return sstream.str();
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
        s += '\"' + key + '\"';
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
  using json_object_t = ordered_map<std::string, json_node *>;
  using json_array_t = std::vector<json_node *>;
  using json_string_t = std::string;
  using json_int_t = int64_t;
  using json_double_t = double;
  using json_boolean_t = bool;

 public:
  /*
    Get the current JSON value type
  */
  json_value_type get_type() const { return m_type; }

  /*
    Get the current boolean value
  */
  const bool get_boolean() const {
    if (m_type == json_value_type::boolean) {
      return m_value.boolean;
    }
    throw json_type_error(
        "trying to access boolean value from a non-boolean JSON node");
  }

  /*
    Get the current boolean value
  */
  const json_string_t get_string() const {
    if (m_type == json_value_type::string) {
      return *m_value.str;
    }
    throw json_type_error(
        "trying to access string value from a non-string JSON node");
  }

  /*
    Get the current integer value
  */
  const json_int_t get_integer() const {
    if (m_type == json_value_type::number_int) {
      return m_value.number_int;
    }
    throw json_type_error(
        "trying to access integer value from a non-integer JSON node");
  }

  /*
    Get the current double value
  */
  const json_double_t get_double() const {
    if (m_type == json_value_type::number_double) {
      return m_value.number_double;
    }
    throw json_type_error(
        "trying to access double value from a non-double JSON node");
  }

  /*
    Get the current object value
  */
  json_node &operator[](json_string_t key) const {
    if (m_type == json_value_type::object) {
      if (m_value.object->count(key) == 0) {
        throw std::out_of_range("key is not found in the object");
      }
      auto val = (*m_value.object)[key];
      return *val;
    }
    throw json_type_error(
        "trying to access object value from a non-object JSON node");
  }

  /*
    Get the current array value
  */
  json_node &operator[](size_t index) const {
    if (m_type == json_value_type::array) {
      if ((index >= m_value.array->size()) || index < 0) {
        throw std::out_of_range("given index out of range");
      }
      auto val = (*m_value.array)[index];
      return *val;
    }
    throw json_type_error(
        "trying to access array value from a non-array JSON node");
  }

 public:
  /*
    Initialize the current boolean value
   */
  void set_boolean(json_boolean_t b) {
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
    Initialize the current integer numeric value
   */
  void set_number_int(json_int_t x) {
    m_value.number_int = x;
    m_type = json_value_type::number_int;
  }
  /*
    Initialize the current double numeric value
   */
  void set_number_double(json_double_t d) {
    m_value.number_double = d;
    m_type = json_value_type::number_double;
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
  union json_value {
    json_object_t *object;
    json_array_t *array;
    json_string_t *str;
    json_int_t number_int;
    json_double_t number_double;
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
inline json_node parse(const std::string &s) {
  json_node j;
  detail::parser<json_node> parser{&j, s};
  parser.parse();
  return j;
}

}  // namespace miniJSON
