#include "../external/catch.hpp"
#include "../type_list.h"

#include <tuple>
#include <type_traits>

SCENARIO("Testing Type_list", "[type_list]") {
	WHEN("Testing append") {
		static_assert(std::is_same_v<Type_list<>::append<int>, Type_list<int>>);
		static_assert(std::is_same_v<Type_list<int>::append<double>, Type_list<int, double>>);
		static_assert(std::is_same_v<Type_list<int, double>::append<double>, Type_list<int, double, double>>);
	}
	WHEN("Testing prepend") {
		static_assert(std::is_same_v<Type_list<>::prepend<int>, Type_list<int>>);
		static_assert(std::is_same_v<Type_list<int>::prepend<double>, Type_list<double, int>>);
		static_assert(std::is_same_v<Type_list<double, int>::prepend<double>, Type_list<double, double, int>>);
	}
	WHEN("Testing apply") {
		static_assert(std::is_same_v<Type_list<int, void>::apply<std::add_pointer_t>, Type_list<int *, void *>>);
	}
	WHEN("Testing instantiate") {
		static_assert(std::is_same_v<Type_list<int, double>::instantiate<std::tuple>, std::tuple<int, double>>);
	}
	WHEN("Testing at") {
		static_assert(std::is_same_v<Type_list<int>::at<0>, int>);
		static_assert(std::is_same_v<Type_list<int, double, void *>::at<0>, int>);
		static_assert(std::is_same_v<Type_list<int, double, void *>::at<2>, void *>);
		static_assert(std::is_same_v<Type_list<int>::at<-1>, int>);
		static_assert(std::is_same_v<Type_list<int, double, void *>::at<-3>, int>);
		static_assert(std::is_same_v<Type_list<int, double, void *>::at<-1>, void *>);
	}
	WHEN("Testing concatenate") {
		static_assert(std::is_same_v<Type_list<>::concatenate<Type_list<>>, Type_list<>>);
		static_assert(std::is_same_v<Type_list<int, int *>::concatenate<Type_list<double, double *>>, Type_list<int, int *, double, double *>>);
	}
	WHEN("Testing remove") {
		static_assert(std::is_same_v<Type_list<int>::remove<int>, Type_list<>>);
		static_assert(std::is_same_v<Type_list<int, int, int>::remove<int>, Type_list<>>);
		static_assert(std::is_same_v<Type_list<int, double, int>::remove<int>, Type_list<double>>);
		static_assert(std::is_same_v<Type_list<int &, int *, const int>::remove<int>, Type_list<int &, int *, const int>>);
	}
}