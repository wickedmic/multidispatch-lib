#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE multidispatch
#include <boost/test/unit_test.hpp>

#include "multidispatch.hpp"

template<typename...> struct List;

BOOST_AUTO_TEST_CASE(empty_handle)
{
	using namespace md;

	handle<List<int,float>> h;
	BOOST_CHECK_EQUAL( static_cast<bool>(h), false );
	BOOST_CHECK( h.get() == nullptr );
	BOOST_CHECK_THROW( h.type(), empty_handle_exception );
}

BOOST_AUTO_TEST_CASE(handle_value_test)
{
	md::handle<List<int,double,bool>> handle;
	BOOST_CHECK( (static_cast<bool>(handle) == false) );

	handle = static_cast<int>(1234);
	BOOST_CHECK( (handle.type() == 0) );
	BOOST_CHECK( (*static_cast<int const*>(handle.get()) == 1234) );

	handle = static_cast<double>(5.678);
	BOOST_CHECK( (handle.type() == 1) );
	BOOST_CHECK( (*static_cast<double const*>(handle.get()) == 5.678) );

	handle = static_cast<bool>(true);
	BOOST_CHECK( (handle.type() == 2) );
	BOOST_CHECK( (*static_cast<bool const*>(handle.get()) == true) );
}

BOOST_AUTO_TEST_CASE(handle_cast_test)
{
	md::handle<List<int, float, bool>> handle;

	handle = 1234;
	BOOST_REQUIRE( (md::cast<int>(handle) != nullptr) );
	BOOST_CHECK(  (*md::cast<int>(handle) == 1234) );

	handle = 5.678f;
	BOOST_REQUIRE( (md::cast<float>(handle) != nullptr) );
	BOOST_CHECK(  (*md::cast<float>(handle) == 5.678f) );

	handle = true;
	BOOST_REQUIRE( (md::cast<bool>(handle) != nullptr) );
	BOOST_CHECK(  (*md::cast<bool>(handle) == true) );

	BOOST_CHECK( (md::cast<int>(handle) == nullptr) );

	md::handle<List<int, float>> const const_handle = 1234;
	*md::cast<int>(const_handle) = 5678;
	BOOST_CHECK_EQUAL( *md::cast<int>(const_handle), 5678 );
}

BOOST_AUTO_TEST_CASE(handle_get_set_reset_test)
{
	md::handle<List<int, float>> h;
	BOOST_CHECK( h.get() == nullptr );

	h.set<int>(3);
	BOOST_CHECK( h.get() != nullptr );
	BOOST_CHECK_EQUAL( *md::cast<int>(h), 3 );

	h.reset();
	BOOST_CHECK( h.get() == nullptr );
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

BOOST_AUTO_TEST_CASE(const_handles)
{
	struct A { int a; };
	struct B { float dummy; int a; };
	md::handle<List<A, B>> const h1 = A{1};
	md::handle<List<A, B>> const h2 = B{0, 2};

	BOOST_CHECK_EQUAL( md::dispatch([](auto& p){ return p.a; }, h1), 1 );
	BOOST_CHECK_EQUAL( md::dispatch([](auto& p){ return p.a; }, h2), 2 );
}

BOOST_AUTO_TEST_CASE(const_types)
{
	struct TestClass3 { int i; };
	struct TestClass4 { float f; int i; };
	struct functor
	{
		int operator() (TestClass3 const& c) const
		{
			return c.i;
		}

		int operator() (TestClass4& d) const
		{
			d.f = 1.234f;
			return d.i;
		}
	};

	//md::handle<List<TestClass3 const, TestClass4>>       h1 = static_cast<TestClass3 const>(TestClass3{1});
	md::handle<List<TestClass3 const, TestClass4>>       h1; h1.set<TestClass3 const>(TestClass3{1});
	md::handle<List<TestClass3 const, TestClass4>> const h2 = TestClass4{0, 2};

	BOOST_CHECK_EQUAL( md::dispatch(functor{}, h1), 1 );
	BOOST_CHECK_EQUAL( md::dispatch(functor{}, h2), 2);
	BOOST_CHECK_EQUAL( md::cast<TestClass4>(h2)->f, 1.234f );
}
