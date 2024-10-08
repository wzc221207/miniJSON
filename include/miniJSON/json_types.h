// Copyright (c) 2024 Zhichen (Joshua) Wen
#pragma once
#include <string>

namespace miniJSON {
/*
  Types of JSON values
*/
enum class json_value_type {
  object,
  array,
  string,
  number_int,
  number_double,
  boolean,
  null,
  indeterminate
};
}  // namespace miniJSON
