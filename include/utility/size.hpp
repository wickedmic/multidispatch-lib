#ifndef INCLGUARD_size_hpp
#define INCLGUARD_size_hpp

namespace meta
{
	/// returns the number of elements in List
	template<typename> struct size;

	template<template<typename...> class List, typename... Types>
	struct size<List<Types...>>
	{
		static const std::size_t value = sizeof...(Types);
	};
}

#endif
