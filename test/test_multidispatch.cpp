#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE multidispatch
#include <boost/test/unit_test.hpp>

#include <tuple>
#include "multidispatch.hpp"

template<typename> struct probe;

BOOST_AUTO_TEST_CASE(handle_empty_test)
{
	using namespace md;

	handle<int,float> h;
	BOOST_CHECK_EQUAL( static_cast<bool>(h), false );
	BOOST_CHECK( h.get() == nullptr );
	BOOST_CHECK_THROW( h.type(), empty_handle_exception );
}

BOOST_AUTO_TEST_CASE(handle_value_test)
{
	md::handle<int,double,bool> handle;
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
	md::handle<int, float, bool> handle;

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

	md::handle<int, float> const const_handle = 1234;
	*md::cast<int>(const_handle) = 5678;
	BOOST_CHECK_EQUAL( *md::cast<int>(const_handle), 5678 );
}

BOOST_AUTO_TEST_CASE(handle_get_set_reset_test)
{
	md::handle<int, float> h;
	BOOST_CHECK( h.get() == nullptr );

	h.set<int>(3);
	BOOST_CHECK( h.get() != nullptr );
	BOOST_CHECK_EQUAL( *md::cast<int>(h), 3 );

	h.set<float>(4);
	BOOST_CHECK( h.get() != nullptr );
	BOOST_CHECK_EQUAL( *md::cast<float>(h), 4.0f );

	h.reset();
	BOOST_CHECK( h.get() == nullptr );
}

// ---
BOOST_AUTO_TEST_CASE(type_index_test)
{
	using h1 = md::handle<int,float>;
	using h2 = md::handle<bool,double>;

	h1 a;
	h2 b;

	a = 1; b = true;
	BOOST_CHECK_EQUAL( (md::type_index<h1, h2>::index(a, b)), 0 );
	a = 1; b = 2.3;
	BOOST_CHECK_EQUAL( (md::type_index<h1, h2>::index(a, b)), 1 );
	a = 4.5f; b = true;
	BOOST_CHECK_EQUAL( (md::type_index<h1, h2>::index(a, b)), 2 );
	a = 4.5f; b = 2.3;
	BOOST_CHECK_EQUAL( (md::type_index<h1, h2>::index(a, b)), 3 );


	a = 1; b = true;
	BOOST_CHECK_EQUAL( (md::type_index<h1, int, h2>::index(a, 1, b)), 0 );
	a = 1; b = 2.3;
	BOOST_CHECK_EQUAL( (md::type_index<h1, int, h2>::index(a, 1, b)), 1 );
	a = 4.5f; b = true;
	BOOST_CHECK_EQUAL( (md::type_index<h1, int, h2>::index(a, 1, b)), 2 );
	a = 4.5f; b = 2.3;
	BOOST_CHECK_EQUAL( (md::type_index<h1, int, h2>::index(a, 1, b)), 3 );


	BOOST_CHECK_EQUAL( (md::type_index<short, int, float>::index(1, 1, 1)), 0 );
}

BOOST_AUTO_TEST_CASE(dispatch_function_test)
{
	using list = md::_md_detail::list<int>;
	struct functor
	{
		int operator()(int i) const { return i; }
	};

	int i = 123456789;
	BOOST_CHECK_EQUAL( (md::dispatch_function<functor,list>::function(functor{}, &i)), i );
}

BOOST_AUTO_TEST_CASE(dispatch_function_table)
{
	using md::_md_detail::list;
	using list1 = list<list<int>, list<float>>;

	struct functor
	{
		void operator()(int) const { }
		void operator()(float) const { }
	};

	BOOST_CHECK_EQUAL( (md::function_table<functor,list1>::get(0)), (md::dispatch_function<functor, list<int>>::function) );
	BOOST_CHECK_EQUAL( (md::function_table<functor,list1>::get(1)), (md::dispatch_function<functor, list<float>>::function) );
}


BOOST_AUTO_TEST_CASE(is_handle_test)
{
	BOOST_CHECK_EQUAL( (md::is_handle<int>::value), false );
	BOOST_CHECK_EQUAL( (md::is_handle<md::handle<int>>::value), true );
	BOOST_CHECK_EQUAL( (md::is_handle<md::handle<int> const>::value), true );
}

BOOST_AUTO_TEST_CASE(make_list_test)
{
	using md::_md_detail::list;

	BOOST_CHECK( (std::is_same<typename md::make_list<int>::type, list<int>>::value) );
	BOOST_CHECK( (std::is_same<typename md::make_list<md::handle<int>>::type, md::handle<int>>::value) );
}

BOOST_AUTO_TEST_CASE(get_object_test)
{
	md::handle<int> hi = 42;
	float f;

	BOOST_CHECK_EQUAL( md::get_object(hi), hi.get() );
	BOOST_CHECK_EQUAL( md::get_object(f),  &f );
}

BOOST_AUTO_TEST_CASE(dispatcher_no_handle_test)
{
	using md::_md_detail::list;

	struct functor
	{
		auto operator()(int i, float f, bool b) const { return std::make_tuple(i,f,b); }
	};

	BOOST_CHECK( (md::dispatcher<functor, int, float, bool>::dispatch(functor{}, 1, 2.3f, true) == std::tuple<int,float,bool>{1, 2.3f, true}) );
}

BOOST_AUTO_TEST_CASE(dispatcher_only_handle_test)
{
	using md::_md_detail::list;

	struct functor
	{
		auto operator()(int i, float f, bool b) const { return std::make_tuple(i,f,b); }
	};

	using h1 = md::handle<int>;
	using h2 = md::handle<float>;
	using h3 = md::handle<bool>;

	BOOST_CHECK( (md::dispatcher<functor,h1,h2,h3>::dispatch(functor{}, h1{1}, h2{2.3f}, h3{true}) == std::tuple<int,float,bool>{1, 2.3f, true}) );
}

struct dispatcher_multi_handle_test_functor
{
	template<typename T1, typename T2>
	auto operator()(T1 const& t1, T2 const& t2) const { return std::make_tuple(t1.m, t2.m); }
};

BOOST_AUTO_TEST_CASE(dispatcher_multi_handle_test)
{
	using md::_md_detail::list;
	using functor = dispatcher_multi_handle_test_functor;

	struct t1{ int m; };
	struct t2{ int _; int m; };
	struct t3{ int _; int __; int m; };
	struct t4{ int _; int __; int ___; int m; };

	using h1 = md::handle<t1,t2>;
	using h2 = md::handle<t3,t4>;

	t1 o1{}; o1.m = 1;
	t2 o2{}; o2.m = 2;
	t3 o3{}; o3.m = 3;
	t4 o4{}; o4.m = 4;

	BOOST_CHECK( (md::dispatcher<functor,h1,h2>::dispatch(functor{}, h1{ o1 }, h2{ o3 }) == std::tuple<int,int>{1, 3}) );
	BOOST_CHECK( (md::dispatcher<functor,h1,h2>::dispatch(functor{}, h1{ o1 }, h2{ o4 }) == std::tuple<int,int>{1, 4}) );
	BOOST_CHECK( (md::dispatcher<functor,h1,h2>::dispatch(functor{}, h1{ o2 }, h2{ o3 }) == std::tuple<int,int>{2, 3}) );
	BOOST_CHECK( (md::dispatcher<functor,h1,h2>::dispatch(functor{}, h1{ o2 }, h2{ o4 }) == std::tuple<int,int>{2, 4}) );
}

struct dispatcher_mixed_handle_test_functor
{
	template<typename T1, typename T2>
	auto operator()(T1 const& t1, float f, T2 const& t2) const { return std::make_tuple(t1.m, f, t2.m); }
};

BOOST_AUTO_TEST_CASE(dispatcher_mixed_handle_test)
{
	using md::_md_detail::list;
	using functor = dispatcher_mixed_handle_test_functor;

	struct t1{ int m; };
	struct t2{ int _; int m; };
	struct t3{ int _; int __; int m; };
	struct t4{ int _; int __; int ___; int m; };

	using h1 = md::handle<t1,t2>;
	using h2 = md::handle<t3,t4>;

	t1 o1{}; o1.m = 1;
	t2 o2{}; o2.m = 2;
	t3 o3{}; o3.m = 3;
	t4 o4{}; o4.m = 4;

	BOOST_CHECK( (md::dispatcher<functor,h1,float,h2>::dispatch(functor{}, h1{ o1 }, 4.2f, h2{ o3 }) == std::tuple<int,float,int>{1, 4.2f, 3}) );
	BOOST_CHECK( (md::dispatcher<functor,h1,float,h2>::dispatch(functor{}, h1{ o1 }, 4.2f, h2{ o4 }) == std::tuple<int,float,int>{1, 4.2f, 4}) );
	BOOST_CHECK( (md::dispatcher<functor,h1,float,h2>::dispatch(functor{}, h1{ o2 }, 4.2f, h2{ o3 }) == std::tuple<int,float,int>{2, 4.2f, 3}) );
	BOOST_CHECK( (md::dispatcher<functor,h1,float,h2>::dispatch(functor{}, h1{ o2 }, 4.2f, h2{ o4 }) == std::tuple<int,float,int>{2, 4.2f, 4}) );
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
	md::handle<int, float> h1_1 = 1;
	md::handle<int, float> h1_2 = 2.3f;
	md::handle<double, bool> h2_1 = 4.5;
	md::handle<double, bool> h2_2 = true;
	md::handle<void(*)(int,int), int*> h3_1 = static_cast<void(*)(int,int)>(nullptr);
	md::handle<void(*)(int,int), int*> h3_2 = static_cast<int*>(nullptr);

	BOOST_CHECK_EQUAL( (md::dispatch(functor{}, h1_1, h2_1, h3_1)), 0);
	BOOST_CHECK_EQUAL( (md::dispatch(functor{}, h1_1, h2_1, h3_2)), 1);
	BOOST_CHECK_EQUAL( (md::dispatch(functor{}, h1_1, h2_2, h3_1)), 2);
	BOOST_CHECK_EQUAL( (md::dispatch(functor{}, h1_1, h2_2, h3_2)), 3);
	BOOST_CHECK_EQUAL( (md::dispatch(functor{}, h1_2, h2_1, h3_1)), 4);
	BOOST_CHECK_EQUAL( (md::dispatch(functor{}, h1_2, h2_1, h3_2)), 5);
	BOOST_CHECK_EQUAL( (md::dispatch(functor{}, h1_2, h2_2, h3_1)), 6);
	BOOST_CHECK_EQUAL( (md::dispatch(functor{}, h1_2, h2_2, h3_2)), 7);
}

BOOST_AUTO_TEST_CASE(empty_function_table_test)
{
	BOOST_CHECK(md::dispatch([](){return true;}) == true);
}

BOOST_AUTO_TEST_CASE(const_handles)
{
	struct A { int a; };
	struct B { int dummy; int a; };

	A a{}; a.a = 1;
	B b{}; b.a = 2;

	md::handle<A, B> const h1 = a;
	md::handle<A, B> const h2 = b;

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
	md::handle<TestClass3 const, TestClass4>       h1; h1.set<TestClass3 const>(TestClass3{1});
	md::handle<TestClass3 const, TestClass4> const h2 = TestClass4{0, 2};

	BOOST_CHECK_EQUAL( md::dispatch(functor{}, h1), 1 );
	BOOST_CHECK_EQUAL( md::dispatch(functor{}, h2), 2);
	BOOST_CHECK_EQUAL( md::cast<TestClass4>(h2)->f, 1.234f );
}

