#ifndef INCLGUARD_concat_hpp
#define INCLGUARD_concat_hpp

namespace meta
{
	/// concat
	template<typename List1, typename List2> struct concat;

	template<template<typename...> class List1, typename... Types1, template<typename...> class List2, typename... Types2>
	struct concat<List1<Types1...>, List2<Types2...>>
	{
		using type = List1<Types1..., Types2...>;
	};

	template<typename List1, typename List2>
	using concat_t = typename concat<List1, List2>::type;
}

#endif
