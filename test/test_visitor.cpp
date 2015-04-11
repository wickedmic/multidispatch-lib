#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE visitor
#include <boost/test/unit_test.hpp>

#include "visitor.hpp"

template<typename> struct probe;

struct A
{
	A() : visited{false} {}
	bool visited;
};

struct B
{
	B() : visited{false} {}
	bool visited;
};

using base = utility::visitor_base<A,B>;

struct visitor : public utility::visitor<visitor, base>
{
	void visit(A& a) const
	{
		a.visited = true;
	}

	void visit(B& b) const
	{
		b.visited = true;
	}
};

template<typename T>
void visit(base& visitor, T&& obj)
{
	visitor._visit(std::forward<T>(obj));
}

BOOST_AUTO_TEST_CASE(visitor_test)
{
	visitor v;
	A a{};
	B b{};

	visit(v, a);
	BOOST_CHECK(a.visited == true);

	visit(v, b);
	BOOST_CHECK(b.visited == true);
}


