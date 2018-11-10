#include <iostream>
#include <string_view>
#include <type_traits>
#include <typeinfo>

//store and manipulate types
template <class... Ts>
struct Type_list {
	private:
	//helper to get type at index
	template <int index, class... Args>
	struct Get_at;
	template <class T, class... Args>
	struct Get_at<0, T, Args...> {
		using Type = T;
	};
	template <int index>
	struct Get_at<index> {};
	template <int index, class T, class... Args>
	struct Get_at<index, T, Args...> {
		using Type = typename Get_at<index - 1, Args...>::Type;
	};
	//helper to remove a type
	template <class T, class... Args>
	struct Remove;
	template <class T>
	struct Remove<T> {
		using types = Type_list<>;
	};
	template <class T, class Head, class... Tail>
	struct Remove<T, Head, Tail...> {
		using types = typename std::conditional_t<std::is_same_v<T, Head>, typename Remove<T, Tail...>::types,
												  typename Remove<T, Tail...>::types::template prepend<Head>>;
	};

	public:
	//this should be private but clang-7 is buggy
	template <class... Args>
	static auto concat(Type_list<Args...>) -> Type_list<Ts..., Args...>;

	public:
	template <class... T>
	using append = Type_list<Ts..., T...>;
	template <class... T>
	using prepend = Type_list<T..., Ts...>;
	template <template <typename...> class F>
	using apply = Type_list<F<Ts>...>;
	template <template <typename...> class T>
	using instantiate = T<Ts...>;
	template <int index>
	using at = typename Get_at < index<0 ? sizeof...(Ts) + index : index, Ts...>::Type;
	template <class T>
	using concatenate = decltype(concat(T{}));
	template <class T>
	using remove = typename Remove<T, Ts...>::types;
};

//helpers to extract callable information
namespace detail {
	//return type
	template <class Return_type, class... Args>
	auto get_return_type(Return_type (*)(Args...)) -> Return_type;
	template <class Return_type, class Class_type, class... Args>
	auto get_return_type(Return_type (Class_type::*)(Args...)) -> Return_type;
	template <class Return_type, class Class_type, class... Args>
	auto get_return_type(Return_type (Class_type::*)(Args...) const) -> Return_type;
	template <class T>
	auto get_return_type(T &&) -> decltype(get_return_type(&std::remove_reference_t<T>::operator()));
	template <class T>
	using Return_type = decltype(get_return_type(std::declval<T>()));

	//class type
	template <class Return_type, class Class_type, class... Args>
	auto get_class_type(Return_type (Class_type::*)(Args...)) -> Class_type;
	template <class Return_type, class Class_type, class... Args>
	auto get_class_type(Return_type (Class_type::*)(Args...) const) -> Class_type;
	template <class T>
	auto get_class_type(T &&) -> decltype(get_class_type(&std::remove_reference_t<T>::operator()));
	template <class T>
	using Class_type = decltype(get_class_type(std::declval<T>()));
	template <class T>
	auto get_has_class_type(T &&t) -> decltype(get_class_type(std::forward<T>(t)), std::true_type{});
	std::false_type get_has_class_type(...);
	template <class T>
	constexpr bool has_class_type = decltype(get_has_class_type(std::declval<T>()))::value;

	//arguments
	template <class Return_type, class... Args>
	auto get_args(Return_type (*)(Args...)) -> Type_list<Args...>;
	template <class Return_type, class Class_type, class... Args>
	auto get_args(Return_type (Class_type::*)(Args...)) -> Type_list<Args...>;
	template <class Return_type, class Class_type, class... Args>
	auto get_args(Return_type (Class_type::*)(Args...) const) -> Type_list<Args...>;
	template <class T>
	auto get_args(T &&) -> decltype(get_args(&std::remove_reference_t<T>::operator()));
	template <class T>
	using Args = decltype(get_args(std::declval<T>()));
} // namespace detail

//struct to hold callable information
template <class T, bool has_class>
struct Callable_info;

template <class T>
struct Callable_info<T, true> {
	Callable_info(T) {}
	Callable_info() = default;
	using Return_type = detail::Return_type<T>;
	using Class_type = detail::Class_type<T>;
	constexpr static bool has_class_type = true;
	using Args = detail::Args<T>;
};

template <class T>
struct Callable_info<T, false> {
	Callable_info(T) {}
	Callable_info() = default;
	using Return_type = detail::Return_type<T>;
	constexpr static bool has_class_type = false;
	using Args = detail::Args<T>;
};

template <class T>
Callable_info(T)->Callable_info<T, detail::has_class_type<T>>;

//printing type names, from https://stackoverflow.com/a/20170989/3484570
template <class T>
constexpr std::string_view type_name() {
	using namespace std;
#ifdef __clang__
	string_view p = __PRETTY_FUNCTION__;
	return string_view(p.data() + 34, p.size() - 34 - 1);
#elif defined(__GNUC__)
	string_view p = __PRETTY_FUNCTION__;
#if __cplusplus < 201402
	return string_view(p.data() + 36, p.size() - 36 - 1);
#else
	return string_view(p.data() + 49, p.find(';', 49) - 49);
#endif
#elif defined(_MSC_VER)
	string_view p = __FUNCSIG__;
	return string_view(p.data() + 84, p.size() - 84 - 7);
#endif
}

