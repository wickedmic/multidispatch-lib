#define BOOST_TEST_MODULE multidispatch
#include <boost/test/included/unit_test.hpp>

#include "multidispatch.hpp"

template<typename...> struct List;

BOOST_AUTO_TEST_CASE(HandleTest)
{
	md::handle<List<int,double,bool>> handle;
	BOOST_CHECK( (static_cast<bool>(handle) == false) );

	handle = static_cast<int>(1234);
	BOOST_CHECK( (handle.type() == 0) );
	BOOST_CHECK( (*static_cast<int*>(handle.get()) == 1234) );

	handle = static_cast<double>(5.678);
	BOOST_CHECK( (handle.type() == 1) );
	BOOST_CHECK( (*static_cast<double*>(handle.get()) == 5.678) );

	handle = static_cast<bool>(true);
	BOOST_CHECK( (handle.type() == 2) );
	BOOST_CHECK( (*static_cast<bool*>(handle.get()) == true) );
}

#define MAKE_FUNCTOR_OPERATOR(type1, type2, type3, return_value) unsigned operator()(type1,type2,type3) const { return return_value; }
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


BOOST_AUTO_TEST_CASE(function_call_test)
{
	md::handle<List<int, float>> h1_1 = 1;
	md::handle<List<int, float>> h1_2 = 2.3f;
	md::handle<List<double, bool>> h2_1 = 4.5;
	md::handle<List<double, bool>> h2_2 = true;
	md::handle<List<void(*)(int,int), int*>> h3_1 = static_cast<void(*)(int,int)>(nullptr);
	md::handle<List<void(*)(int,int), int*>> h3_2 = static_cast<int*>(nullptr);

	BOOST_CHECK(md::dispatch(functor{}, h1_1, h2_1, h3_1) == 0);
	BOOST_CHECK(md::dispatch(functor{}, h1_1, h2_1, h3_2) == 1);
	BOOST_CHECK(md::dispatch(functor{}, h1_1, h2_2, h3_1) == 2);
	BOOST_CHECK(md::dispatch(functor{}, h1_1, h2_2, h3_2) == 3);
	BOOST_CHECK(md::dispatch(functor{}, h1_2, h2_1, h3_1) == 4);
	BOOST_CHECK(md::dispatch(functor{}, h1_2, h2_1, h3_2) == 5);
	BOOST_CHECK(md::dispatch(functor{}, h1_2, h2_2, h3_1) == 6);
	BOOST_CHECK(md::dispatch(functor{}, h1_2, h2_2, h3_2) == 7);
}

BOOST_AUTO_TEST_CASE(empty_function_table_test)
{
	BOOST_CHECK(md::dispatch([](){return true;}) == true);
}
