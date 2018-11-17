#include "../external/catch.hpp"
#include "../traits.h"

#include <list>
#include <map>
#include <string>
#include <vector>

SCENARIO("Testing traits", "[traits]") {
	WHEN("Testing specializations") {
		REQUIRE(TMP_IS_TYPE_SPECIALIZATION_V(std::vector<int>, std::vector));
		REQUIRE(not TMP_IS_TYPE_SPECIALIZATION_V(std::vector<int>, int));
		REQUIRE(not TMP_IS_TYPE_SPECIALIZATION_V(std::vector<int>, std::list));
		REQUIRE(TMP_IS_TYPE_SPECIALIZATION_V(std::map<int, std::string>, std::map));
		REQUIRE(decltype(TMP::is_type_specialization<std::map<int, std::string>, std::map>())::value);
		REQUIRE(not TMP_IS_TYPE_SPECIALIZATION_V(std::map<int, std::string>, std::vector));
		REQUIRE(not decltype(TMP::is_type_specialization<std::map<int, std::string>, std::vector>())::value);
	}
}