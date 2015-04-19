#ifndef INCLGUARD_cartesian_product_hpp
#define INCLGUARD_cartesian_product_hpp

#include "map.hpp"
#include "prepend.hpp"
#include "foldr.hpp"
#include "concat.hpp"
#include "bind.hpp"

namespace meta
{

	/// prepends Item to each List in ListOfLists
	template<typename Item, typename ListOfLists>
	struct prepend_item_to_each_list
	{
		//template<typename PrependItem_List>
		//struct prepend_item
		//{
		//	using type = typename meta::prepend<Item, PrependItem_List>::type;
		//};

		//using type = typename meta::map<prepend_item, ListOfLists>::type;
		using type =
			typename meta::map<
				meta::bind<
					meta::prepend,
					Item,
					meta::arg<0>
				>::template apply,
				ListOfLists
			>::type;
	};

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
		using sub_product = typename cartesian_product<OtherLists...>::type;

		template<typename Item>
		struct append_combinations
		{
			using type = typename prepend_item_to_each_list<Item, sub_product>::type;
		};

		using type = typename meta::foldr<meta::concat, List<>, typename map<append_combinations, List<Items...>>::type>::type;
	};

}

#endif
