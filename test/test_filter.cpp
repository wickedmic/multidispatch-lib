#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE fold
#include <boost/test/unit_test.hpp>

#include <type_traits>
#include "filter.hpp"

template<typename...> struct List{};


template<typename T> struct is_odd;

template<typename T, T N>
struct is_odd<std::integral_constant<T, N>>
	: public std::integral_constant<bool, N % 2 == 1>
{ };


BOOST_AUTO_TEST_CASE(fold_max_test)
{
	using list1 =
		List<
			std::integral_constant<unsigned, 1>,
			std::integral_constant<unsigned, 2>,
			std::integral_constant<unsigned, 3>,
			std::integral_constant<unsigned, 4>,
			std::integral_constant<unsigned, 5>,
			std::integral_constant<unsigned, 6>
		>;

	using list2 =
		List<
			std::integral_constant<unsigned, 1>,
			std::integral_constant<unsigned, 3>,
			std::integral_constant<unsigned, 5>
		>;

	BOOST_CHECK_EQUAL( (is_odd<std::integral_constant<int, 1>>::value), true );

	BOOST_CHECK_EQUAL( (std::is_same<typename meta::filter<is_odd, list1>::type, list2>::value), true );
}
