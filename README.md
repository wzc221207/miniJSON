# MiniJSON ![Lint](https://github.com/wzc221207/miniJSON/actions/workflows/lint.yml/badge.svg) ![Test](https://github.com/wzc221207/miniJSON/actions/workflows/test.yml/badge.svg)

MiniJSON is a simple header-only JSON parsing library written using C++11 features. It is created while I was learning the inner workings of a JSON parser from awesome libraries such as JSON for Modern C++ and cJSON. It is maintained only for learning purposes and not intended to be used on production.

## Usage

```cpp
#include <iostream>
#include "miniJSON/miniJSON.h"

// parse JSON string
auto json = miniJSON::parse(
R"({"username": "Alicia", "age": 32, "friends": ["Michael", "David"], "job": null})");
// access values in JSON node
std::cout << json["username"].get_string() << std::endl;    // Alicia
std::cout << json["age"].get_integer() << std::endl;        // 32
std::cout << json["friends"][1].get_string() << std::endl;  // David
// change values in JSON node
json["username"] = "Gloria";
json["age"] = 26;
json["friends"][1] = "Daryl";
json["job"] = "Statistician";
std::cout << json["username"].get_string() << std::endl;    // Gloria
std::cout << json["age"].get_integer() << std::endl;        // 26
std::cout << json["friends"][1].get_string() << std::endl;  // Daryl
std::cout << json["job"].get_string() << std::endl;         // Statistician
// use initializer list to create JSON object or array
json["likes"] = {"volleyball", "tennis"}; // array
json["relationship"] = {{"John", "husband"}}; // object
/* 
  serialization
  output:
    {"username":"Gloria","age":26,"friends":["Michael","Daryl"],"job":"Statistician","likes":["volleyball","tennis"],"relationship":{"John":"husband"}}
*/
std::cout << json.to_string() << std::endl;
/* 
  iterating over array
  output:
    Michael
    Daryl
*/
for (auto &j : json["friends"]) {
    std::cout << j.get_string() << std::endl;
}
/* 
  iterating over object
  output:
    John : husband
*/
for (auto it = json["relationship"].begin(); it != json["relationship"].end(); it++) {
    std::cout << it.key() << " : " << it.value()->get_string() << std::endl;
}
/*
  delete entries from object
*/
json["friends"].erase(0); // deleted Michael at array index 0
/*
  delete entries from array
*/
json["relationship"].erase("John"); // deleted a pair whose key is John
```

## License

[MIT License](https://github.com/wzc221207/miniJSON/blob/main/LICENSE)

Copyright (c) 2024 Zhichen (Joshua) Wen