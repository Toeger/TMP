#include "../callable.h"
#include "../external/catch.hpp"

#include <memory>

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
		REQUIRE_THROWS_AS(f(), TMP::Bad_function_call);
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
}
