#include "../callable.h"
#include "../external/catch.hpp"

static char *foo(int, double) {
	return nullptr;
}

SCENARIO("Getting callable info", "[callable]") {
	WHEN("Getting callable info for a function pointer") {
		const auto fpp = &foo;
		Callable_info fp = fpp;
		REQUIRE(type_name<decltype(fp)::Return_type>() == "char *");
		REQUIRE(type_name<decltype(fp)::Args>() == "Type_list<int, double>");
		REQUIRE(not decltype(fp)::has_class_type);
	}
	WHEN("Getting callable info for a lambda") {
		Callable_info lambda = [i = 42](int, void *) { return &i; };
		REQUIRE(type_name<decltype(lambda)::Return_type>() == "const int *");
		REQUIRE(decltype(lambda)::has_class_type);
		REQUIRE(type_name<decltype(lambda)::Class_type>().size() > 0);
	}

	static_assert(std::is_same_v<Type_list<int, void *>::at<-1>, void *>);
	static_assert(std::is_same_v<Type_list<int, void *>::at<1>, void *>);
	static_assert(std::is_same_v<Type_list<int, double>::template concatenate<Type_list<int *, double *>>, Type_list<int, double, int *, double *>>);
	static_assert(std::is_same_v<typename Type_list<int, int *, double>::remove<int *>, Type_list<int, double>>);
}
