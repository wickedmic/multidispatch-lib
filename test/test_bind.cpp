#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE bind
#include <boost/test/unit_test.hpp>

#include "bind.hpp"
#include "map.hpp"
#include "at.hpp"
#include "concat.hpp"

template<typename... Types>
struct list {};


template<int N>
using int_ = std::integral_constant<int,N>;


template<typename P1, typename P2>
struct add;

template<int N1, int N2>
struct add<int_<N1>, int_<N2>>
{
	using type = int_<N1+N2>;
};


BOOST_AUTO_TEST_CASE(bind_test)
{
	using namespace meta;

	using result =
		typename map<
			bind<add, arg<0>, int_<1>>::apply,
			list<
				int_<1>,
				int_<2>,
				int_<3>
			>
		>::type;

	BOOST_CHECK_EQUAL( (at<result, 0>::type::value), 2 );
	BOOST_CHECK_EQUAL( (at<result, 1>::type::value), 3 );
	BOOST_CHECK_EQUAL( (at<result, 2>::type::value), 4 );
}


BOOST_AUTO_TEST_CASE(swap_test)
{
	using namespace meta;

	using result = bind<concat, arg<1>, arg<0>>::apply<list<int_<1>>, list<int_<2>>>::type;

	BOOST_CHECK_EQUAL( (at<result, 0>::type::value), 2);
	BOOST_CHECK_EQUAL( (at<result, 1>::type::value), 1);
}
