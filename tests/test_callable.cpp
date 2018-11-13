#include "../callable.h"
#include "../external/catch.hpp"

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

	static_assert(std::is_same_v<TMP::Type_list<int, void *>::at<-1>, void *>);
	static_assert(std::is_same_v<TMP::Type_list<int, void *>::at<1>, void *>);
	static_assert(
		std::is_same_v<TMP::Type_list<int, double>::template concatenate<TMP::Type_list<int *, double *>>, TMP::Type_list<int, double, int *, double *>>);
	static_assert(std::is_same_v<typename TMP::Type_list<int, int *, double>::remove<int *>, TMP::Type_list<int, double>>);
}
