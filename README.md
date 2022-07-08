# codie
C++20 modularized encoding library

# Usage

codie is a cmake compatible library.
```cmake

include(FetchContent)

FetchContent_Declare(
    codie
    GIT_REPOSITORY https://github.com/warrenulrich/codie.git
)

FetchContent_MakeAvailable(codie)
```

# JSON

## Usage
```cmake

# After fetch
target_link_libraries(<target_name> codie::json)
```

## Decoding
```c++
import<iostream>;

import codie.json;

std::string person_json = R"({
  "name": "John Doe",
  "age": 42,
  "married": true,
  "children": [
    "Jane Doe",
    "John Doe Jr."
  ],
  "address": {
    "street": "123 Main St.",
    "city": "Anytown",
    "state": "California"
  }
})";

int main(int argc, char **args) {
  try {
    codie::json::value json = codie::json::decode(person_json);

    if (auto *obj = json.get_if<codie::json::object>()) { // type checking.
      for (auto &[key, v] : *obj) { // iterating over object.
        std::cout << "(key, value): " << '(' << key << ", ";
        codie::json::encode(v, std::cout);

        std::cout << ")" << std::endl;
      }
    } else {
      std::cout << "not an object" << std::endl;
    }
  } catch (const codie::json::decoding_error &e) { // error handling.
    std::cerr << e.what() << '\n';
  }
}
```