#ifndef INCLGUARD_bind_hpp
#define INCLGUARD_bind_hpp

#include <type_traits>
#include "if.hpp"
#include "concat.hpp"

namespace meta
{
	namespace bind_detail
	{
		template<typename...> struct list;
	};


	// placeholder to represent an agrument
	template<std::size_t N> struct arg;


	// meta template function, which returns true if given type is of type arg
	template<typename>
	struct is_argument : public std::false_type
	{ };

	template<std::size_t N>
	struct is_argument<arg<N>> : public std::true_type
	{ };




	// takes arg<N> as argument and returns the N-th element from the given list
	template<typename Argument, typename List>
	struct pick_argument
	{
		// Return void if the Argument is not of type meta::arg.
		// This is necessary because both TrueType and FalseType
		// of if_ will be evaluated. So 'type' must always be present
		// even if it is not being used.
		using type = void;
	};


	template<
		template<std::size_t> class Placeholder,
		std::size_t N,
		template<typename...> class List,
		typename FirstItem,
		typename... OtherItems
	>
	struct pick_argument<Placeholder<N>, List<FirstItem, OtherItems...>>
	{
		static_assert(N <= sizeof...(OtherItems)+1, "placeholder refers to non-existing parameter");

		using type = typename pick_argument<Placeholder<N-1>, List<OtherItems...>>::type;
	};


	template<
		template<std::size_t> class Placeholder,
		template<typename...> class List,
		typename FirstItem,
		typename... OtherItems
	>
	struct pick_argument<Placeholder<0>, List<FirstItem, OtherItems...>>
	{
		using type = FirstItem;
	};

	template<typename Placeholder, template<typename...> class List>
	struct pick_argument<Placeholder, List<>>
	{
		static_assert(sizeof(Placeholder) == 0, "placeholder refers to non-existing parameter");
	};



	// replaces all arg<N> types in UntransformedParams with thier coresponding type from AvailableParams
	template<typename UntransformedParams, typename AvailableParams>
	struct transform_params;

	template<
		template<typename...> class UntransformedParams,
		typename FirstParam,
		typename... OtherParams,
		typename AvailableParams
	>
	struct transform_params<UntransformedParams<FirstParam, OtherParams...>, AvailableParams>
	{
		using type =
			typename meta::concat<
				bind_detail::list<
					typename if_<
						typename is_argument<FirstParam>::type,
						typename pick_argument<FirstParam, AvailableParams>::type,
						FirstParam
					>::type
				>,
				typename transform_params<bind_detail::list<OtherParams...>, AvailableParams>::type
			>::type;
	};

	template<
		template<typename...> class UntransformedParams,
		typename AvailableParams
	>
	struct transform_params<UntransformedParams<>, AvailableParams>
	{
		using type = bind_detail::list<>;
	};




	// applies the given parameter list to the given meta function
	template<template<typename...> class Function, typename Params>
	struct apply_function;

	template<
		template<typename...> class Function,
		template<typename...> class List,
		typename... Params
	>
	struct apply_function<Function, List<Params...>>
	{
		using type = typename Function<Params...>::type;
	};



	// meta function wrapper to change the signature of the function
	template<template<typename...> class function, typename... Params>
	struct bind
	{
		template<typename... AvailableParams>
		struct apply
		{
			using type =
				typename apply_function<
					function,
					typename transform_params<
						bind_detail::list<Params...>,
						bind_detail::list<AvailableParams...>
					>::type
				>::type;
		};
	};
}

#endif

