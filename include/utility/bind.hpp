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


	// arg
	template<std::size_t N> struct arg;


	// is_argument
	template<typename>
	struct is_argument : public std::false_type
	{ };

	template<std::size_t N>
	struct is_argument<arg<N>> : public std::true_type
	{ };




	// pick_argument
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
		// error message for empty List
		static_assert(sizeof(Placeholder) == 0, "placeholder refers to non-existing parameter");
	};



	// applied_param_list
	template<typename OrigParamList, typename AppliedParamList>
	struct applied_param_list;

	template<
		template<typename...> class OrigParamList,
		typename FirstParam,
		typename... OtherParams,
		typename AppliedParamList
	>
	struct applied_param_list<OrigParamList<FirstParam, OtherParams...>, AppliedParamList>
	{
		using type =
			typename meta::concat<
				bind_detail::list<
					typename if_<
						typename is_argument<FirstParam>::type,
						typename pick_argument<FirstParam, AppliedParamList>::type,
						FirstParam
					>::type
				>,
				typename applied_param_list<bind_detail::list<OtherParams...>, AppliedParamList>::type
			>::type;
	};

	template<
		template<typename...> class OrigParamList,
		typename AppliedParamList
	>
	struct applied_param_list<OrigParamList<>, AppliedParamList>
	{
		using type = bind_detail::list<>;
	};




	// apply_function
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



	// bind
	template<template<typename...> class function, typename... Params>
	struct bind
	{
		template<typename... ApplyParams>
		struct apply
		{
			using type =
				typename apply_function<
					function,
					typename applied_param_list<
						bind_detail::list<Params...>,
						bind_detail::list<ApplyParams...>
					>::type
				>::type;
		};
	};
}

#endif

