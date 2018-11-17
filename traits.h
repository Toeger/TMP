#pragma once

#include "type_list.h"

#include <type_traits>

namespace TMP {
	template <class T, template <class...> class Template>
	struct is_type_specialization : std::false_type {};
	template <template <class...> class Template, class... Args>
	struct is_type_specialization<Template<Args...>, Template> : std::true_type {};
	template <class T, template <class...> class Template>
	constexpr bool is_type_specialization_v = is_type_specialization<T, Template>::value;

	template <class T, template <class...> class Template>
	struct get_type_specialization;
	template <template <class...> class Template, class... Args>
	struct get_type_specialization<Template<Args...>, Template> {
		using Types = Type_list<Args...>;
	};
	template <class T, template <class...> class Template>
	using get_type_specialization_t = typename get_type_specialization<T, Template>::Types;
} // namespace TMP