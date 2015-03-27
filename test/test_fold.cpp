#include <type_traits>
#include <algorithm>
#include <tuple>
#include "foldr.hpp"
#include "foldl.hpp"

template<typename left, typename right>
struct max;

template<typename int_type, std::size_t left, std::size_t right>
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

template<typename> struct probe;

int main()
{
	using list =
		std::tuple<
			std::integral_constant<unsigned, 1>,
			std::integral_constant<unsigned, 2>,
			std::integral_constant<unsigned, 3>,
			std::integral_constant<unsigned, 4>,
			std::integral_constant<unsigned, 5>,
			std::integral_constant<unsigned, 6>
		>;
	static_assert(foldr<max, std::integral_constant<unsigned,0>, list>::type::value == 6, "");
	static_assert(foldl<max, std::integral_constant<unsigned,0>, list>::type::value == 6, "");

	using list2 = std::tuple<int,float, double>;
	static_assert(
		std::is_same<
			typename foldl<append, std::tuple<>, list2>::type,
			std::tuple<int, float, double>
		>::value,
	"");

	return 0;
}
