#ifndef INCLGUARD_append_hpp
#define INCLGUARD_append_hpp

namespace meta
{
	/// append
	template<typename TypeList, typename Type> struct append;

	template<template<typename...> class List, typename... Types, typename Type>
	struct append<List<Types...>, Type>
	{
		using type = List<Types..., Type>;
	};

	template<template<typename...> class List, typename... Types, typename Type>
	using append_t = typename append<List<Types...>, Type>::type;
}

#endif
