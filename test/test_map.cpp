#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE multidispatch
#include <boost/test/unit_test.hpp>

#include <type_traits>
#include "map.hpp"

template<typename T> struct add_one;

template<int N>
struct add_one<std::integral_constant<int, N>>
{
	using type = std::integral_constant<int, N+1>;
};

template<typename...> struct list;

BOOST_AUTO_TEST_CASE(map_test)
{
	using in_list =
		list<
			std::integral_constant<int, 1>,
			std::integral_constant<int, 2>,
			std::integral_constant<int, 3>,
			std::integral_constant<int, 4>,
			std::integral_constant<int, 5>
		>;

	using out_list =
		list<
			std::integral_constant<int, 2>,
			std::integral_constant<int, 3>,
			std::integral_constant<int, 4>,
			std::integral_constant<int, 5>,
			std::integral_constant<int, 6>
		>;

	BOOST_CHECK( (std::is_same<meta::map<add_one, in_list>::type, out_list>::value) );	
}
