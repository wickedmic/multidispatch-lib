#ifndef INCLGUARD_index_of_hpp
#define INCLGUARD_index_of_hpp

namespace utility
{
	template<class List, typename Type> struct index_of;

	template<template<typename...> class List, typename First, typename... Types, typename Type>
	struct index_of<List<First, Types...>, Type>
	{
		static constexpr unsigned value = index_of<List<Types...>, Type>::value + 1;
	};

	template<template<typename...> class List, typename... Types, typename Type>
	struct index_of<List<Type, Types...>, Type>
	{
		static constexpr unsigned value = 0;
	};

} // namespace utility

#endif

