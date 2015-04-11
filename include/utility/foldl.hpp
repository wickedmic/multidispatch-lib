#ifndef INCLGUARD_foldl_hpp
#define INCLGUARD_foldl_hpp

namespace meta
{
	template<template<typename...> class Functor, typename InitValue, typename List>
	struct foldl;

	template<template<typename...> class Functor, typename InitValue, template<typename...> class List, typename Head, typename... Tail>
	struct foldl<Functor, InitValue, List<Head, Tail...>>
	{
		using type =
			typename foldl<
				Functor,
				typename Functor<
					InitValue,
					Head
				>::type,
				List<Tail...>
			>::type;
	};

	template<template<typename,typename> class Functor, typename InitValue, template<typename...> class List>
	struct foldl<Functor, InitValue, List<>>
	{
		using type = InitValue;
	};
}

#endif

