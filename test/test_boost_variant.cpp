#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE boost_variant
#include <boost/test/unit_test.hpp>

#include "register_boost_variant.hpp"

BOOST_AUTO_TEST_CASE(boost_variant_is_handle_test)
{
	using variant_t = boost::variant<int, float, bool>;
	using const_variant_t = boost::variant<int, float, bool> const;

	BOOST_CHECK_EQUAL( md::is_handle<variant_t>::value, true );
	BOOST_CHECK_EQUAL( md::is_handle<const_variant_t>::value, true );
}

struct TestStruct1{ int m; };
struct TestStruct2{ int _dummy1; int m; };
struct TestStruct3{ int _dummy1; int _dummy2; int m; };

BOOST_AUTO_TEST_CASE(boost_variant_get_address)
{
	{
		boost::variant<TestStruct1, TestStruct2, TestStruct3> variant = TestStruct1{ 42 };
		TestStruct1* ts = reinterpret_cast<TestStruct1*>(const_cast<void*>(get_object_address(variant)));
		BOOST_CHECK_EQUAL( ts->m, 42 );
	}

	{
		boost::variant<TestStruct1, TestStruct2, TestStruct3> variant = TestStruct2{ 0, 42 };
		TestStruct2* ts = reinterpret_cast<TestStruct2*>(const_cast<void*>(get_object_address(variant)));
		BOOST_CHECK_EQUAL( ts->m, 42 );
	}

	{
		boost::variant<TestStruct1, TestStruct2, TestStruct3> variant = TestStruct3{ 0, 0, 42 };
		TestStruct3* ts = reinterpret_cast<TestStruct3*>(const_cast<void*>(get_object_address(variant)));
		BOOST_CHECK_EQUAL( ts->m, 42 );
	}
}

