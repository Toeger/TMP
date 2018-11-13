#include <iostream>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <variant>

#include "type_list.h"

namespace TMP {
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

		//get function pointer
		template <class Return_type, class... Args>
		auto get_function_pointer(Type_list<Args...>) -> Return_type (*)(Args...);
		template <class Return_type, class Class, class... Args>
		auto get_member_function_pointer(Type_list<Args...>) -> Return_type (Class::*)(Args...);
		template <class Return_type, class Class, class... Args>
		auto get_const_member_function_pointer(Type_list<Args...>) -> Return_type (Class::*)(Args...) const;
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
		using as_function_pointer = decltype(detail::get_member_function_pointer<Return_type, Class_type>(Args{}));
	};

	template <class T>
	struct Callable_info<T, false> {
		Callable_info(T) {}
		Callable_info() = default;
		using Return_type = detail::Return_type<T>;
		constexpr static bool has_class_type = false;
		using Args = detail::Args<T>;
		using as_function_pointer = decltype(detail::get_function_pointer<Return_type>(Args{}));
	};

	template <class T>
	Callable_info(T)->Callable_info<T, detail::has_class_type<T>>;

	//helper to make a function pointer
	template <class Return_type, class Typelist>
	using function_pointer = decltype(detail::get_function_pointer<Return_type>(Typelist{}));
	template <class Return_type, class Class, class Typelist>
	using member_function_pointer = decltype(detail::get_member_function_pointer<Return_type, Class>(Typelist{}));
	template <class Return_type, class Class, class Typelist>
	using const_member_function_pointer = decltype(detail::get_const_member_function_pointer<Return_type, Class>(Typelist{}));

	//make overloaded functions
	template <class... Callables>
	struct Overload : Callables... {
		Overload(Callables &&... callables)
			: Callables(std::forward<Callables>(callables))... {}
		using Callables::operator()...;
	};

	//like std::function, but not an owner
	template <class Signature>
	struct Function_ref {
		Function_ref(Signature function_pointer)
			: target{reinterpret_cast<void (*)()>(function_pointer)}
			, call{&fp_caller} {}
		template <class Callable>
		Function_ref(Callable &callable)
			: object{&callable}
			, call{&class_caller<typename Function_info::Return_type,
								 std::conditional_t<std::is_const_v<Callable>, std::add_const_t<typename decltype(TMP::Callable_info(callable))::Class_type>,
													typename decltype(TMP::Callable_info(callable))::Class_type>,
								 decltype(&Callable::operator())>} {
			target.mfp = reinterpret_cast<void (Function_ref::*)()>(&Callable::operator());
		}

		template <class... Args>
		auto operator()(Args &&... args) const {
			return call(target, object, {std::forward<Args>(args)...});
		}

		private:
		using Function_info = decltype(TMP::Callable_info(std::declval<Signature>()));
		union Target {
			void (*fp)();
			void (Function_ref::*mfp)();
		} target;
		void *object = nullptr;
		typename Function_info::Return_type (*call)(Target target, void *object, typename Function_info::Args::template instantiate<std::tuple> args);

		using FP = TMP::function_pointer<typename Function_info::Return_type, typename Function_info::Args::template prepend<void *>>;

		static auto fp_caller(Target target, void *, typename Function_info::Args::template instantiate<std::tuple> args) {
			return std::apply(reinterpret_cast<Signature>(target.fp), std::move(args));
		}
		template <class Return_type, class Class, class FP>
		static Return_type class_caller(Target target, void *obj, typename Function_info::Args::template instantiate<std::tuple> args) {
			return std::apply(
				[target, obj](auto &&... largs) {
					(reinterpret_cast<Class *>(obj)->*reinterpret_cast<FP>(target.mfp))(std::forward<decltype(largs)>(largs)...);
				},
				std::move(args));
		}
	};
	template <class F>
	Function_ref(F)->Function_ref<typename decltype(Callable_info{std::declval<F>()})::as_function_pointer>;
} // namespace TMP

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
