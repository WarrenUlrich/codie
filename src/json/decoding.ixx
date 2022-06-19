export module codie.json.decoding;

import <iterator>;
import <vector>;
import <stdexcept>;

import codie.json.tokens;
import codie.json.object;

export namespace codie::json
{
	template <typename Object, std::input_iterator Iter>
	Object decode_object(Iter &begin, Iter end);

	template <typename T = value, std::input_iterator Iter>
	std::vector<T> decode_array(Iter &begin, Iter end)
	{
		std::vector<T> result;

		tokenize(begin, end, [&](auto &&tok) -> bool
				 {
			using tok_t = std::decay_t<decltype(tok)>;
			if constexpr (std::is_same_v<tok_t, codie::json::token_boolean>)
			{
				result.emplace_back(tok.value);
				return true;
			}
			else if constexpr (std::is_same_v<tok_t, codie::json::token_number>)
			{
				result.emplace_back(tok.value);
				return true;
			}
			else if constexpr (std::is_same_v<tok_t, codie::json::token_string>)
			{
				result.emplace_back(tok.value);
				return true;
			}
			else if constexpr (std::is_same_v<tok_t, codie::json::token_object_begin>)
			{
				result.emplace_back(decode_object(++begin, end));
				return true;
			}
			else if constexpr (std::is_same_v<tok_t, codie::json::token_array_begin>)
			{
				result.emplace_back(decode_array(++begin, end));
				return true;
			}
			else if constexpr (std::is_same_v<tok_t, codie::json::token_comma>)
			{
				return true;
			}
			else if constexpr (std::is_same_v<tok_t, codie::json::token_array_end>)
			{
				begin++;
				return false;
			}
			else
			{
				throw std::runtime_error("unexpected token, expected ',' or ']'");
			} });
		return result;
	}

	template <typename Object = object, std::input_iterator Iter>
	Object decode_object(Iter &begin, Iter end)
	{
		Object result;

		tokenize(begin, end, [&](auto &&tok) -> bool
				 {
				using tok_t = std::decay_t<decltype(tok)>;
				if constexpr (std::is_same_v<tok_t, token_string>)
				{
					tokenize(begin, end, [&](auto&& tok2) -> bool
						{
							using tok2_t = std::decay_t<decltype(tok2)>;
							if constexpr (std::is_same_v<tok2_t, token_colon>)
							{
								return tokenize(++begin, end, [&](auto&& tok3) -> bool
									{
										using tok3_t = std::decay_t<decltype(tok3)>;
										if constexpr (std::is_same_v<tok3_t, token_boolean>)
										{
											result.emplace(tok.value, tok3.value);
										}
										else if constexpr (std::is_same_v<tok3_t, token_string>)
										{
											result.emplace(tok.value, tok3.value);
										}
										else if constexpr (std::is_same_v<tok3_t, token_number>)
										{
											result.emplace(tok.value, tok3.value);
										}
										else if constexpr (std::is_same_v<tok3_t, token_object_begin>)
										{
											result.emplace(tok.value, decode_object(++begin, end));
										}
										else if constexpr (std::is_same_v<tok3_t, token_array_begin>)
										{
											result.emplace(tok.value, decode_array(++begin, end));
										}
										else if constexpr (std::is_same_v<tok3_t, token_null>)
										{
											result.emplace(tok.value, value());
										}
										else
										{
											throw std::runtime_error(
												"unexpected token, expected:"
												"	'codie::json::token_boolean'"
												"	'codie::json::token_string'"
												"	'codie::json::token_number'"
												"	'codie::json::token_object_begin'"
												"	'codie::json::token_array_begin'"
												"	'codie::json::token_null"
											);
										}

										return tokenize(begin, end, [&](auto&& tok4) -> bool
											{
												using tok4_t = std::decay_t<decltype(tok4)>;
												if constexpr (std::is_same_v<tok4_t, token_comma>)
												{
													begin++;
													return false;
												}
												else if constexpr (std::is_same_v<tok4_t, token_object_end>)
												{
													return false;
												}
												else
												{
													throw std::runtime_error(
														"unexpected token, expected:\n"
														"	'codie::json::token_comma'\n"
														"	'codie::json::token_object_end'"
													);
												}
											});
									});
							}
							else
							{
								throw std::runtime_error("unexpected token, expected 'codie::json::token_colon'");
							}
						});

					return true;
				}
				else if constexpr (std::is_same_v<tok_t, token_object_end>)
				{
					return false;
				}
				else
				{
					throw std::runtime_error("unexpected token, expected 'codie::json::token_string'");
				} });
		return result;
	}

	template <typename T = value, std::input_iterator Iter>
	T decode(Iter &begin, Iter end)
	{
		T result;

		tokenize(begin, end, [&](auto &&tok) -> bool
				 {
			using tok_t = std::decay_t<decltype(tok)>;

			if constexpr (std::is_same_v<tok_t, token_object_begin>)
			{
				if constexpr (std::is_same_v<T, object> || std::is_same_v<T, value>)
				{
					result = decode_object(++begin, end);
					return false;
				}
				else
				{
					throw std::exception("invalid type T, expected 'codie::json::object' or 'codie::json::value'");
				}
			}
			else if constexpr (std::is_same_v<tok_t, token_array_begin>)
			{
				if constexpr (std::is_same_v<T, std::vector<value>> || std::is_same_v<T, value>)
				{
					result = decode_array(++begin, end);
					return false;
				}
				else
				{
					throw std::exception("invalid type T, expected 'std::vector<codie::json::value>' or 'codie::json::value'");
				}
			}
			else if constexpr (std::is_same_v<tok_t, token_boolean>)
			{
				if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, value>)
				{
					result = tok.value;
					return false;
				}
				else
				{
					throw std::exception("invalid type T, expected 'bool' or 'codie::json::value'");
				}
			}
			else if constexpr (std::is_same_v<tok_t, token_string>)
			{
				if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, value>)
				{
					result = tok.value;
					return false;
				}
				else
				{
					throw std::exception("invalid type T, expected 'std::string' or 'codie::json::value'");
				}
			}
			else if constexpr (std::is_same_v<tok_t, token_number>)
			{
				if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>)
				{
					result = static_cast<T>(tok.value);
					return false;
				}
				else if constexpr (std::is_same_v<T, value>)
				{
					result = tok.value;
					return false;
				}
				else
				{
					throw std::exception("invalid type T, expected integral type, floating point type, or 'codie::json::value'");
				}
			}

			return true; });

		return result;
	}
}