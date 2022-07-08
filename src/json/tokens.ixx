export module codie.json.tokens;

import<string_view>;
import<iterator>;
import<cctype>;
import<charconv>;
import<stdexcept>;
import<iostream>;

export namespace codie::json {
struct token_object_begin {};

struct token_object_end {};

struct token_array_begin {};

struct token_array_end {};

struct token_comma {};

struct token_colon {};

struct token_boolean_true {};

struct token_boolean_false {};

struct token_null {};

struct token_string {
  std::string value;
};

struct token_number_float {
  double value;
};

struct token_number_unsigned_integer {
  std::uint64_t value;
};

struct token_number_signed_integer {
  std::int64_t value;
};

// Input iterator tokenizer.
export template <typename Iter>
requires std::input_iterator<Iter> &&
    std::same_as < typename std::iterator_traits<Iter>::value_type,
char > bool tokenize(Iter &begin, Iter end, auto &&predicate) {
  while (begin != end) {
    switch (*begin) {
    case '{':
      ++begin;
      if (!predicate(token_object_begin{}))
        return false;
      break;
    case '}':
      ++begin;
      if (!predicate(token_object_end{}))
        return false;
      break;
    case '[':
      ++begin;
      if (!predicate(token_array_begin{}))
        return false;
      break;
    case ']':
      ++begin;
      if (!predicate(token_array_end{}))
        return false;
      break;
    case ',':
      ++begin;
      if (!predicate(token_comma{}))
        return false;
      break;
    case ':':
      ++begin;
      if (!predicate(token_colon{}))
        return false;
      break;
    case 't':
      if (++begin == end || *begin != 'r')
        throw std::runtime_error("expected 'true'");

      if (++begin == end || *begin != 'u')
        throw std::runtime_error("expected 'true'");

      if (++begin == end || *begin != 'e')
        throw std::runtime_error("expected 'true'");

      if (!predicate(token_boolean_true{}))
        return false;

      break;
    case 'f':
      if (++begin == end || *begin != 'a')
        throw std::runtime_error("expected 'false'");

      if (++begin == end || *begin != 'l')
        throw std::runtime_error("expected 'false'");

      if (++begin == end || *begin != 's')
        throw std::runtime_error("expected 'false'");

      if (++begin == end || *begin != 'e')
        throw std::runtime_error("expected 'false'");

      if (!predicate(token_boolean_false{}))
        return false;

      break;
    case 'n':
      if (++begin == end || *begin != 'u')
        throw std::runtime_error("expected 'null'");

      if (++begin == end || *begin != 'l')
        throw std::runtime_error("expected 'null'");

      if (++begin == end || *begin != 'l')
        throw std::runtime_error("expected 'null'");

      if (!predicate(token_null{}))
        return false;

      break;
    case '"': {
      ++begin;
      if (begin == end)
        throw std::runtime_error("expected string");

      // check if string is empty.
      if (*begin == '"') {
        ++begin;
        if (!predicate(token_string{}))
          return false;
        break;
      }

      std::string temp;

      while (begin != end && *begin != '"') {
        if (*begin == '\\') {
          temp += *begin++;
          if (begin == end)
            throw std::runtime_error("unexpected end of input");
          temp += *begin++;
        } else {
          temp += *begin++;
        }
      }

      if (begin == end)
        throw std::runtime_error("unexpected end of input");

      ++begin;
      if (!predicate(token_string{std::move(temp)}))
        return false;

      // if (!predicate(temp))
      //     return false;
      break;
    }
    case '-': {
      std::string temp;
      temp += *begin++;
      if (begin == end)
        throw std::runtime_error("unexpected end of input");

      while (begin != end && std::isdigit(*begin))
        temp += *begin++;

      if (begin == end)
        throw std::runtime_error("unexpected end of input");

      bool is_float = false;
      if (*begin == '.') {
        is_float = true;
        temp += *begin++;
        if (begin == end)
          throw std::runtime_error("unexpected end of input");

        while (begin != end && std::isdigit(*begin))
          temp += *begin++;

        if (begin == end)
          throw std::runtime_error("unexpected end of input");

        if (*begin == 'e' || *begin == 'E') {
          temp += *begin++;
          if (begin == end)
            throw std::runtime_error("unexpected end of input");

          if (*begin == '+' || *begin == '-')
            temp += *begin++;

          if (begin == end)
            throw std::runtime_error("unexpected end of input");

          while (begin != end && std::isdigit(*begin))
            temp += *begin++;

          if (begin == end)
            throw std::runtime_error("unexpected end of input");
        }
      }

      if (is_float) {
        double value;

        auto [ptr, ec] =
            std::from_chars(temp.data(), temp.data() + temp.size(), value);
        if (ec != std::errc())
          throw std::runtime_error("invalid number token");

        if (!predicate(token_number_float{value}))
          return false;
      } else {
        std::int64_t value;
        auto [ptr, ec] =
            std::from_chars(temp.data(), temp.data() + temp.size(), value);
        if (ec != std::errc())
          throw std::runtime_error("invalid number token");

        if (!predicate(token_number_signed_integer{value}))
          return false;
      }
      break;
    }
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
      std::string temp;
      temp += *begin++;
      if (begin == end)
        throw std::runtime_error("unexpected end of input");

      while (begin != end && std::isdigit(*begin))
        temp += *begin++;

      if (begin == end)
        throw std::runtime_error("unexpected end of input");

      bool is_float = false;
      if (*begin == '.') {
        is_float = true;
        temp += *begin++;
        if (begin == end)
          throw std::runtime_error("unexpected end of input");

        while (begin != end && std::isdigit(*begin))
          temp += *begin++;

        if (begin == end)
          throw std::runtime_error("unexpected end of input");

        if (*begin == 'e' || *begin == 'E') {
          temp += *begin++;
          if (begin == end)
            throw std::runtime_error("unexpected end of input");

          if (*begin == '+' || *begin == '-')
            temp += *begin++;

          if (begin == end)
            throw std::runtime_error("unexpected end of input");

          while (begin != end && std::isdigit(*begin))
            temp += *begin++;

          if (begin == end)
            throw std::runtime_error("unexpected end of input");
        }
      }

      if (is_float) {
        double value;

        auto [ptr, ec] =
            std::from_chars(temp.data(), temp.data() + temp.size(), value);
        if (ec != std::errc())
          throw std::runtime_error("invalid number token");

        if (!predicate(token_number_float{value}))
          return false;
      } else {
        std::uint64_t value;
        auto [ptr, ec] =
            std::from_chars(temp.data(), temp.data() + temp.size(), value);
        if (ec != std::errc())
          throw std::runtime_error("invalid number token");

        if (!predicate(token_number_unsigned_integer{value}))
          return false;
      }
      break;
    }
    default:
      ++begin;
      break;
    }
  }
  return true;
}

// Contiguous iterator tokenizer.
export template <typename Iter>
requires std::contiguous_iterator<Iter> &&
    std::same_as < typename std::iterator_traits<Iter>::value_type,
char > bool tokenize(Iter &begin, Iter end, auto &&predicate) {
  while (begin != end) {
    switch (*begin) {
    case '{':
      ++begin;
      if (!predicate(token_object_begin{}))
        return false;
      break;
    case '}':
      ++begin;
      if (!predicate(token_object_end{}))
        return false;
      break;
    case '[':
      ++begin;
      if (!predicate(token_array_begin{}))
        return false;
      break;
    case ']':
      ++begin;
      if (!predicate(token_array_end{}))
        return false;
      break;
    case ',':
      ++begin;
      if (!predicate(token_comma{}))
        return false;
      break;
    case ':':
      ++begin;
      if (!predicate(token_colon{}))
        return false;
      break;
    case 't':
      if (begin + 4 < end && *(begin + 1) == 'r' && *(begin + 2) == 'u' &&
          *(begin + 3) == 'e') {
        begin += 4;
        if (!predicate(token_boolean_true{}))
          return false;

        break;
      } else {
        throw std::runtime_error("invalid token");
      }
    case 'f':
      if (begin + 5 < end && *(begin + 1) == 'a' && *(begin + 2) == 'l' &&
          *(begin + 3) == 's' && *(begin + 4) == 'e') {
        begin += 5;
        if (!predicate(token_boolean_false{}))
          return false;

        break;
      } else {
        throw std::runtime_error("invalid token");
      }
    case 'n':
      if (begin + 4 < end && *(begin + 1) == 'u' && *(begin + 2) == 'l' &&
          *(begin + 3) == 'l') {
        begin += 4;
        if (!predicate(token_null{}))
          return false;

        break;
      } else {
        throw std::runtime_error("invalid token");
      }
    case '"': {
      // parse string
      ++begin;
      if (begin == end)
        throw std::runtime_error("unexpected end of input");

      // check if string is empty
      if (*begin == '"') {
        ++begin;
        if (!predicate(token_string{}))
          return false;
        break;
      }

      auto string_begin = begin;
      while (begin != end && *begin != '"') {
        if (*begin == '\\') {
          ++begin;
          if (begin == end)
            throw std::runtime_error("invalid string");
          ++begin;
        } else {
          ++begin;
        }
      }

      if (begin == end)
        throw std::runtime_error("invalid string");

      if (!predicate(token_string{std::string{string_begin, begin++}}))
        return false;
      break;
    }
    case '-': {
      // parse signed integer/float
      const auto start = begin++;

      if (begin == end)
        throw std::runtime_error("invalid number token");

      while (begin != end && std::isdigit(*begin))
        ++begin;

      if (begin == end)
        throw std::runtime_error("invalid number token");

      bool is_float = false;
      if (*begin == '.') {
        is_float = true;
        ++begin;
        if (begin == end || !std::isdigit(*begin))
          throw std::runtime_error("invalid number token");

        while (begin != end && std::isdigit(*begin))
          ++begin;

        if (begin == end)
          throw std::runtime_error("invalid number token");

        // e-notation
        if (*begin == 'e' || *begin == 'E') {
          ++begin;
          // check for +/-
          if (begin == end || (*begin != '+' && *begin != '-'))
            throw std::runtime_error("invalid number token");

          ++begin;
          while (begin != end && std::isdigit(*begin))
            ++begin;
        }
      }

      const auto str = std::string_view{start, begin};

      if (is_float) {
        double value;

        auto [_, ec] =
            std::from_chars(str.data(), str.data() + str.size(), value);
        if (ec != std::errc())
          throw std::runtime_error("invalid number token");

        if (!predicate(token_number_float{value}))
          return false;
      } else {
        std::int64_t value;
        auto [_, ec] =
            std::from_chars(str.data(), str.data() + str.size(), value);
        if (ec != std::errc())
          throw std::runtime_error("invalid number token");

        if (!predicate(token_number_signed_integer{value}))
          return false;
      }

      break;
    }
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
      // parse unsigned integer/float
      const auto start = begin++;

      if (begin == end)
        throw std::runtime_error("invalid number token");

      while (begin != end && std::isdigit(*begin))
        ++begin;

      if (begin == end)
        throw std::runtime_error("invalid number token");

      bool is_float = false;
      if (*begin == '.') {
        is_float = true;
        ++begin;
        if (begin == end || !std::isdigit(*begin))
          throw std::runtime_error("invalid number token");

        while (begin != end && std::isdigit(*begin))
          ++begin;

        if (begin == end)
          throw std::runtime_error("invalid number token");

        // e-notation
        if (*begin == 'e' || *begin == 'E') {
          ++begin;
          // check for +/-
          if (begin == end || (*begin != '+' && *begin != '-'))
            throw std::runtime_error("invalid number token");

          ++begin;
          while (begin != end && std::isdigit(*begin))
            ++begin;
        }
      }

      const auto str = std::string_view{start, begin};

      if (is_float) {
        double value;

        auto [_, ec] =
            std::from_chars(str.data(), str.data() + str.size(), value);
        if (ec != std::errc())
          throw std::runtime_error("invalid number token");

        if (!predicate(token_number_float{value}))
          return false;
      } else {
        std::uint64_t value;
        auto [_, ec] =
            std::from_chars(str.data(), str.data() + str.size(), value);
        if (ec != std::errc())
          throw std::runtime_error("invalid number token");

        if (!predicate(token_number_unsigned_integer{value}))
          return false;
      }

      break;
    }
    default:
      ++begin;
      break;
    }
  }
  return true;
}
} // namespace codie::json