export module codie.json.encoding;

import<string>;
import<string_view>;
import<stdexcept>;
import<optional>;
import<iostream>;
import<charconv>;

import codie.json.object;

export namespace codie::json {
template <typename T>
void encode(const T &val, std::ostream &os, bool indented = false) {
  if constexpr (std::is_same_v<T, bool>) {
    os << (val ? "true" : "false");
  } else if constexpr (std::is_same_v<T, std::nullopt_t>) {
    os << "null";
  } else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
    os << val;
  } else if constexpr (std::is_same_v<T, std::string>) {
    os << "\"" << val << "\"";
  } else if constexpr (std::is_same_v<T, object>) {
    os << "{";

    if (indented)
      os << "\n";

    for (auto it = val.begin(); it != val.end(); ++it) {
      os << "\"" << it->first << "\": ";
      encode(it->second, os, indented);

      if (std::next(it) != val.end())
        os << ",";

      if (indented)
        os << "\n";
    }

    os << "}";
  } else if constexpr (std::is_same_v<T, std::vector<value>>) {
    os << '[';
    if (indented)
      os << "\n";

    for (auto it = val.begin(); it != val.end(); ++it) {
      encode(*it, os, indented);

      if (it != val.end() - 1)
        os << ",";

      if (indented)
        os << "\n";
    }

    os << ']';

  } else if constexpr (std::is_same_v<T, value>) {
    if (auto *v = val.get_if<std::nullopt_t>()) {
      os << "null";
    } else if (auto *v = val.get_if<bool>()) {
      encode(*v, os, indented);
    } else if (auto *v = val.get_if<std::int64_t>()) {
      encode(*v, os, indented);
    } else if (auto *v = val.get_if<std::uint64_t>()) {
      encode(*v, os, indented);
    } else if (auto *v = val.get_if<double>()) {
      encode(*v, os, indented);
    } else if (auto *v = val.get_if<std::string>()) {
      encode(*v, os, indented);
    } else if (auto *v = val.get_if<object>()) {
      encode(*v, os, indented);
    } else if (auto *v = val.get_if<std::vector<value>>()) {
      encode(*v, os, indented);
    } else {
      throw std::runtime_error("unknown value type");
    }
  }
}

template <typename T>
std::string encode(const T &value, bool indented = false) {
  std::stringstream ss;
  encode(value, ss, indented);
  return ss.str();
}
} // namespace codie::json