#pragma once

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

	//TODO: sort, remove_if
};
