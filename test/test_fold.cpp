#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE fold
#include <boost/test/unit_test.hpp>

#include <type_traits>
#include "foldr.hpp"
#include "foldl.hpp"

template<typename left, typename right>
struct max;

template<typename int_type, int_type left, int_type right>
struct max<std::integral_constant<int_type, left>, std::integral_constant<int_type, right>>
{
	using type = std::integral_constant<int_type, left < right ? right : left>;
};

template<typename List, typename Type> struct append;

template<template<typename...> class List, typename... Types, typename Type>
struct append<List<Types...>, Type>
{
	using type = List<Types..., Type>;
};

template<typename...> struct list_t;

BOOST_AUTO_TEST_CASE(fold_max_test)
{
	using list1 =
		std::tuple<
			std::integral_constant<unsigned, 1>,
			std::integral_constant<unsigned, 2>,
			std::integral_constant<unsigned, 3>,
			std::integral_constant<unsigned, 4>,
			std::integral_constant<unsigned, 5>,
			std::integral_constant<unsigned, 6>
		>;

	BOOST_CHECK( (meta::foldr<max, std::integral_constant<unsigned,0>, list1>::type::value == 6) );
	BOOST_CHECK( (meta::foldl<max, std::integral_constant<unsigned,0>, list1>::type::value == 6) );
}

BOOST_AUTO_TEST_CASE(fold_append_test)
{
	using list2 = std::tuple<int,float, double>;
	BOOST_CHECK( (
		std::is_same<
			typename meta::foldl<append, std::tuple<>, list2>::type,
			std::tuple<int, float, double>
		>::value == true
	) );
}
