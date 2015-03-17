#include "multidispatch.hpp"
#include <typeinfo>
#include <tuple>
#include <iostream>

template<typename> struct probe;

template<typename...> struct List{};


// concat tests
//probe<typename md::concat<List<>, List<>>::type> p1;
//probe<typename md::concat<List<int>, List<>>::type> p2;
//probe<typename md::concat<List<>, List<int>>::type> p3;
//probe<typename md::concat<List<int>, List<int>>::type> p4;
//probe<typename md::concat<List<int,int>, List<>>::type> p5;
//probe<typename md::concat<List<>, List<int,int>>::type> p6;
//probe<typename md::concat<List<int,int>, List<int,int>>::type> p7;
//probe<typename md::concat<List<int>, List<int,int>>::type> p8;
//probe<typename md::concat<List<int,int>, List<int>>::type> p9;

// foreach_prepend tests
probe<typename md::foreach_prepend<int, List<>>::type> p13;
// probe<typename md::foreach_prepend<int, List<List<>>>::type> p10;
// probe<typename md::foreach_prepend<int, List<List<>,List<>>>::type> p11;
// probe<typename md::foreach_prepend<int, List<List<float>,List<short>>>::type> p12;

probe<typename md::build_tuple<List<>>::type> p14;
probe<typename md::build_tuple<List<char,int>, List<char,int>, List<char,int>>::type> p15;

int main()
{
//	using type_list = std::tuple<int,float>;
//
//	md::handle<type_list> h1 = 1.3f;
//	md::handle<type_list> h2 = 1;
//
//	auto f = [](auto obj){
//		std::cout << "object [" << typeid(obj).name() << "]: " << obj << std::endl;
//	};
//
//	md::dispatch(h1, f);
//	md::dispatch(h2, f);
//
	return 0;
}
