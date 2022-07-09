export module codie.json.encoding;

import<string>;
import<string_view>;
import<stdexcept>;
import<optional>;
import<iostream>;
import<charconv>;
import<vector>;
import codie.json.object;
import codie.json.meta;

export namespace codie::json {

/*
 * @tparam T The type of the value.
 * @brief Returns whether type T is encodable to a json string. T is decodable
 * if it is a codie::json::object, codie::json::value, bool, int64_t, uint64_t,
 * double, std::string, or std::vector<decodable>.
 */
template <typename T> constexpr bool is_encodable() {
  if constexpr (meta::is_specialization_v<T, std::vector>) {
    using vec_value_t = typename T::value_type;
    return is_encodable<vec_value_t>();
  } else {
    return std::is_same_v<T, object> || std::is_same_v<T, value> ||
           std::is_same_v<T, std::string> ||
           std::is_same_v<T, std::string_view> || std::is_same_v<T, bool> ||
           std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t> ||
           std::is_same_v<T, double>;
  }
}

template <typename T>
concept encodable = is_encodable<T>();

template <encodable T>
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
  } else if constexpr (meta::is_specialization_v<T, std::vector>) {
    using vec_value_t = typename T::value_type;
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