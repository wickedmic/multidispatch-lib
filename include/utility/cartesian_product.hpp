#ifndef INCLGUARD_cartesian_product_hpp
#define INCLGUARD_cartesian_product_hpp

namespace meta
{
	/// prepends Type to each list in List
	template<typename Item, typename List>
	struct foreach_prepend
	{
		template<typename _List>
		struct prepend
		{
			using type = meta::prepend_t<Item, _List>;
		};

		using type = meta::map_t<prepend, List>;
	};


	/// if_
	template<bool, typename TrueType, typename FalseType>
	struct if_
	{
		using type = FalseType;
	};


	template<typename TrueType, typename FalseType>
	struct if_<true, TrueType, FalseType>
	{
		using type = TrueType;
	};


	/// cartesian_product
	template<typename... List> struct cartesian_product;

	template<template<typename...> class List, typename Type, typename... Types, typename... OtherLists>
	struct cartesian_product<List<Type, Types...>, OtherLists...>
	{
		using type =
			typename meta::concat<
				typename foreach_prepend<
					Type,
					typename if_<
						sizeof...(OtherLists) != 0,
						typename cartesian_product<OtherLists...>::type,
						List<List<>>
					>::type
				>::type,
				typename cartesian_product<
					List<Types...>,
					OtherLists...
				>::type
			>::type;
	};

	template<template<typename...> class List, typename... OtherLists>
	struct cartesian_product<List<>, OtherLists...>
	{
		//using type = typename cartesian_product<OtherLists...>::type;
		using type = List<>;
	};

	template<template<typename...> class List>
	struct cartesian_product<List<>>
	{
		using type = List<>;
	};

	template<>
	struct cartesian_product<>
	{
		// sadly we don't know the list type here, so we return void
		// and compensate this 'wrong' type by using if_ (see above)
		using type = void;
	};

}

#endif
