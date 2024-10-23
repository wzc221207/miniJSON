// Copyright (c) 2024 Zhichen (Joshua) Wen
#pragma once

#include <cstring>
#include <initializer_list>
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
#define MINIJSON_VERSION_PATCH 10

namespace miniJSON {
/*
  A JSON node can hold the value of any JSON type
  (object/array/string/number/boolean/null).
*/
class json_node {
 public:
  json_node() {}
  json_node(json_node &&other)
      : m_value(std::move(other.m_value)), m_type(other.m_type) {
    other.m_value = {};
    other.m_type = json_value_type::null;
  }
  json_node &operator=(json_node &&other) {
    if (this != &other) {
      if (m_type == json_value_type::object) {
        delete_object();
      }
      if (m_type == json_value_type::array) {
        delete_array();
      }
      if (m_type == json_value_type::string) {
        delete_string();
      }
      m_value = std::move(other.m_value);
      m_type = other.m_type;
      other.m_value = {};
      other.m_type = json_value_type::null;
    }
    return *this;
  }
  json_node(const json_node &other) : m_type(other.m_type) {
    if (m_type == json_value_type::object) {
      m_value.object = new json_object_t(*other.m_value.object);
      for (auto &key : *other.m_value.object) {
        (*m_value.object)[key] = new json_node(json_value_type::indeterminate);
        *(*m_value.object)[key] = *(*other.m_value.object)[key];
      }
    } else if (m_type == json_value_type::array) {
      m_value.array = new json_array_t(*other.m_value.array);
      for (int i = 0; i < other.m_value.array->size(); i++) {
        (*m_value.array)[i] = new json_node(json_value_type::indeterminate);
        *(*m_value.array)[i] = *(*other.m_value.array)[i];
      }
    } else if (m_type == json_value_type::string) {
      m_value.str = new json_string_t(*other.m_value.str);
    } else {
      m_value = other.m_value;
    }
  }
  json_node &operator=(const json_node &other) {
    if (this != &other) {
      if (m_type == json_value_type::object) {
        delete_object();
      }
      if (m_type == json_value_type::array) {
        delete_array();
      }
      if (m_type == json_value_type::string) {
        delete_string();
      }
      m_type = other.m_type;
      if (m_type == json_value_type::object) {
        m_value.object = new json_object_t(*other.m_value.object);
        for (auto &key : *other.m_value.object) {
          (*m_value.object)[key] =
              new json_node(json_value_type::indeterminate);
          *(*m_value.object)[key] = *(*other.m_value.object)[key];
        }
      } else if (m_type == json_value_type::array) {
        m_value.array = new json_array_t(*other.m_value.array);
        for (int i = 0; i < other.m_value.array->size(); i++) {
          (*m_value.array)[i] = new json_node(json_value_type::indeterminate);
          *(*m_value.array)[i] = *(*other.m_value.array)[i];
        }
      } else if (m_type == json_value_type::string) {
        m_value.str = new json_string_t(*other.m_value.str);
      } else {
        m_value = other.m_value;
      }
    }
    return *this;
  }
  ~json_node() {
    if (m_type == json_value_type::array) {
      delete_array();
    } else if (m_type == json_value_type::string) {
      delete_string();
    } else if (m_type == json_value_type::object) {
      delete_object();
    }
  }

 private:
  /*
    Helper methods to delete JSON nodes of object/array/string types
   */

  void delete_object() {
    for (auto key : *m_value.object) {
      delete (*m_value.object)[key];
    }
    delete m_value.object;
  }
  void delete_array() {
    for (int i = 0; i < m_value.array->size(); i++) {
      json_node *j = (*m_value.array)[i];
      delete j;
    }
    delete m_value.array;
  }
  void delete_string() { delete m_value.str; }

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
    if (m_type == json_value_type::indeterminate) {
      throw json_type_error(
          "JSON node has an indeterminate child. Access of the previously "
          "non-existent element of object or array creates indeterminate "
          "node.");
    }
    throw json_type_error("invalid type");
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
    Get the current JSON value type in string
  */
  std::string get_type_in_string() const {
    switch (m_type) {
      case json_value_type::array:
        return "Array";
        break;
      case json_value_type::object:
        return "Object";
        break;
      case json_value_type::null:
        return "Null";
        break;
      case json_value_type::indeterminate:
        return "Indeterminate";
        break;
      case json_value_type::number_double:
        return "Double Number";
        break;
      case json_value_type::number_int:
        return "Integral Number";
        break;
      case json_value_type::string:
        return "String";
        break;
      case json_value_type::boolean:
        return "Boolean";
        break;
      default:
        break;
    }
    throw json_type_error("invalid type");
  }

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
    Get value associated with key from object
  */
  json_node &operator[](json_string_t key) const {
    if (m_type == json_value_type::object) {
      if (m_value.object->count(key) == 0) {
        (*m_value.object)[key] = new json_node(json_value_type::indeterminate);
      }
      auto val = (*m_value.object)[key];
      return *val;
    }
    throw json_type_error(
        "trying to access object value from a non-object JSON node");
  }

  /*
    Get value at index from array
  */
  json_node &operator[](size_t index) const {
    if (m_type == json_value_type::array) {
      if (index < 0) {
        throw std::out_of_range("invalid negative index value");
      }
      if ((index >= m_value.array->size())) {
        m_value.array->resize(index + 1);
        (*m_value.array)[index] = new json_node(json_value_type::indeterminate);
      }
      auto val = (*m_value.array)[index];
      return *val;
    }
    throw json_type_error(
        "trying to access array value from a non-array JSON node");
  }

 public:
  struct iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = const json_node &;
    using pointer = json_node *;
    using reference = json_node &;
    iterator(pointer ptr, json_array_t::iterator array_iterator = {},
             json_object_t::values_iterator object_iterator = {})
        : m_ptr(ptr),
          m_array_iterator(array_iterator),
          m_object_iterator(object_iterator) {}
    value_type operator*() const {
      if (m_ptr->m_type == json_value_type::array) {
        return *(*m_array_iterator);
      } else if (m_ptr->m_type == json_value_type::object) {
        return *(*m_object_iterator);
      }
      throw json_type_error(
          "json_node iterator not supported for non object or array types");
    }
    pointer operator->() {
      if (m_ptr->m_type == json_value_type::array) {
        return *m_array_iterator;
      } else if (m_ptr->m_type == json_value_type::object) {
        return *m_object_iterator;
      }
      throw json_type_error(
          "json_node iterator not supported for non object or array types");
    }

    iterator &operator++() {
      auto j = *m_ptr;
      if (m_ptr->m_type == json_value_type::array) {
        m_array_iterator++;
      } else if (m_ptr->m_type == json_value_type::object) {
        m_object_iterator++;
      }
      return *this;
    }

    iterator operator++(int) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    bool operator==(const iterator &other) const {
      return this->m_ptr == other.m_ptr &&
             m_array_iterator == other.m_array_iterator &&
             m_object_iterator == other.m_object_iterator;
    }
    bool operator!=(const iterator &other) const {
      return this->m_ptr != other.m_ptr ||
             m_array_iterator != other.m_array_iterator ||
             m_object_iterator != other.m_object_iterator;
    }

   public:
    json_string_t key() {
      if (m_ptr->m_type == json_value_type::object) {
        return m_object_iterator.key();
      }
      throw json_type_error(
          "iterator::key() is only supported for object type");
    }
    pointer value() {
      if (m_ptr->m_type == json_value_type::object) {
        return m_object_iterator.value();
      }
      throw json_type_error(
          "iterator::value() is only supported for object type");
    }

   private:
    pointer m_ptr;
    json_array_t::iterator m_array_iterator;
    json_object_t::values_iterator m_object_iterator;
  };

  iterator begin() {
    if (m_type == json_value_type::array) {
      return iterator(this, m_value.array->begin());
    } else if (m_type == json_value_type::object) {
      return iterator(this, {}, m_value.object->values_begin());
    }
    throw json_type_error(
        "json_node iterator not supported for non object or array types");
    return iterator(nullptr);
  }
  iterator end() {
    if (m_type == json_value_type::array) {
      return iterator(this, m_value.array->end());
    } else if (m_type == json_value_type::object) {
      return iterator(this, {}, m_value.object->values_end());
    }
    throw json_type_error(
        "json_node iterator not supported for non object or array types");
    return iterator(nullptr);
  }

 public:
  /*
    List of convenient constructors
  */

  json_node(json_value_type t) : m_type(t) {
    switch (t) {
      case json_value_type::string:
        m_value.str = new json_string_t;
        break;
      case json_value_type::boolean:
        m_value.boolean = true;
        break;
      case json_value_type::number_int:
        m_value.number_int = 0;
        break;
      case json_value_type::number_double:
        m_value.number_double = 0.0;
        break;
      case json_value_type::object:
        m_value.object = new json_object_t;
        break;
      case json_value_type::array:
        m_value.array = new json_array_t;
        break;
      case json_value_type::null:
        break;
      case json_value_type::indeterminate:
        break;
      default:
        break;
    }
  }
  json_node(json_boolean_t b) {
    m_value.boolean = b;
    m_type = json_value_type::boolean;
  }
  json_node(json_int_t num) {
    m_value.number_int = num;
    m_type = json_value_type::number_int;
  }
  json_node(int num) {
    m_value.number_int = num;
    m_type = json_value_type::number_int;
  }
  json_node(json_double_t num) {
    m_value.number_double = num;
    m_type = json_value_type::number_double;
  }
  json_node(const char *s) {
    m_value.str = new std::string(s);
    m_type = json_value_type::string;
  }
  json_node(const json_string_t &s) {
    m_value.str = new std::string(s);
    m_type = json_value_type::string;
  }
  json_node(std::nullptr_t n) {}
  json_node(std::initializer_list<json_node> l) {
    bool is_object = is_initializer_object(l);
    size_t sz = l.size();
    if (is_object) {
      m_type = json_value_type::object;
      m_value.object = new json_object_t;
      for (auto it = l.begin(); it != l.end(); it++) {
        const std::string &key = (*(it->m_value.array))[0]->get_string();
        json_node *value = (*(it->m_value.array))[1];
        (*m_value.object)[key] = new json_node(json_value_type::indeterminate);
        *(*m_value.object)[key] = *value;
      }
    } else {
      m_type = json_value_type::array;
      m_value.array = new json_array_t(sz);
      for (int i = 0; i < sz; i++) {
        (*m_value.array)[i] = new json_node(json_value_type::indeterminate);
      }
      size_t idx = 0;
      for (auto it = l.begin(); it != l.end(); idx++) {
        *(*m_value.array)[idx] = *it;
        it++;
      }
    }
  }

 public:
  /*
    delete methods for JSON of object or array type
  */
  size_t erase(const std::string &key) {
    if (m_type == json_value_type::object) {
      if (m_value.object->count(key) == 0) {
        return 0;
      }
      return m_value.object->erase(key);
    }
    throw json_type_error(
        "trying to delete key-pair from a non-object JSON node");
  }
  json_array_t::iterator erase(size_t index) {
    if (m_type == json_value_type::array) {
      if (index >= m_value.array->size()) {
        throw std::out_of_range("invalid index");
      }
      return m_value.array->erase(m_value.array->begin() + index);
    }
    throw json_type_error("trying to delete value from a non-array JSON node");
  }

 private:
  /*
    Check if the initializer list contains an JSON of object type
  */
  bool is_initializer_object(const std::initializer_list<json_node> &l) {
    bool is_object = true;
    for (auto it = l.begin(); it != l.end(); it++) {
      if (it->m_type != json_value_type::array) {
        is_object = false;
        break;
      }
      if (it->m_value.array->size() != 2) {
        is_object = false;
        break;
      }
      if ((*it->m_value.array)[0]->m_type != json_value_type::string) {
        is_object = false;
        break;
      }
    }
    return is_object;
  }

 private:
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
    json_value(const json_value &other) = default;
    json_value &operator=(const json_value &other) = default;
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
