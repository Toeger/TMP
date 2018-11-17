#pragma once

#include <type_traits>

namespace TMP {
	namespace detail {
		template <class T, template <class...> class Template>
		struct Is_type_specialization : std::false_type {};
		template <template <class...> class Template, class... Args>
		struct Is_type_specialization<Template<Args...>, Template> : std::true_type {};

	} // namespace detail

	template <class T, template <class...> class Template>
	auto is_type_specialization() -> typename detail::Is_type_specialization<T, Template>::type;
	template <class T, class U>
	std::false_type is_type_specialization();

#define TMP_IS_TYPE_SPECIALIZATION_V(TYPE, ...) decltype(TMP::is_type_specialization<TYPE, __VA_ARGS__>())::value
} // namespace TMP