#include "multidispatch.hpp"
#include <typeinfo>
#include <tuple>
#include <iostream>

int main()
{
	using type_list = std::tuple<int,float>;

	md::handle<type_list> h1 = 1.3f;
	md::handle<type_list> h2 = 1;

	auto f = [](auto obj){
		std::cout << "object [" << typeid(obj).name() << "]: " << obj << std::endl;
	};

	md::dispatch(h1, f);
	md::dispatch(h2, f);

	return 0;
}
