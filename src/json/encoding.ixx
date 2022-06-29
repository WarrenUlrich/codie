export module codie.json.encoding;

import <string>;
import <string_view>;
import <stdexcept>;
import <optional>;

import codie.json.object;

export namespace codie::json
{
    std::string encode_string(std::string_view str)
    {
        std::string result;
        result += '"';
        for (auto c : str)
        {
            if (c == '"')
            {
                result += "\\\"";
            }
            else if (c == '\\')
            {
                result += "\\\\";
            }
            else if (c == '\b')
            {
                result += "\\b";
            }
            else if (c == '\f')
            {
                result += "\\f";
            }
            else if (c == '\n')
            {
                result += "\\n";
            }
            else if (c == '\r')
            {
                result += "\\r";
            }
            else if (c == '\t')
            {
                result += "\\t";
            }
            else if (c < 0x20)
            {
                result += "\\u";
                result += std::to_string(c);
            }
            else
            {
                result += c;
            }
        }

        result += '"';
        return result;
    }

    template <typename T>
    std::string encode(const T &val)
    {
        if constexpr (std::is_same_v<T, std::nullopt_t>) // nullopt
        {
            return "null";
        }
        else if constexpr (std::is_same_v<T, bool>) // bool
        {
            return val ? "true" : "false";
        }
        else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) // string
        {
            std::string temp{};
            temp += '"';
            for (auto c : val)
            {
                switch (c)
                {
                case '"':
                    temp += "\\\"";
                    break;
                case '\\':
                    temp += "\\\\";
                    break;
                case '\b':
                    temp += "\\b";
                    break;
                case '\f':
                    temp += "\\f";
                    break;
                case '\n':
                    temp += "\\n";
                    break;
                case '\r':
                    temp += "\\r";
                    break;
                case '\t':
                    temp += "\\t";
                    break;
                case '\0':
                    temp += "\\u0000";
                    break;
                default:
                    temp += c;
                    break;
                }
            }

            temp += '"';
            return temp;
        }
        else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) // ints/floats
        {
            return std::to_string(val);
        }
        else if constexpr (std::is_same_v<T, object>) // json::object
        {
            std::string temp{};
            temp += '{';

            for (auto it = val.begin(); it != val.end(); ++it)
            {
                temp += encode(it->first);
                temp += ':';
                temp += encode(it->second);

                if (std::next(it) != val.end())
                {
                    temp += ',';
                }
            }

            temp += '}';
            return temp;
        }
        else if constexpr (std::is_same_v<T, std::vector<value>>) // std::vector<json::value>
        {
            std::string temp{};
            temp += '[';
            for (auto it = val.begin(); it != val.end(); ++it)
            {
                temp += encode(*it);

                if (it != val.end() - 1)
                {
                    temp += ',';
                }
            }

            temp += ']';
            return temp;
        }
        else if constexpr (std::is_same_v<T, value>) // json::value
        {
            if (val.is_null())
            {
                return "null";
            }
            else if (val.is_bool())
            {
                return val.as<bool>() ? "true" : "false";
            }
            else if (val.is_number())
            {
                return std::to_string(val.as<double>());
            }
            else if (val.is_string())
            {
                return encode(val.as<std::string>());
            }
            else if (val.is_object())
            {
                return encode(val.as<object>());
            }
            else if (val.is_array())
            {
                return encode(val.as<std::vector<value>>());
            }
        }
        else
        {
            throw std::runtime_error("invalid type T for json::encode.");
        }
    }
}