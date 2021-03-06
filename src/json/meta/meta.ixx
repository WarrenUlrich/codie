export module codie.json.meta;

import<type_traits>;

export namespace codie::json::meta {
template <typename Test, template <typename...> class Ref>
struct is_specialization : std::false_type {};

template <template <typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

template <typename Test, template <typename...> class Ref>
constexpr bool is_specialization_v = is_specialization<Test, Ref>::value;
} // namespace codie::json::meta