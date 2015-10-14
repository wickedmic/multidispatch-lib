#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE multidispatch
#include <boost/test/unit_test.hpp>

#include <tuple>
#include "multidispatch.hpp"
#include "variant.hpp"
#include "register_variant.hpp"

// --- type_index tests ---

// check whether type_index returns the correct index for given type combination
BOOST_AUTO_TEST_CASE(type_index_test)
{
	using h1 = utility::variant<int,float>;
	using h2 = utility::variant<bool,double>;

	h1 a;
	h2 b;

	// variant only tests
	a = 1; b = true;
	BOOST_CHECK_EQUAL( (md::type_index<h1, h2>::index(a, b)), 0 );

	a = 1; b = 2.3;
	BOOST_CHECK_EQUAL( (md::type_index<h1, h2>::index(a, b)), 1 );

	a = 4.5f; b = true;
	BOOST_CHECK_EQUAL( (md::type_index<h1, h2>::index(a, b)), 2 );

	a = 4.5f; b = 2.3;
	BOOST_CHECK_EQUAL( (md::type_index<h1, h2>::index(a, b)), 3 );


	// tests with extra non-handle type
	a = 1; b = true;
	BOOST_CHECK_EQUAL( (md::type_index<h1, int, h2>::index(a, 1, b)), 0 );

	a = 1; b = 2.3;
	BOOST_CHECK_EQUAL( (md::type_index<h1, int, h2>::index(a, 1, b)), 1 );

	a = 4.5f; b = true;
	BOOST_CHECK_EQUAL( (md::type_index<h1, int, h2>::index(a, 1, b)), 2 );

	a = 4.5f; b = 2.3;
	BOOST_CHECK_EQUAL( (md::type_index<h1, int, h2>::index(a, 1, b)), 3 );


	// test with no variant types (i.e. there is nothing to form a combination -> return value defined as 0)
	BOOST_CHECK_EQUAL( (md::type_index<short, int, float>::index(1, 1, 1)), 0 );
}


// --- dispatch_function tests ---

// check whether the functor returns the given int
BOOST_AUTO_TEST_CASE(dispatch_function_test)
{
	using list = md::detail::list<int>;
	struct functor
	{
		int operator()(int i) const { return i; }
	};

	int i = 123456789;
	BOOST_CHECK_EQUAL( (md::dispatch_function<functor,list>::function(functor{}, &i)), i );
}

// check whether the correct function pointer is returned
BOOST_AUTO_TEST_CASE(dispatch_function_table_test)
{
	using md::detail::list;
	using list1 = list<list<int>, list<float>>;

	struct functor
	{
		void operator()(int) const { }
		void operator()(float) const { }
	};

	BOOST_CHECK_EQUAL( (md::function_table<functor,list1>::get(0)), (md::dispatch_function<functor, list<int>>::function) );
	BOOST_CHECK_EQUAL( (md::function_table<functor,list1>::get(1)), (md::dispatch_function<functor, list<float>>::function) );
}


// is_handle tests
BOOST_AUTO_TEST_CASE(is_handle_test)
{
	BOOST_CHECK_EQUAL( (md::is_handle<int>::value), false );
	BOOST_CHECK_EQUAL( (md::is_handle<utility::variant<int>>::value), true );
	BOOST_CHECK_EQUAL( (md::is_handle<utility::variant<int> const>::value), true );
}

// make_list tests
BOOST_AUTO_TEST_CASE(make_list_test)
{
	using md::detail::list;

	BOOST_CHECK( (std::is_same<typename md::detail::make_list<int>::type, list<int>>::value) );
	BOOST_CHECK( (std::is_same<typename md::detail::make_list<utility::variant<int>>::type, utility::variant<int>>::value) );
}

// get_object tests
BOOST_AUTO_TEST_CASE(get_object_test)
{
	utility::variant<int> hi = 42;
	float f;

	BOOST_CHECK_EQUAL( md::get_object(hi), hi.get() );
	BOOST_CHECK_EQUAL( md::get_object(f),  &f );
}

// dispatcher tests
BOOST_AUTO_TEST_CASE(dispatcher_no_handle_test)
{
	using md::detail::list;

	struct functor
	{
		auto operator()(int i, float f, bool b) const { return std::make_tuple(i,f,b); }
	};

	BOOST_CHECK( (md::dispatcher<functor, int, float, bool>::dispatch(functor{}, 1, 2.3f, true) == std::tuple<int,float,bool>{1, 2.3f, true}) );
}

BOOST_AUTO_TEST_CASE(dispatcher_only_handle_test)
{
	using md::detail::list;

	struct functor
	{
		auto operator()(int i, float f, bool b) const { return std::make_tuple(i,f,b); }
	};

	using h1 = utility::variant<int>;
	using h2 = utility::variant<float>;
	using h3 = utility::variant<bool>;

	BOOST_CHECK( (md::dispatcher<functor,h1,h2,h3>::dispatch(functor{}, h1{1}, h2{2.3f}, h3{true}) == std::tuple<int,float,bool>{1, 2.3f, true}) );
}


struct dispatcher_multi_handle_test_functor
{
	template<typename T1, typename T2>
	auto operator()(T1 const& t1, T2 const& t2) const
	{
		return std::make_tuple(t1.m, t2.m);
	}
};

BOOST_AUTO_TEST_CASE(dispatcher_multi_handle_test)
{
	using md::detail::list;
	using functor = dispatcher_multi_handle_test_functor;

	struct t1{ int m; };
	struct t2{ int _dummy1; int m; };
	struct t3{ int _dummy1; int _dummy2; int m; };
	struct t4{ int _dummy1; int _dummy2; int _dummy3; int m; };

	using h1 = utility::variant<t1,t2>;
	using h2 = utility::variant<t3,t4>;

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
	template<typename T1, typename T2, typename T3>
	auto operator()(T1 const& t1, T2 const& t2, T3 const& t3) const { return std::make_tuple(t1.m, t2.m, t3.m); }
};

BOOST_AUTO_TEST_CASE(dispatcher_mixed_handle_test)
{
	using md::detail::list;
	using functor = dispatcher_mixed_handle_test_functor;

	struct t1{ int m; };
	struct t2{ int _; int m; };
	struct t3{ int _; int __; int m; };
	struct t4{ int _; int __; int ___; int m; };

	using h1 = utility::variant<t1,t2>;
	using h2 = utility::variant<t3,t4>;

	t1 o1{}; o1.m = 1;
	t2 o2{}; o2.m = 2;
	t3 o3{}; o3.m = 3;
	t4 o4{}; o4.m = 4;

	BOOST_CHECK( (md::dispatcher<functor,h1,t1,h2>::dispatch(functor{}, h1{ o1 }, o1, h2{ o3 }) == std::tuple<int,int,int>{1, 1, 3}) );
	BOOST_CHECK( (md::dispatcher<functor,h1,t1,h2>::dispatch(functor{}, h1{ o1 }, o1, h2{ o4 }) == std::tuple<int,int,int>{1, 1, 4}) );
	BOOST_CHECK( (md::dispatcher<functor,h1,t1,h2>::dispatch(functor{}, h1{ o2 }, o1, h2{ o3 }) == std::tuple<int,int,int>{2, 1, 3}) );
	BOOST_CHECK( (md::dispatcher<functor,h1,t1,h2>::dispatch(functor{}, h1{ o2 }, o1, h2{ o4 }) == std::tuple<int,int,int>{2, 1, 4}) );

	// tests where the fixed type comes last
	BOOST_CHECK( (md::dispatcher<functor,h1,h2,t2>::dispatch(functor{}, h1{ o1 }, h2{ o3 }, o2) == std::tuple<int,int,float>{1, 3, 2}) );
	BOOST_CHECK( (md::dispatcher<functor,h1,h2,t2>::dispatch(functor{}, h1{ o1 }, h2{ o4 }, o2) == std::tuple<int,int,float>{1, 4, 2}) );
	BOOST_CHECK( (md::dispatcher<functor,h1,h2,t2>::dispatch(functor{}, h1{ o2 }, h2{ o3 }, o2) == std::tuple<int,int,float>{2, 3, 2}) );
	BOOST_CHECK( (md::dispatcher<functor,h1,h2,t2>::dispatch(functor{}, h1{ o2 }, h2{ o4 }, o2) == std::tuple<int,int,float>{2, 4, 2}) );
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


BOOST_AUTO_TEST_CASE(function_call_test)
{
	utility::variant<int, float> h1_1 = 1;
	utility::variant<int, float> h1_2 = 2.3f;
	utility::variant<double, bool> h2_1 = 4.5;
	utility::variant<double, bool> h2_2 = true;
	utility::variant<void(*)(int,int), int*> h3_1 = static_cast<void(*)(int,int)>(nullptr);
	utility::variant<void(*)(int,int), int*> h3_2 = static_cast<int*>(nullptr);

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

	utility::variant<A, B> const h1 = a;
	utility::variant<A, B> const h2 = b;

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

	//utility::variant<List<TestClass3 const, TestClass4>>       h1 = static_cast<TestClass3 const>(TestClass3{1});
	utility::variant<TestClass3 const, TestClass4>       h1; h1.set<TestClass3 const>(TestClass3{1});
	utility::variant<TestClass3 const, TestClass4> const h2 = TestClass4{0, 2};

	BOOST_CHECK_EQUAL( md::dispatch(functor{}, h1), 1 );
	BOOST_CHECK_EQUAL( md::dispatch(functor{}, h2), 2);
	BOOST_CHECK_EQUAL( utility::cast_variant<TestClass4>(h2)->f, 1.234f );
}

