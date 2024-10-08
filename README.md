# MiniJSON ![Lint](https://github.com/wzc221207/miniJSON/actions/workflows/lint.yml/badge.svg) ![Test](https://github.com/wzc221207/miniJSON/actions/workflows/test.yml/badge.svg)

MiniJSON is a C++ header-only JSON parsing library I wrote for my own learning purposes. It is not intended to be used for production.

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
json["friends"][1] = "John";
std::cout << json["username"].get_string() << std::endl;    // Gloria
std::cout << json["age"].get_integer() << std::endl;        // 26
std::cout << json["friends"][1].get_string() << std::endl;  // John
```

## License

[MIT License](https://github.com/wzc221207/miniJSON/blob/main/LICENSE)

Copyright (c) 2024 Zhichen (Joshua) Wen