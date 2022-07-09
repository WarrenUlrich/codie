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
/*
 * @tparam T The type of the value.
 * @brief Returns whether type T is decodable from a json string. T is decodable
 * if it is a codie::json::object, codie::json::value, bool, int64_t, uint64_t,
 * double, std::string, or std::vector<decodable>.
 */
template <typename T> 
constexpr bool is_decodable();

template <typename T>
concept decodable = is_decodable<T>();

template <decodable T = value, typename Iter>
T decode(Iter &begin, Iter end) requires std::is_same_v<typename Iter::value_type, char>;

template <decodable T = value, typename Iter>
T decode(Iter &&begin, Iter end) requires std::is_same_v<typename Iter::value_type, char>;

template <decodable T = value, std::ranges::range Range>
T decode(Range &range) requires std::is_same_v <std::ranges::range_value_t<Range>, char>

template <decodable T = value, std::ranges::range Range>
T decode(Range &&range) requires std::is_same_v <std::ranges::range_value_t<Range>, char>
```

### Object
```c++
class object {
public:
  using map_t = std::unordered_map<std::string, value>;

  using key_type = map_t::key_type;

  using size_type = map_t::size_type;

  using difference_type = map_t::difference_type;

  using mapped_type = map_t::mapped_type;

  using value_type = map_t::value_type;

  using iterator = map_t::iterator;

  using const_iterator = map_t::const_iterator;

  object() noexcept = default;

  object(const object &other) noexcept;

  object(object &&other) noexcept;

  object(const map_t &map) noexcept;

  object(map_t &&map) noexcept;

  object &operator=(const object &other);

  object &operator=(object &&other) noexcept;

  void clear() noexcept;

  void insert(const value_type &val);

  void insert(const value_type &&val);

  std::pair<iterator, bool> emplace(const key_type &key,
                                    const mapped_type &value);

  std::pair<iterator, bool> emplace(const key_type &key, mapped_type &&value);

  std::pair<iterator, bool> emplace(key_type &&key, const mapped_type &value);

  std::pair<iterator, bool> emplace(key_type &&key, mapped_type &&value);

  mapped_type &at(const key_type &key);

  const mapped_type &at(const key_type &key) const;

  mapped_type &operator[](const key_type &key);

  mapped_type &operator[](const key_type &&key);

  const mapped_type &operator[](const key_type &key) const;

  const mapped_type &operator[](const key_type &&key) const;

  size_type count(const key_type &key) const;

  bool contains(const key_type &key) const;

  iterator begin() noexcept;

  const_iterator begin() const noexcept;

  iterator end() noexcept;

  const_iterator end() const noexcept;
};
```

### Value
```c++
class value {
public:
  using variant_t =
      std::variant<std::nullopt_t, bool, double, std::uint64_t, std::int64_t,
                   std::string, object, std::vector<value>>;

  value() noexcept;

  value(const value &other) noexcept;

  value(value &&other) noexcept;

  value(bool b) noexcept;

  value(std::int64_t i) noexcept;

  value(std::uint64_t u) noexcept;

  value(double d);

  value(const std::string &str) noexcept;

  value(std::string &&str) noexcept;

  value(const object &obj) noexcept;

  value(object &&obj) noexcept;

  value(const std::vector<value> &vec) noexcept;

  value(std::vector<value> &&vec) noexcept;
  value &operator=(const std::string &str) noexcept;

  value &operator=(std::string &&str) noexcept;

  value &operator=(const object &obj) noexcept;

  value &operator=(object &&obj) noexcept;

  value &operator=(const std::vector<value> &vec) noexcept;

  value &operator=(std::vector<value> &&vec) noexcept;

  value &operator=(bool b) noexcept;

  template <std::integral T> value &operator=(T i) noexcept;

  template <std::floating_point T> value &operator=(T f) noexcept;

  value &operator=(std::nullopt_t) noexcept;

  value &operator=(const value &other) noexcept;

  value &operator=(value &&other) noexcept;

  bool is_null() const noexcept;

  bool is_bool() const noexcept;

  bool is_integer() const noexcept;

  bool is_signed_integer() const noexcept;

  bool is_unsigned_integer() const noexcept;

  bool is_floating_point() const noexcept;

  bool is_string() const noexcept;

  bool is_array() const noexcept;

  bool is_object() const noexcept;

  template <typename T> T &get();

  template <typename T> const T &get() const;

  template <typename T> T *get_if() noexcept;

  template <typename T> const T *get_if() const noexcept;

  variant_t &get_variant() noexcept;

  const variant_t &get_variant() const noexcept;
};
```

### Example
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

### Output
```
(key, value): (name, "John Doe")
(key, value): (age, 42)
(key, value): (married, true)
(key, value): (children, ["Jane Doe","John Doe Jr."])
(key, value): (address, {"state": "California","street": "123 Main St.","city": "Anytown"})
```