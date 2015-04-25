#ifndef INCLGAURD_apply_list_hpp
#define INCLGAURD_apply_list_hpp

namespace meta
{
	template<template<typename...> class TypeTemplate, typename List>
	struct apply_list;

	template<template<typename...> class TypeTemplate, template<typename...> class List, typename... Types>
	struct apply_list<TypeTemplate, List<Types...>>
	{
		using type = TypeTemplate<Types...>;
	};
}

#endif

