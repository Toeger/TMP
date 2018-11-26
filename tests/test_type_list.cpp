#include "../external/catch.hpp"
#include "../type_list.h"

#include <tuple>
#include <type_traits>

SCENARIO("Testing Type_list", "[Type_list]") {
	WHEN("Testing append") {
		REQUIRE(std::is_same_v<TMP::Type_list<>::append<int>, TMP::Type_list<int>>);
		REQUIRE(std::is_same_v<TMP::Type_list<int>::append<double>, TMP::Type_list<int, double>>);
		REQUIRE(std::is_same_v<TMP::Type_list<int, double>::append<double>, TMP::Type_list<int, double, double>>);
	}
	WHEN("Testing prepend") {
		REQUIRE(std::is_same_v<TMP::Type_list<>::prepend<int>, TMP::Type_list<int>>);
		REQUIRE(std::is_same_v<TMP::Type_list<int>::prepend<double>, TMP::Type_list<double, int>>);
		REQUIRE(std::is_same_v<TMP::Type_list<double, int>::prepend<double>, TMP::Type_list<double, double, int>>);
	}
	WHEN("Testing apply") {
		REQUIRE(std::is_same_v<TMP::Type_list<int, void>::apply<std::add_pointer_t>, TMP::Type_list<int *, void *>>);
	}
	WHEN("Testing instantiate") {
		REQUIRE(std::is_same_v<TMP::Type_list<int, double>::instantiate<std::tuple>, std::tuple<int, double>>);
	}
	WHEN("Testing at") {
		REQUIRE(std::is_same_v<TMP::Type_list<int>::at<0>, int>);
		REQUIRE(std::is_same_v<TMP::Type_list<int, double, void *>::at<0>, int>);
		REQUIRE(std::is_same_v<TMP::Type_list<int, double, void *>::at<2>, void *>);
		REQUIRE(std::is_same_v<TMP::Type_list<int>::at<-1>, int>);
		REQUIRE(std::is_same_v<TMP::Type_list<int, double, void *>::at<-3>, int>);
		REQUIRE(std::is_same_v<TMP::Type_list<int, double, void *>::at<-1>, void *>);
	}
	WHEN("Testing concatenate") {
		REQUIRE(std::is_same_v<TMP::Type_list<>::concatenate<TMP::Type_list<>>, TMP::Type_list<>>);
		REQUIRE(std::is_same_v<TMP::Type_list<int, int *>::concatenate<TMP::Type_list<double, double *>>, TMP::Type_list<int, int *, double, double *>>);
	}
	WHEN("Testing remove") {
		REQUIRE(std::is_same_v<TMP::Type_list<int>::remove<int>, TMP::Type_list<>>);
		REQUIRE(std::is_same_v<TMP::Type_list<int, int, int>::remove<int>, TMP::Type_list<>>);
		REQUIRE(std::is_same_v<TMP::Type_list<int, double, int>::remove<int>, TMP::Type_list<double>>);
		REQUIRE(std::is_same_v<TMP::Type_list<int &, int *, const int>::remove<int>, TMP::Type_list<int &, int *, const int>>);
	}
	WHEN("Testing adopt_from") {
		REQUIRE(std::is_same_v<TMP::Type_list<int, double>, TMP::adopt_from<std::tuple<int, double>>>);
	}
}