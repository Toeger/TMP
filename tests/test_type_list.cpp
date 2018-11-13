#include "../external/catch.hpp"
#include "../type_list.h"

#include <tuple>
#include <type_traits>

SCENARIO("Testing Type_list", "[Type_list]") {
	WHEN("Testing append") {
		static_assert(std::is_same_v<TMP::Type_list<>::append<int>, TMP::Type_list<int>>);
		static_assert(std::is_same_v<TMP::Type_list<int>::append<double>, TMP::Type_list<int, double>>);
		static_assert(std::is_same_v<TMP::Type_list<int, double>::append<double>, TMP::Type_list<int, double, double>>);
	}
	WHEN("Testing prepend") {
		static_assert(std::is_same_v<TMP::Type_list<>::prepend<int>, TMP::Type_list<int>>);
		static_assert(std::is_same_v<TMP::Type_list<int>::prepend<double>, TMP::Type_list<double, int>>);
		static_assert(std::is_same_v<TMP::Type_list<double, int>::prepend<double>, TMP::Type_list<double, double, int>>);
	}
	WHEN("Testing apply") {
		static_assert(std::is_same_v<TMP::Type_list<int, void>::apply<std::add_pointer_t>, TMP::Type_list<int *, void *>>);
	}
	WHEN("Testing instantiate") {
		static_assert(std::is_same_v<TMP::Type_list<int, double>::instantiate<std::tuple>, std::tuple<int, double>>);
	}
	WHEN("Testing at") {
		static_assert(std::is_same_v<TMP::Type_list<int>::at<0>, int>);
		static_assert(std::is_same_v<TMP::Type_list<int, double, void *>::at<0>, int>);
		static_assert(std::is_same_v<TMP::Type_list<int, double, void *>::at<2>, void *>);
		static_assert(std::is_same_v<TMP::Type_list<int>::at<-1>, int>);
		static_assert(std::is_same_v<TMP::Type_list<int, double, void *>::at<-3>, int>);
		static_assert(std::is_same_v<TMP::Type_list<int, double, void *>::at<-1>, void *>);
	}
	WHEN("Testing concatenate") {
		static_assert(std::is_same_v<TMP::Type_list<>::concatenate<TMP::Type_list<>>, TMP::Type_list<>>);
		static_assert(std::is_same_v<TMP::Type_list<int, int *>::concatenate<TMP::Type_list<double, double *>>, TMP::Type_list<int, int *, double, double *>>);
	}
	WHEN("Testing remove") {
		static_assert(std::is_same_v<TMP::Type_list<int>::remove<int>, TMP::Type_list<>>);
		static_assert(std::is_same_v<TMP::Type_list<int, int, int>::remove<int>, TMP::Type_list<>>);
		static_assert(std::is_same_v<TMP::Type_list<int, double, int>::remove<int>, TMP::Type_list<double>>);
		static_assert(std::is_same_v<TMP::Type_list<int &, int *, const int>::remove<int>, TMP::Type_list<int &, int *, const int>>);
	}
}