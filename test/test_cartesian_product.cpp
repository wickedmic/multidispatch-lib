#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE cartesian_product
#include <boost/test/unit_test.hpp>

#include <type_traits>
#include <cartesian_product.hpp>

template<typename...> struct List;

BOOST_AUTO_TEST_CASE(empty_list)
{
	BOOST_CHECK( (std::is_same<typename meta::cartesian_product<List<>>::type, List<>>::value == true) );
}

BOOST_AUTO_TEST_CASE(one_list)
{
	BOOST_CHECK( (
		std::is_same<
			typename meta::cartesian_product<
				List<int,float,bool>
			>::type,
			List<
				List<int>,
				List<float>,
				List<bool>
			>
		>::value == true) );
}

BOOST_AUTO_TEST_CASE(single_lists)
{
	BOOST_CHECK( (
		std::is_same<
			typename meta::cartesian_product<
				List<int>,
				List<float>,
				List<bool>
			>::type,
			List<
				List<int, float, bool>
			>
		>::value == true) );
}

template<typename> struct probe;

BOOST_AUTO_TEST_CASE(two_lists)
{
	struct type1 {};
	struct type2 {};
	struct type3 {};

	BOOST_CHECK( (
		std::is_same<
			typename meta::cartesian_product<
				List<type1, type2, type3>,
				List<type1, type2, type3>
			>::type,
			List<
				List<type1, type1>,
				List<type1, type2>,
				List<type1, type3>,
				List<type2, type1>,
				List<type2, type2>,
				List<type2, type3>,
				List<type3, type1>,
				List<type3, type2>,
				List<type3, type3>
			>
		>::value == true) );
}
