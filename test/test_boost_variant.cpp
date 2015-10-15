#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE boost_variant
#include <boost/test/unit_test.hpp>

#include <type_traits>
#include "multidispatch.hpp"
#include "register_boost_variant.hpp"

BOOST_AUTO_TEST_CASE(boost_variant_is_handle_test)
{
	using variant_t = boost::variant<int, float, bool>;
	using const_variant_t = boost::variant<int, float, bool> const;

	BOOST_CHECK_EQUAL( md::is_handle<variant_t>::value, true );
	BOOST_CHECK_EQUAL( md::is_handle<const_variant_t>::value, true );
}

BOOST_AUTO_TEST_CASE(boost_variant_type_list)
{
	BOOST_CHECK( (std::is_same<md::type_list<boost::variant<int,float,bool>>::type, md::list<int,float,bool>>::value) );
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

BOOST_AUTO_TEST_CASE(boost_variant_type_id)
{
	using var_t = boost::variant<int,float,bool>;

	var_t v1 = 1;
	var_t v2 = 2.3f;
	var_t v3 = true;

	BOOST_CHECK_EQUAL( type_id(v1), 0);
	BOOST_CHECK_EQUAL( type_id(v2), 1);
	BOOST_CHECK_EQUAL( type_id(v3), 2);
}

#define MAKE_FUNCTOR_OPERATOR(type1, type2, type3, return_value) \
	unsigned operator()(type1,type2,type3) const { return return_value; }
struct functor
{
	MAKE_FUNCTOR_OPERATOR(int,   double, void(*)(int,int), 0)
	MAKE_FUNCTOR_OPERATOR(int,   double, int*,             1)
	MAKE_FUNCTOR_OPERATOR(int,   bool,   void(*)(int,int), 2)
	MAKE_FUNCTOR_OPERATOR(int,   bool,   int*,             3)
	MAKE_FUNCTOR_OPERATOR(float, double, void(*)(int,int), 4)
	MAKE_FUNCTOR_OPERATOR(float, double, int*,             5)
	MAKE_FUNCTOR_OPERATOR(float, bool,   void(*)(int,int), 6)
	MAKE_FUNCTOR_OPERATOR(float, bool,   int*,             7)
};

BOOST_AUTO_TEST_CASE(boost_variant)
{
	boost::variant<int, float> h1_1 = 1;
	boost::variant<int, float> h1_2 = 2.3f;
	boost::variant<double, bool> h2_1 = 4.5;
	boost::variant<double, bool> h2_2 = true;
	boost::variant<void(*)(int,int), int*> h3_1 = static_cast<void(*)(int,int)>(nullptr);
	boost::variant<void(*)(int,int), int*> h3_2 = static_cast<int*>(nullptr);

	BOOST_CHECK_EQUAL( (md::dispatch(functor{}, h1_1, h2_1, h3_1)), 0);
	BOOST_CHECK_EQUAL( (md::dispatch(functor{}, h1_1, h2_1, h3_2)), 1);
	BOOST_CHECK_EQUAL( (md::dispatch(functor{}, h1_1, h2_2, h3_1)), 2);
	BOOST_CHECK_EQUAL( (md::dispatch(functor{}, h1_1, h2_2, h3_2)), 3);
	BOOST_CHECK_EQUAL( (md::dispatch(functor{}, h1_2, h2_1, h3_1)), 4);
	BOOST_CHECK_EQUAL( (md::dispatch(functor{}, h1_2, h2_1, h3_2)), 5);
	BOOST_CHECK_EQUAL( (md::dispatch(functor{}, h1_2, h2_2, h3_1)), 6);
	BOOST_CHECK_EQUAL( (md::dispatch(functor{}, h1_2, h2_2, h3_2)), 7);
}
