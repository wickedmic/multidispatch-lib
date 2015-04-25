#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE multidispatch
#include <boost/test/unit_test.hpp>

#include <type_traits>
#include "apply_list.hpp"

template<typename...> struct list;

BOOST_AUTO_TEST_CASE(apply_list)
{
	BOOST_CHECK( (std::is_same<meta::apply_list<list, list<int,float>>::type, list<int,float>>::value) );
}
