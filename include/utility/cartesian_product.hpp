#ifndef INCLGUARD_cartesian_product_hpp
#define INCLGUARD_cartesian_product_hpp

#include "map.hpp"
#include "prepend.hpp"
#include "foldr.hpp"
#include "concat.hpp"

namespace meta
{

	/// cartesian_product
	template<typename... List> struct cartesian_product;

	template<template<typename...> class List>
	struct cartesian_product<List<>>
	{
		using type = List<>;
	};

	template<template<typename...> class List, typename... Items>
	struct cartesian_product<List<Items...>>
	{
		template<typename T>
		struct make_list
		{
			using type = List<T>;
		};

		using type = typename map<make_list, List<Items...> >::type;
	};

	template<template<typename...> class List, typename... Items, typename... OtherLists>
	struct cartesian_product<List<Items...>, OtherLists...>
	{
		/// prepends Item to each element of _List
		template<typename Item, typename _List1>
		struct prepend_to_all
		{
			template<typename _List2>
			struct prepend
			{
				using type = typename meta::prepend<Item, _List2>::type;
			};

			using type = typename meta::map<prepend, _List1>::type;
		};

		using sub_product = typename cartesian_product<OtherLists...>::type;

		template<typename Item>
		struct append_combinations
		{
			using type = typename prepend_to_all<Item, sub_product>::type;
		};

		using type = typename meta::foldr<meta::concat, List<>, typename map<append_combinations, List<Items...>>::type>::type;
	};

}

#endif
