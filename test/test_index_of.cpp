#define BOOST_TEST_MODULE index_of
#include <boost/test/included/unit_test.hpp>

#include <index_of.hpp>

template<typename...> struct List;

BOOST_AUTO_TEST_CASE(IndexTest)
{
	using namespace meta;

	using map = List<int,double,float>;

	BOOST_CHECK( (index_of<map, int>::value    == 0) );
	BOOST_CHECK( (index_of<map, double>::value == 1) );
	BOOST_CHECK( (index_of<map, float>::value  == 2) );
}
