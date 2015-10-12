#ifndef INCLGAURD_apply_list_hpp
#define INCLGAURD_apply_list_hpp

namespace meta
{
	/// applys a parameter pack stored in a type list to the given metatemplate function
	template<template<typename...> class TypeTemplate, typename List>
	struct apply_list;

	template<template<typename...> class TypeTemplate, template<typename...> class List, typename... Types>
	struct apply_list<TypeTemplate, List<Types...>>
	{
		using type = TypeTemplate<Types...>;
	};

	template<template<typename...> class TypeTemplate, typename List>
	using apply_list_t = typename apply_list<TypeTemplate, List>::type;
}

#endif

