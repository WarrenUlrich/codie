export module codie.json.tokens;

import <string_view>;
import <iterator>;
import <cctype>;

export namespace codie::json
{
	class token_exception : public std::exception
	{
	public:
		token_exception(std::string_view message) : message(message) {}

		const char *what() const noexcept override { return message.data(); }

	private:
		std::string_view message;
	};

	class unterminated_string_exception : public token_exception
	{
	public:
		unterminated_string_exception() : token_exception("unterminated string") {}
	};

	class token_t
	{
	};

	class token_null : public token_t
	{
	};

	class token_string : public token_t
	{
	public:
		std::string value;

		token_string(std::string string) : value(std::move(string)) {}
	};

	class token_number : public token_t
	{
	public:
		double value;

		token_number(double v) : value(v) {}
	};

	class token_boolean : public token_t
	{
	public:
		bool value;

		token_boolean(bool v) : value(v) {}
	};

	class token_array_begin : public token_t
	{
	};

	class token_array_end : public token_t
	{
	};

	class token_object_begin : public token_t
	{
	};

	class token_object_end : public token_t
	{
	};

	class token_comma : public token_t
	{
	};

	class token_colon : public token_t
	{
	};

	export template <std::input_iterator Iter>
	requires std::is_same_v<typename Iter::value_type, char>
	bool tokenize(Iter &begin, Iter end, const auto &pred)
	{
		while (begin != end)
		{
			switch (*begin)
			{
			case '{': // start of object
				if (!pred(token_object_begin{}))
					return false;
				++begin;
				break;
			case '}': // end of object
				if (!pred(token_object_end{}))
					return false;
				++begin;
				break;
			case '[': // start of array
				if (!pred(token_array_begin{}))
					return false;

				++begin;
				break;
			case ']': // end of array
				if (!pred(token_array_end{}))
					return false;

				++begin;
				break;
			case ',': // comma
				if (!pred(token_comma{}))
					return false;

				++begin;
				break;
			case ':':
				if (!pred(token_colon{}))
					return false;
				begin++;
				break;
			case '"': // string
			{
				std::string temp;
				++begin;

				while (begin != end && *begin != '\"')
					temp += *begin++;

				++begin;

				if (!pred(token_string(std::move(temp))))
					return false;

				break;
			}
			case 't': // true
			{
				// Contiguous iterator
				if constexpr (requires { std::contiguous_iterator<Iter>; }) //(std::is_same_v<std::iterator_traits<Iter>::iterator_category, std::contiguous_iterator_tag>)
				{
					if (begin + 4 < end && *(begin + 1) == 'r' && *(begin + 2) == 'u' && *(begin + 3) == 'e')
					{
						begin += 4;
						if (!pred(token_boolean(true)))
							return false;
					}
					else
						throw token_exception("invalid token, expected 'true'");
				}
				else // Non-contiguous
				{
					if (++begin != end && *begin == 'r')
						if (++begin != end && *begin == 'u')
							if (++begin != end && *begin == 'e')
							{
								if (!pred(token_boolean(true)))
									return false;

								break;
							}

					throw token_exception("invalid token, expected 'true'");
				}
				break;
			}
			case 'f': // false
			{
				// Continous iterator
				if constexpr (requires { std::contiguous_iterator<Iter>; })
				{
					if (begin + 5 < end && *(begin + 1) == 'a' && *(begin + 2) == 'l' && *(begin + 3) == 's' && *(begin + 4) == 'e')
					{
						begin += 5;

						if (!pred(token_boolean(false)))
							return false;
					}
					else
						throw token_exception("invalid token, expected 'false'");
				}
				else // Non-contiguous iterator
				{
					if (++begin != end && *begin == 'a')
						if (++begin != end && *begin == 'l')
							if (++begin != end && *begin == 's')
								if (++begin != end && *begin == 'e')
								{
									if (!pred(token_boolean(false)))
										return false;

									break;
								}

					throw token_exception("invalid token, expected 'true'");
				}
				break;
			}
			case 'n': // null
			{
				// Continous iterator
				if constexpr (requires { std::contiguous_iterator<Iter>; })
				{
					if (begin + 4 < end && *(begin + 1) == 'u' && *(begin + 2) == 'l' && *(begin + 3) == 'l')
					{
						if (!pred(token_null()))
							return false;
						begin += 4;
					}
					else
						throw token_exception("invalid token, expected 'null'");
				}
				else // Non-contiguous iterator
				{
					if (++begin != end && *begin == 'u')
						if (++begin != end && *begin == 'l')
							if (++begin != end && *begin == 'l')
							{
								if (!pred(token_null()))
									return false;

								break;
							}

					throw token_exception("invalid token, expected 'null'");
				}
			}
			case '-': // number
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			{
				// Continous iterator
				if constexpr (requires { std::contiguous_iterator<Iter>; })
				{
					const Iter start = begin;

					while (begin != end && std::isdigit(*begin))
						begin++;

					if (*begin == '.')
					{
						begin++;
						while (begin != end && std::isdigit(*begin))
							begin++;

						if (*begin == 'e' || *begin == 'E')
						{
							begin++;
							if (*begin == '-' || *begin == '+')
								begin++;

							while (begin != end && std::isdigit(*begin))
								begin++;
						}
					}

					const auto str = std::string_view(start, begin);

					if (!pred(token_number(atof(str.data()))))
						return false;

					break;
				}
				else // Non-continiguous iterator
				{
					std::string temp;

					temp += *begin++;

					while (begin != end && std::isdigit(*begin))
						temp += *begin++;

					if (*begin == '.')
					{
						temp += *begin++;
						while (begin != end && std::isdigit(*begin))
							temp += *begin++;

						if (*begin == 'e' || *begin == 'E')
						{
							temp += *begin++;
							if (*begin == '-' || *begin == '+')
								temp += *begin++;

							while (begin != end && std::isdigit(*begin))
								temp += *begin++;
						}
					}

					if (!pred(token_number(atof(temp.c_str()))))
						return false;

					break;
				}
			}
			default:
				++begin;
				break;
			}
		}
		return true;
	}

	export template <std::input_iterator Iter>
	requires std::is_same_v<typename Iter::value_type, char>
	bool tokenize(Iter &&begin, Iter end, const auto &pred)
	{
		auto begin_ = std::move(begin);
		return tokenize(begin, end, pred);
	}

	export template <std::ranges::input_range Range>
	requires std::is_same_v<typename Range::value_type, char>
	bool tokenize(Range &range, const auto &pred)
	{
		return tokenize(std::ranges::begin(range), std::ranges::end(range), pred);
	}
}