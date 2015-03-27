#ifndef INCLGUARD_foldr_hpp
#define INCLGUARD_foldr_hpp

template<template<typename,typename> class Functor, typename InitValue, typename List>
struct foldr;

template<template<typename,typename> class Functor, typename InitValue, template<typename...> class List, typename Head, typename... Tail>
struct foldr<Functor, InitValue, List<Head, Tail...>>
{
	using type =
		typename Functor<
			Head,
			typename foldr<
				Functor,
				InitValue,
				List<Tail...>
			>::type
		>::type;
};

template<template<typename,typename> class Functor, typename InitValue, template<typename...> class List>
struct foldr<Functor, InitValue, List<>>
{
	using type = InitValue;
};

#endif

