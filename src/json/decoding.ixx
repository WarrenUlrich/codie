export module codie.json.decoding;

import<iterator>;
import<vector>;
import<stdexcept>;
import<ranges>;
import<cstdint>;
import<iostream>;
import<optional>;
import<variant>;
import<string_view>;

import codie.json.tokens;
import codie.json.object;
import codie.json.meta;

export namespace codie::json {

/*
 * @tparam T The type of the value.
 * @brief Returns whether type T is decodable from a json string. T is decodable
 * if it is a codie::json::object, codie::json::value, bool, int64_t, uint64_t,
 * double, std::string, or std::vector<decodable>.
 */
template <typename T> constexpr bool is_decodable() {
  if constexpr (meta::is_specialization_v<T, std::vector>) {
    using vec_value_t = typename T::value_type;
    return is_decodable<vec_value_t>();
  } else {
    return std::is_same_v<T, object> || std::is_same_v<T, value> ||
           std::is_same_v<T, std::string> || std::is_same_v<T, bool> ||
           std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t> ||
           std::is_same_v<T, double>;
  }
}

template <typename T>
concept decodable = is_decodable<T>();

class decoding_error : public std::runtime_error {
public:
  decoding_error(const std::string &what) : std::runtime_error(what) {}
  decoding_error(std::string &&what) : std::runtime_error(std::move(what)) {}
};

template <typename T, typename Iter>
T decode_object(Iter &begin, Iter end) requires
    std::is_same_v<typename Iter::value_type, char>;

/*
 * @tparam Vec the type of vector to decode into
 * @param begin iterator to the first character of the object
 * @param end iterator to the last character of the object
 * @returns the decoded Vec
 */
export template <typename Vec = std::vector<value>, std::input_iterator Iter>
Vec decode_array(Iter &begin, Iter end) requires
    std::is_same_v<typename Iter::value_type, char> &&
    meta::is_specialization_v<Vec, std::vector> {
  Vec result{};

  using vec_value_t = typename Vec::value_type;

  tokenize(begin, end, [&](auto &&tok) {
    using tok_t = std::decay_t<decltype(tok)>;

    if constexpr (std::is_same_v<tok_t, token_boolean_false>) {
      if constexpr (std::is_same_v<vec_value_t, value> ||
                    std::is_same_v<vec_value_t, bool>) {
        result.emplace_back(false);
      } else {
        throw decoding_error(
            "invalid type for array, expected codie::json::value or bool");
      }
    } else if constexpr (std::is_same_v<tok_t, token_boolean_true>) {
      if constexpr (std::is_same_v<vec_value_t, value> ||
                    std::is_same_v<vec_value_t, bool>) {
        result.emplace_back(true);
      } else {
        throw decoding_error(
            "invalid type for array, expected codie::json::value or bool");
      }
    } else if constexpr (std::is_same_v<tok_t, token_number_float>) {
      if constexpr (std::is_same_v<vec_value_t, value> ||
                    std::is_same_v<vec_value_t, double>) {
        result.emplace_back(tok.value);
        return true;
      } else {
        throw decoding_error(
            "invalid type for array, expected codie::json::value or double");
      }
    } else if constexpr (std::is_same_v<tok_t, token_number_signed_integer>) {
      if constexpr (std::is_same_v<vec_value_t, value> ||
                    std::is_same_v<vec_value_t, int64_t>) {
        result.emplace_back(tok.value);
        return true;
      } else {
        throw decoding_error(
            "invalid type for array, expected codie::json::value or int64_t");
      }
    } else if constexpr (std::is_same_v<tok_t, token_number_unsigned_integer>) {
      if constexpr (std::is_same_v<vec_value_t, value> ||
                    std::is_same_v<vec_value_t, uint64_t>) {
        result.emplace_back(tok.value);
        return true;
      } else {
        throw decoding_error(
            "invalid type for array, expected codie::json::value or uint64_t");
      }
    } else if constexpr (std::is_same_v<tok_t, token_null>) {
      if constexpr (std::is_same_v<vec_value_t, value>) {
        result.emplace_back(value());
      } else {
        throw decoding_error(
            "invalid type for array, expected codie::json::value");
      }
    } else if constexpr (std::is_same_v<tok_t, token_string>) {
      if constexpr (std::is_same_v<vec_value_t, value> ||
                    std::is_same_v<vec_value_t, std::string>) {
        result.emplace_back(tok.value);
      } else {
        throw decoding_error("invalid type for array, expected "
                             "codie::json::value or std::string");
      }
    } else if constexpr (std::is_same_v<tok_t, token_array_begin>) {
      if constexpr (std::is_same_v<vec_value_t, value> ||
                    std::is_same_v<vec_value_t, Vec>) {
        result.emplace_back(decode_array(begin, end));
      } else {
        throw decoding_error("invalid type for array, expected "
                             "codie::json::value, or codie::json::object");
      }
    } else if constexpr (std::is_same_v<tok_t, token_object_begin>) {
      if constexpr (std::is_same_v<vec_value_t, value> ||
                    std::is_same_v<vec_value_t, object>) {
        result.emplace_back(decode_object(begin, end));
      } else {
        throw decoding_error("invalid type for array, expected "
                             "codie::json::value, or codie::json::object");
      }
    } else if constexpr (std::is_same_v<tok_t, token_comma>) {
      return true;
    } else if constexpr (std::is_same_v<tok_t, token_array_end>) {
      return false; // end of array
    } else {
      std::cout << "unhandled token: " << typeid(tok_t).name() << std::endl;
      throw decoding_error(
          "invalid token, expected array element, comma, or end of array");
    }

    return true;
  });
  return result;
}

/*
 *  @tparam Obj the type of object to decode into
 *  @param begin iterator to the beginning of the JSON string
 *  @param end iterator to the end of the JSON string
 *  @returns a codie::json::object representing the JSON object
 */
export template <typename Obj = object, typename Iter>
Obj decode_object(Iter &begin, Iter end) requires
    std::is_same_v<typename Iter::value_type, char> {
  Obj result{};

  tokenize(begin, end, [&](auto &&tok) {
    using tok_t = std::decay_t<decltype(tok)>;
    if constexpr (std::is_same_v<tok_t, token_string>) // key
    {
      tokenize(begin, end, [&](auto &&tok2) -> bool {
        using tok2_t = std::decay_t<decltype(tok2)>;
        if constexpr (std::is_same_v<tok2_t, token_colon>) // :
        {
          tokenize(begin, end, [&](auto &&tok3) -> bool {
            using tok3_t = std::decay_t<decltype(tok3)>;
            if constexpr (std::is_same_v<tok3_t, token_string>) // string
            {
              if constexpr (std::is_same_v<Obj, object>) {
                result[std::move(tok.value)] = std::move(tok3.value);
              } else {
                throw decoding_error("invalid type for object");
              }
              return false;
            } else if constexpr (std::is_same_v<tok3_t,
                                                token_number_float>) // float
            {
              if constexpr (std::is_same_v<Obj, object>) {
                result[std::move(tok.value)] = tok3.value;
              } else {
                throw decoding_error("invalid type for object");
              }
              return false;
            } else if constexpr (std::is_same_v<
                                     tok3_t,
                                     token_number_signed_integer>) // int
            {
              if constexpr (std::is_same_v<Obj, object>) {
                result[std::move(tok.value)] = tok3.value;
              } else {
                throw decoding_error("invalid type for object");
              }
              return false;
            } else if constexpr (std::is_same_v<
                                     tok3_t,
                                     token_number_unsigned_integer>) // uint
            {
              if constexpr (std::is_same_v<Obj, object>) {
                result[std::move(tok.value)] = tok3.value;
              } else {
                throw decoding_error("invalid type for object");
              }
              return false;
            } else if constexpr (std::is_same_v<tok3_t,
                                                token_boolean_false>) // false
            {
              if constexpr (std::is_same_v<Obj, object>) {
                result[std::move(tok.value)] = false;
              } else {
                throw decoding_error("invalid type for object");
              }
              return false;
            } else if constexpr (std::is_same_v<tok3_t,
                                                token_boolean_true>) // true
            {
              if constexpr (std::is_same_v<Obj, object>) {
                result[std::move(tok.value)] = true;
              } else {
                throw decoding_error("invalid type for object");
              }
              return false;
            } else if constexpr (std::is_same_v<tok3_t, token_null>) // null
            {
              if constexpr (std::is_same_v<Obj, object>) {
                result[std::move(tok.value)] = value();
              } else {
                throw decoding_error("invalid type for object");
              }
              return false;
            } else if constexpr (std::is_same_v<tok3_t,
                                                token_array_begin>) // array
            {
              if constexpr (std::is_same_v<Obj, object>) {
                result[std::move(tok.value)] = decode_array(begin, end);
              } else {
                throw decoding_error("invalid type for object");
              }
              return false;
            } else if constexpr (std::is_same_v<tok3_t,
                                                token_object_begin>) // object
            {
              if constexpr (std::is_same_v<Obj, object>) {
                result[std::move(tok.value)] = decode_object(begin, end);
              } else {
                throw decoding_error("invalid type for object");
              }
              return false;
            } else {
              throw decoding_error(
                  "invalid token, expected string, float, int, uint, boolean, "
                  "null, array, or object");
            }

            throw decoding_error("unexpected token");
          });
        } else {
          throw decoding_error("expected colon");
        }

        return false;
      });
      return true;
    } else if constexpr (std::is_same_v<tok_t, token_object_end>) {
      return false;
    } else if constexpr (std::is_same_v<tok_t, token_comma>) {
      return true;
    } else {
      throw decoding_error("invalid token, expected key string");
    }
    return true;
  });

  return result;
}

/*
 * @tparam T the type of the value to decode, defaulted to
 * 'codie::json::value', can be a 'bool', 'double', 'std::uint64_t',
 * 'std::int64_t', 'std::string', 'codie::json::object', or
 * 'std::vector<codie::json::value>'
 * @tparam Iter the type of iterator to use, the iterator's value_type must be
 * a char.
 * @param begin iterator to the beginning of the input sequence
 * @param end iterator to the end of the input sequence
 * @returns a JSON value,
 */
export template <decodable T = value, typename Iter>
T decode(Iter &begin,
         Iter end) requires std::is_same_v<typename Iter::value_type, char> {
  T result;
  tokenize(begin, end, [&](auto &&tok) {
    using tok_t = std::decay_t<decltype(tok)>;

    if constexpr (std::is_same_v<tok_t, token_boolean_false>) {
      if constexpr (std::is_same_v<T, value> || std::is_same_v<T, bool>) {
        result = false;
        return false; // stop tokenizing
      } else {
        throw decoding_error(
            "invalid type T in decode, expected bool or codie::json::value");
      }
    } else if constexpr (std::is_same_v<tok_t, token_boolean_true>) {
      if constexpr (std::is_same_v<T, value> || std::is_same_v<T, bool>) {
        result = true;
        return false; // stop tokenizing
      } else {
        throw decoding_error(
            "invalid type T in decode, expected bool or codie::json::value");
      }
    } else if constexpr (std::is_same_v<tok_t, token_number_float>) {
      if constexpr (std::is_same_v<T, value> || std::is_same_v<T, double>) {
        result = tok.value;
        return false; // stop tokenizing
      } else {
        throw decoding_error("invalid type T in decode, expected double or "
                             "codie::json::value");
      }
    } else if constexpr (std::is_same_v<tok_t, token_number_unsigned_integer>) {
      if constexpr (std::is_same_v<T, value> ||
                    std::is_same_v<T, std::uint64_t>) {
        result = tok.value;
        return false; // stop tokenizing
      } else {
        throw decoding_error("invalid type T in decode, expected "
                             "std::uint64_t or codie::json::value");
      }
    } else if constexpr (std::is_same_v<tok_t, token_number_signed_integer>) {
      if constexpr (std::is_same_v<T, value> ||
                    std::is_same_v<T, std::int64_t>) {
        result = tok.value;
        return false; // stop tokenizing
      } else {
        throw decoding_error("invalid type T in decode, expected "
                             "std::int64_t or codie::json::value");
      }
    } else if constexpr (std::is_same_v<tok_t, token_string>) {
      if constexpr (std::is_same_v<T, value> ||
                    std::is_same_v<T, std::string>) {
        result = tok.value;
        return false; // stop tokenizing
      } else {
        throw decoding_error("invalid type T in decode, expected "
                             "std::string or codie::json::value");
      }
    } else if constexpr (std::is_same_v<tok_t, token_object_begin>) {
      if constexpr (std::is_same_v<T, value> || std::is_same_v<T, object>) {
        result = decode_object(begin, end);
        return false; // stop tokenizing
      } else {
        throw decoding_error("invalid type T in decode, expected "
                             "codie::json::object or codie::json::value");
      }
    } else if constexpr (std::is_same_v<tok_t, token_array_begin>) {
      if constexpr (std::is_same_v<T, value>) {
        result = decode_array<std::vector<value>>(begin, end);
        return false; // stop tokenizing
      } else if constexpr (meta::is_specialization_v<T, std::vector>) {
        result = decode_array<T>(begin, end);
        return false; // stop tokenizing
      } else {
        throw decoding_error(
            "invalid type T in decode, expected "
            "std::vector<codie::json::value> or codie::json::value");
      }
    } else {
      throw decoding_error("invalid token in decode, expected boolean, "
                           "number, string, object, array, or null");
    }
    return true;
  });
  return result;
}

export template <decodable T = value, typename Iter>
T decode(Iter &&begin,
         Iter end) requires std::is_same_v<typename Iter::value_type, char> {
  return decode<T>(begin, end);
}

/*
 * @tparam T the type of the value to decode, defaulted to
 * 'codie::json::value', can be a 'bool', 'double', 'std::uint64_t',
 * 'std::int64_t', 'std::string', 'codie::json::object', or
 * 'std::vector<codie::json::value>'
 * @tparam Range the type of range to use, the range's value_type must be a
 * char.
 * @param range range of characters to decode into a codie::json::value
 */
export template <decodable T = value, std::ranges::range Range>
    T decode(Range &range) requires std::is_same_v <
    std::ranges::range_value_t<Range>,
char > {
  auto begin = std::ranges::begin(range);
  auto end = std::ranges::end(range);
  return decode<T>(begin, end);
}

/*
 * @tparam T the type of the value to decode, defaulted to
 * 'codie::json::value', can be a 'bool', 'double', 'std::uint64_t',
 * 'std::int64_t', 'std::string', 'codie::json::object', or
 * 'std::vector<codie::json::value>'
 * @tparam Range the type of range to use, the range's value_type must be a
 * char.
 * @param range range of characters to decode into a codie::json::value
 */
export template <decodable T = value, typename Range>
    T decode(Range &&range) requires std::is_same_v <
    std::ranges::range_value_t<Range>,
char > {
  auto begin = std::ranges::begin(range);
  auto end = std::ranges::end(range);
  return decode<T>(begin, end);
}
} // namespace codie::json