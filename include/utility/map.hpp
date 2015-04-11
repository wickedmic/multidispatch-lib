#ifndef INCLGUARD_map_hpp
#define INCLGUARD_map_hpp

namespace meta
{
	template<template<typename...> class function, typename List>
	struct map;

	template<template<typename...> class function, template<typename...> class List, typename... Items>
	struct map<function, List<Items...>>
	{
		using type = List<typename function<Items>::type...>;
	};

	template<template<typename...> class function, typename List>
	using map_t = typename map<function, List>::type;
}
#endif
