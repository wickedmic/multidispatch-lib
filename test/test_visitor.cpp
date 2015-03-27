#include <iostream>
#include "visitor.hpp"

template<typename> struct probe;

class A {};
class B {};

using visitor_base_orig = utility::visitor_base<A, B>;
using visitor_base = utility::make_const_visitor_t<visitor_base_orig>;

struct visitor : public utility::visitor<::visitor, ::visitor_base>
{
	void visit(const A& a)
	{
		std::cout << "visit(A)" << std::endl;
	}

	void visit(const B& b)
	{
		std::cout << "visit(B)" << std::endl;
	}
};

template<typename T>
void test(T&& obj, visitor_base& visitor)
{
	visitor._visit(std::forward<T>(obj));
}

int main()
{
	visitor v;
	A const a{};
	B const b{};

	test(a, v);
	test(b, v);

	return 0;
}
