#include "../external/catch.hpp"
#include "../traits.h"

#include <list>
#include <map>
#include <string>
#include <vector>

SCENARIO("Testing traits", "[traits]") {
	WHEN("Testing specializations") {
		REQUIRE(TMP::is_type_specialization_v<std::vector<int>, std::vector>);
		REQUIRE(not TMP::is_type_specialization_v<std::vector<int>, std::list>);
		REQUIRE(TMP::is_type_specialization_v<std::map<int, std::string>, std::map>);
		REQUIRE(not TMP::is_type_specialization_v<std::map<int, std::string>, std::vector>);
	}
	WHEN("Getting specializations") {
		REQUIRE(std::is_same_v<TMP::get_type_specialization_t<std::vector<int>, std::vector>::at<0>, int>);
		using map_types = TMP::get_type_specialization_t<std::map<int, std::string>, std::map>;
		REQUIRE(std::is_same_v<map_types::at<0>, int>);
		REQUIRE(std::is_same_v<map_types::at<1>, std::string>);
	}
}