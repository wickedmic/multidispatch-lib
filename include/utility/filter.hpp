#ifndef INCLGUARD_filter_hpp
#define INCLGUARD_filter_hpp

#include "prepend.hpp"
#include "foldr.hpp"
#include "if.hpp"

namespace meta
{
	template<template<typename...> class Predicate, typename List>
	struct filter;

	template<template<typename...> class Predicate, template<typename...> class List, typename... Types>
	struct filter<Predicate, List<Types...>>
	{
	private:
		template<typename Type, typename TypeList>
		struct _apply_if
		{
			using type =
				typename if_c<
					Predicate<Type>::value,
					typename prepend<Type, TypeList>::type,
					TypeList
				>::type;
		};

	public:
		using type = typename foldr<_apply_if, List<>, List<Types...>>::type;
	};
}

#endif

