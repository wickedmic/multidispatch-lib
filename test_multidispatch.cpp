template<typename> struct probe;

#include "multidispatch.hpp"
#include <typeinfo>
#include <tuple>
#include <iostream>
#include <string>

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
//probe<typename md::foreach_prepend<int, List<>>::type> p13;
//probe<typename md::foreach_prepend<int, List<List<>>>::type> p10;
//probe<typename md::foreach_prepend<int, List<List<>,List<>>>::type> p11;
//probe<typename md::foreach_prepend<int, List<List<float>,List<short>>>::type> p12;

// cartesian_product
//probe<typename md::cartesian_product<List<>>::type> p14;
//probe<typename md::cartesian_product<List<char,int>, List<char,int>, List<char,int>>::type> p15;

// dispatch_function
//probe<decltype(md::dispatch_function<decltype(f), List<int,float>>::value)> p16;
auto f = [](auto p1, auto p2, auto p3)
{
	std::cout << "p1[" << typeid(p1).name() << "]: " << p1 << std::endl;
	std::cout << "p2[" << typeid(p2).name() << "]: " << p2 << std::endl;
	std::cout << "p3[" << typeid(p3).name() << "]: " << p3 << std::endl;
};

// size tests
static_assert(md::size<typename md::cartesian_product<List<int,char,short>, List<float,double,long double>>::type>::value == 9, "");

using list_t = List<List<char,int>,List<float,double>,List<std::string>>;
//probe<typename md::cartesian_product<List<char,int>,List<float,double>,List<int,int,int>>::type> p17;

int main()
{
//	int p1 = 1;
//	float p2 = 2.3f;
//	md::dispatch_function<decltype(f), List<int,float>>::value(f, (void*)&p1, (void*)&p2);


	std::cout << md::type_index<List<char,int>,List<float,double>,List<std::string>>::index(0,0,0) << std::endl;
	std::cout << md::type_index<List<char,int>,List<float,double>,List<std::string>>::index(0,1,0) << std::endl;
	std::cout << md::type_index<List<char,int>,List<float,double>,List<std::string>>::index(1,0,0) << std::endl;
	std::cout << md::type_index<List<char,int>,List<float,double>,List<std::string>>::index(1,1,0) << std::endl;

	md::handle<List<int, float>> h1_1 = 1;
	md::handle<List<int, float>> h1_2 = 2.3f;
	md::handle<List<double, bool>> h2_1 = 4.5;
	md::handle<List<double, bool>> h2_2 = true;
	md::handle<List<std::string, int*>> h3 = std::string("hello world");

	md::dispatch(f, h1_1, h2_1, h3);
	md::dispatch(f, h1_2, h2_1, h3);
	md::dispatch(f, h1_1, h2_2, h3);
	md::dispatch(f, h1_2, h2_2, h3);

	return 0;
}
