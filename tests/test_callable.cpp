#include "../callable.h"
#include "../external/catch.hpp"

#include <memory>
#include <mutex>
#include <string>

static char *foo(int, double) {
	return nullptr;
}

static bool same_type_string(std::string_view sv1, std::string_view sv2) {
	std::string s1{std::begin(sv1), std::end(sv1)};
	std::string s2{std::begin(sv2), std::end(sv2)};
	s1.erase(std::remove(std::begin(s1), std::end(s1), ' '), std::end(s1));
	s2.erase(std::remove(std::begin(s2), std::end(s2), ' '), std::end(s2));
	return s1 == s2;
}

SCENARIO("Getting callable info", "[callable]") {
	WHEN("Getting callable info for a function pointer") {
		const auto fpp = &foo;
		TMP::Callable_info fp = fpp;
		REQUIRE(same_type_string(type_name<decltype(fp)::Return_type>(), "char *"));
		REQUIRE(same_type_string(type_name<decltype(fp)::Args>(), "TMP::Type_list<int, double>"));
		REQUIRE(not decltype(fp)::has_class_type);
	}
	WHEN("Getting callable info for a lambda") {
		TMP::Callable_info lambda = [i = 42](int, void *) { return &i; };
		REQUIRE(same_type_string(type_name<decltype(lambda)::Return_type>(), "const int *"));
		REQUIRE(decltype(lambda)::has_class_type);
		REQUIRE(type_name<decltype(lambda)::Class_type>().size() > 0);
	}
	WHEN("Getting info from non-copyable") {
		{
			auto lambda = [up = std::make_unique<int>(42)] { return *up; };
			TMP::Callable_info{lambda};
		}
		{
			const auto lambda = [up = std::make_unique<int>(42)] { return *up; };
			TMP::Callable_info{lambda};
		}
		{
			TMP::Callable_info{[up = std::make_unique<int>(42)] { return *up; }};
		}
	}

	static_assert(std::is_same_v<TMP::Type_list<int, void *>::at<-1>, void *>);
	static_assert(std::is_same_v<TMP::Type_list<int, void *>::at<1>, void *>);
	static_assert(
		std::is_same_v<TMP::Type_list<int, double>::template concatenate<TMP::Type_list<int *, double *>>, TMP::Type_list<int, double, int *, double *>>);
	static_assert(std::is_same_v<typename TMP::Type_list<int, int *, double>::remove<int *>, TMP::Type_list<int, double>>);
}

struct Ref_counted {
	Ref_counted() {
		ref_count++;
	}
	Ref_counted(const Ref_counted &) {
		ref_count++;
	}
	Ref_counted(Ref_counted &&) {
		ref_count--;
	}
	Ref_counted &operator=(const Ref_counted &) = default;
	Ref_counted &operator=(Ref_counted &&) noexcept = default;
	~Ref_counted() {
		ref_count--;
	}
	static inline int ref_count = 0;
};

SCENARIO("Testing Function_ref", "[Function_ref]") {
	WHEN("Checking Overload") {
		{ TMP::Overload{}; }
		{
			TMP::Overload f{[](int) { return 1; }, [](double) { return 2; }};
			REQUIRE(f(42) == 1);
			REQUIRE(f(3.14) == 2);
		}
	}
	WHEN("Creating Function_ref from function pointer") {
		{
			TMP::Function_ref fr{+[](int i) { return i * i; }};
			REQUIRE(fr(3) == 9);
		}
		{
			TMP::Function_ref fr{+[] { return 42; }};
			REQUIRE(fr() == 42);
		}
	}
	WHEN("Creating Function_ref from lambda") {
		int i = 0;
		auto lambda = [&i] { i = 42; };
		TMP::Function_ref fr{lambda};
		REQUIRE(i == 0);
		fr();
		REQUIRE(i == 42);
	}
	WHEN("Passing move-only types") {
		TMP::Function_ref fr{+[](std::unique_ptr<int> p) { return p; }};
		auto up = fr(std::make_unique<int>(42));
		REQUIRE(*up == 42);
	}
	WHEN("Making sure we don't use up overly much space") {
		TMP::Function_ref f{foo};
		REQUIRE(sizeof(f) <= sizeof(void (*)()) * 2);
	}
	WHEN("Making using a temporary capturing lambda") {
		TMP::Function_ref f{[i = 42] { return i; }};
		REQUIRE(f() == 42);
	}
	WHEN("Default-constructing and assigning functions") {
		TMP::Function_ref<int()> f;
		f = [] { return 42; };
		REQUIRE(f() == 42);
		f = [] { return 0; };
		REQUIRE(f() == 0);
	}
	WHEN("Calling empty Function_ref") {
		TMP::Function_ref<int()> f;
		REQUIRE_THROWS_AS(f(), std::bad_function_call);
	}
	WHEN("Referencing an uncopyable callable") {
		{
			auto lambda = [up = std::make_unique<int>(42)] { return *up; };
			TMP::Function_ref f = lambda;
			REQUIRE(f() == 42);
		}
		{
			const auto lambda = [up = std::make_unique<int>(42)] { return *up; };
			TMP::Function_ref f = lambda;
			REQUIRE(f() == 42);
		}
	}
	WHEN("Testing lvalue arguments") {
		TMP::Function_ref<int(int)> fr = [](int i) { return i; };
		int i = 42;
		REQUIRE(fr(i) == i);
	}
	WHEN("Testing const propagation of callable") {
		struct C {
			bool *called_with_const;
			void operator()() {
				*called_with_const = false;
			}
			void operator()() const {
				*called_with_const = true;
			}
		};
		bool propagated_as_const{};
		C c{&propagated_as_const};
		TMP::Function_ref<void()> f{c};
		f();
		REQUIRE(not propagated_as_const);
		const auto &cc = c;
		f = cc;
		f();
		REQUIRE(propagated_as_const);
	}
	WHEN("Checking if we can drop output") {
		TMP::Function_ref<void()> f;
		Ref_counted::ref_count = 0;
		WHEN("Using a lambda") {
			f = [] { return std::make_unique<Ref_counted>(); };
			f();
			REQUIRE(Ref_counted::ref_count == 0);
		}
		WHEN("Using a function pointer") {
			f = +[] { return std::make_unique<Ref_counted>(); };
			f();
			REQUIRE(Ref_counted::ref_count == 0);
		}
	}
	WHEN("Using convertible types") {
		WHEN("Returning convertible type") {
			TMP::Function_ref<std::string()> f = [] { return "42"; };
			REQUIRE(f() == "42");
		}
		WHEN("Passing convertible type") {
			TMP::Function_ref<std::string(const char *)> f = [](std::string s) { return s; };
			REQUIRE(f("42") == "42");
		}
	}
	WHEN("Using pointer to member function") {
		struct S {
			int f(int i) {
				return i;
			}
		} s;
		TMP::Function_ref<int(S &, int)> f = &S::f;
		REQUIRE(f(s, 42) == 42);
	}
	WHEN("Testing RVO") {
		TMP::Function_ref f = [] { return std::mutex{}; };
		[[maybe_unused]] auto mutex = f();
	}
}
