
export module codie.json.object;

import <optional>;
import <vector>;
import <unordered_map>;
import <variant>;

export namespace codie::json
{
	class value;

	class object
	{
	public:
		using map_t = std::unordered_map<std::string, value>;

		using key_type = map_t::key_type;

		using size_type = map_t::size_type;

		using mapped_type = map_t::mapped_type;

		using value_type = map_t::value_type;

		using iterator = map_t::iterator;

		using const_iterator = map_t::const_iterator;

		object() = default;

		object(const map_t& map) : _map(map) {}

		object(map_t&& map) : _map(std::move(map)) {}

		void clear() noexcept;

		void insert(const value_type& val);

		void insert(const value_type&& val);

		void emplace(const key_type& key, const mapped_type& value);

		void emplace(const key_type& key, mapped_type&& value);

		void emplace(key_type&& key, const mapped_type& value);

		void emplace(key_type&& key, mapped_type&& value);

		mapped_type& at(const key_type& key);

		const mapped_type& at(const key_type& key) const;

		mapped_type& operator[](const key_type& key);

		mapped_type& operator[](const key_type&& key);

		size_type count(const key_type& key) const;

		bool contains(const key_type& key) const;

		iterator begin() noexcept;

		const_iterator begin() const noexcept;

		iterator end() noexcept;

		const_iterator end() const noexcept;

	private:
		std::unordered_map<std::string, value> _map;
	};

	class value
	{
	public:
		using variant_type = std::variant<std::nullopt_t, bool, double, std::string, object, std::vector<value>>;

		value() : _value(std::nullopt) {}

		value(bool b) : _value(b) {}

		template<std::integral T>
		value(T i) : _value(static_cast<double>(i)) {}

		template<std::floating_point T>
		value(T f) : _value(static_cast<double>(f)) {}

		value(const char* s) : _value(std::string(s)) {}

		value(std::string_view s) : _value(std::string(s)) {}

		value(std::string str) : _value(std::move(str)) {}

		value(const object& obj) : _value(obj) {}

		value(object&& obj) : _value(std::move(obj)) {}

		value(const std::vector<value>& vec) : _value(vec) {}

		value(std::vector<value>&& vec) : _value(std::move(vec)) {}

		bool is_null() const
		{
			return std::holds_alternative<std::nullopt_t>(_value);
		}

		bool is_bool() const
		{
			return std::holds_alternative<bool>(_value);
		}

		bool is_number() const
		{
			return std::holds_alternative<double>(_value);
		}

		bool is_string() const
		{
			return std::holds_alternative<std::string>(_value);
		}

		bool is_array() const
		{
			return std::holds_alternative<std::vector<value>>(_value);
		}

		bool is_object() const
		{
			return std::holds_alternative<object>(_value);
		}

		template<typename T>
		T as() const
		{
			return std::get<T>(_value);
		}

	private:
		variant_type _value;
	};

	void object::clear() noexcept
	{
		_map.clear();
	}

	void object::insert(const object::value_type& value)
	{
		_map.insert(value);
	}

	void object::insert(const object::value_type&& value)
	{
		_map.insert(std::move(value));
	}

	void object::emplace(const object::key_type& key, const object::mapped_type& value)
	{
		_map.emplace(key, value);
	}

	void object::emplace(const object::key_type& key, object::mapped_type&& value)
	{
		_map.emplace(key, value);
	}

	void object::emplace(object::key_type&& key, const object::mapped_type& value)
	{
		_map.emplace(std::move(key), value);
	}

	void object::emplace(object::key_type&& key, object::mapped_type&& value)
	{
		_map.emplace(std::move(key), std::move(value));
	}

	object::mapped_type& object::at(const key_type& key)
	{
		return _map.at(key);
	}

	const object::mapped_type& object::at(const key_type& key) const
	{
		return _map.at(key);
	}

	object::mapped_type& object::operator[](const key_type& key)
	{
		return _map[key];
	}

	object::mapped_type& object::operator[](const key_type&& key)
	{
		return _map[std::move(key)];
	}

	object::size_type object::count(const key_type& key) const
	{
		return _map.count(key);
	}

	bool object::contains(const key_type& key) const
	{
		return _map.contains(key);
	}

	object::iterator object::begin() noexcept
	{
		return _map.begin();
	}

	object::const_iterator object::begin() const noexcept
	{
		return _map.begin();
	}

	object::iterator object::end() noexcept
	{
		return _map.end();
	}

	object::const_iterator object::end() const noexcept
	{
		return _map.end();
	}
}