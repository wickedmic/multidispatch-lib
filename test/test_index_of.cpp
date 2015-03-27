#include <iostream>
#include <tuple>
#include "type_map.hpp"

int main()
{
	using namespace utility;

	using map = std::tuple<int,double,float>;

	std::cout << "int: " << index_of<map, int>::value << "\ndouble: " << index_of<map,double>::value << "\nfloat: " << index_of<map,float>::value << std::endl;

	return 0;
}
