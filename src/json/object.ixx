export module codie.json.object;

import<optional>;
import<vector>;
import<unordered_map>;
import<variant>;
import<iostream>;

export namespace codie::json {
class value;

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

  object(const object &other) noexcept : _map(other._map) {}

  object(object &&other) noexcept : _map(std::move(other._map)) {}

  object(const map_t &map) noexcept : _map(map) {}

  object(map_t &&map) noexcept : _map(std::move(map)) {}

  object &operator=(const object &other) {
    _map = other._map;
    return *this;
  }

  object &operator=(object &&other) noexcept {
    _map = std::move(other._map);
    return *this;
  }

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

private:
  std::unordered_map<std::string, value> _map;
};

class value {
public:
  using variant_t =
      std::variant<std::nullopt_t, bool, double, std::uint64_t, std::int64_t,
                   std::string, object, std::vector<value>>;

  value() noexcept : _value(std::nullopt) {}

  value(const value &other) noexcept : _value(other._value) {}

  value(value &&other) noexcept : _value(std::move(other._value)) {}

  value(bool b) noexcept : _value(b) {}

  value(std::int64_t i) noexcept : _value(i) {}

  value(std::uint64_t u) noexcept : _value(u) {}

  value(double d) : _value(d) {}

  value(const std::string &str) noexcept : _value(str) {}

  value(std::string &&str) noexcept : _value(std::move(str)) {}

  value(const object &obj) noexcept : _value(obj) {}

  value(object &&obj) noexcept : _value(std::move(obj)) {}

  value(const std::vector<value> &vec) noexcept : _value(vec) {}

  value(std::vector<value> &&vec) noexcept : _value(std::move(vec)) {}

  value &operator=(const std::string &str) noexcept {
    _value = str;
    return *this;
  }

  value &operator=(std::string &&str) noexcept {
    _value = std::move(str);
    return *this;
  }

  value &operator=(const object &obj) noexcept {
    _value = obj;
    return *this;
  }

  value &operator=(object &&obj) noexcept {
    _value = std::move(obj);
    return *this;
  }

  value &operator=(const std::vector<value> &vec) noexcept {
    _value = vec;
    return *this;
  }

  value &operator=(std::vector<value> &&vec) noexcept {
    _value = std::move(vec);
    return *this;
  }

  value &operator=(bool b) noexcept {
    _value = b;
    return *this;
  }

  template <std::integral T> value &operator=(T i) noexcept {
    if constexpr (std::is_signed_v<T>) {
      _value = std::int64_t(i);
    } else {
      _value = std::uint64_t(i);
    }
    return *this;
  }

  template <std::floating_point T> value &operator=(T f) noexcept {
    _value = static_cast<double>(f);
    return *this;
  }

  value &operator=(std::nullopt_t) noexcept {
    _value = std::nullopt;
    return *this;
  }

  value &operator=(const value &other) noexcept {
    _value = other._value;
    return *this;
  }

  value &operator=(value &&other) noexcept {
    _value = std::move(other._value);
    return *this;
  }

  bool is_null() const noexcept {
    return std::holds_alternative<std::nullopt_t>(_value);
  }

  bool is_bool() const noexcept { return std::holds_alternative<bool>(_value); }

  bool is_integer() const noexcept {
    return std::holds_alternative<std::int64_t>(_value) ||
           std::holds_alternative<std::uint64_t>(_value);
  }

  bool is_signed_integer() const noexcept {
    return std::holds_alternative<std::int64_t>(_value);
  }

  bool is_unsigned_integer() const noexcept {
    return std::holds_alternative<std::uint64_t>(_value);
  }

  bool is_floating_point() const noexcept {
    return std::holds_alternative<double>(_value);
  }

  bool is_string() const noexcept {
    return std::holds_alternative<std::string>(_value);
  }

  bool is_array() const noexcept {
    return std::holds_alternative<std::vector<value>>(_value);
  }

  bool is_object() const noexcept {
    return std::holds_alternative<object>(_value);
  }

  template <typename T> T &get() { return std::get<T>(_value); }

  template <typename T> const T &get() const { return std::get<T>(_value); }

  template <typename T> T *get_if() noexcept { return std::get_if<T>(&_value); }

  template <typename T> const T *get_if() const noexcept {
    return std::get_if<T>(&_value);
  }

private:
  variant_t _value;
};

void object::clear() noexcept { _map.clear(); }

void object::insert(const object::value_type &value) { _map.insert(value); }

void object::insert(const object::value_type &&value) {
  _map.insert(std::move(value));
}

std::pair<object::iterator, bool>
object::emplace(const object::key_type &key, const object::mapped_type &value) {
  return _map.emplace(key, value);
}

std::pair<object::iterator, bool> object::emplace(const object::key_type &key,
                                                  object::mapped_type &&value) {
  return _map.emplace(key, value);
}

std::pair<object::iterator, bool>
object::emplace(object::key_type &&key, const object::mapped_type &value) {
  return _map.emplace(std::move(key), value);
}

std::pair<object::iterator, bool> object::emplace(object::key_type &&key,
                                                  object::mapped_type &&value) {
  return _map.emplace(std::move(key), std::move(value));
}

object::mapped_type &object::at(const key_type &key) { return _map.at(key); }

const object::mapped_type &object::at(const key_type &key) const {
  return _map.at(key);
}

object::mapped_type &object::operator[](const key_type &key) {
  return _map[key];
}

object::mapped_type &object::operator[](const key_type &&key) {
  return _map[std::move(key)];
}

const object::mapped_type &object::operator[](const key_type &key) const {
  return _map.at(key);
}

const object::mapped_type &object::operator[](const key_type &&key) const {
  return _map.at(std::move(key));
}

object::size_type object::count(const key_type &key) const {
  return _map.count(key);
}

bool object::contains(const key_type &key) const { return _map.contains(key); }

object::iterator object::begin() noexcept { return _map.begin(); }

object::const_iterator object::begin() const noexcept { return _map.begin(); }

object::iterator object::end() noexcept { return _map.end(); }

object::const_iterator object::end() const noexcept { return _map.end(); }
} // namespace codie::json