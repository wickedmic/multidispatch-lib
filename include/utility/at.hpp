#ifndef INCLGUARD_at_hpp
#define INCLGUARD_at_hpp

namespace meta
{
	/// returns the N-th element of List
	template<typename List, std::size_t N>
	struct at;

	template<template<typename...> class List, typename FirstItem, typename... OtherItems, std::size_t N>
	struct at<List<FirstItem, OtherItems...>, N>
	{
		static_assert(N < sizeof...(OtherItems)+1, "index out of bounds");
		using type = typename at<List<OtherItems...>, N-1>::type;
	};

	template<template<typename...> class List, typename FirstItem, typename... OtherItems>
	struct at<List<FirstItem, OtherItems...>, 0>
	{
		using type = FirstItem;
	};

	template<template<typename...> class List, std::size_t N>
	struct at<List<>, N>
	{
		static_assert(sizeof(List<>) == 0, "index out of bounds");
	};

	template<typename List, std::size_t N>
	using at_t = typename at<List,N>::type;
}
#endif
