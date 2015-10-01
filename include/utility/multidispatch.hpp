#ifndef INCLGUARD_multidispatch_hpp
#define INCLGUARD_multidispatch_hpp

#include <cstdlib>
#include <memory>
#include <type_traits>
#include "index_of.hpp"
#include "concat.hpp"
#include "prepend.hpp"
#include "map.hpp"
#include "cartesian_product.hpp"
#include "size.hpp"
#include "if.hpp"
#include "apply_list.hpp"

namespace md
{

	namespace _md_detail
	{
		/// meta function, which replaces the first type by the second
		template<typename, typename Type> struct replace
		{
			using type = Type;
		};

		template<typename...> struct list;
	}



	// is_handle
	template<typename>
	struct is_handle
		: public std::false_type
	{ };

	template<typename Type>
	using is_handle_t = typename is_handle<Type>::type;



	template<typename Type>
	struct make_list
	{
		using type = typename meta::if_c<is_handle<Type>::value, Type, _md_detail::list<Type>>::type;
	};

	// type_index
	namespace _md_detail
	{
		// type_index_impl prototype
		template<bool, typename... Lists>
		struct type_index_impl;

		// type_index_dispatch
		template<typename... Lists>
		struct type_index_dispatch;

		template<typename First, typename... OtherLists>
		struct type_index_dispatch<First, OtherLists...>
			: public type_index_impl<is_handle<First>::value, First, OtherLists...>
		{ };

		template<typename First>
		struct type_index_dispatch<First>
			: public type_index_impl<is_handle<First>::value, First>
		{ };

		// type_index_impl specializations
		template<template<typename...> class Handle, typename... Types, typename... OtherTypes>
		struct type_index_impl<true, Handle<Types...>, OtherTypes...>
		{
			/// returns the index of a set of types (given by thier ids)
			static std::size_t index(Handle<Types...> const& handle, OtherTypes const&... other_params)
			{
				auto size =
					meta::size<
						typename meta::apply_list<
							meta::cartesian_product,
							typename meta::map<
								make_list,
								_md_detail::list<OtherTypes...>
							>::type
						>::type::type
					>::value;

				return handle.type() * size + type_index_dispatch<OtherTypes...>::index(other_params...);
			}
		};

		template<template<typename...> class Handle, typename... Types>
		struct type_index_impl<true, Handle<Types...>>
		{
			static std::size_t index(Handle<Types...> const& handle)
			{
				return handle.type();
			}
		};

		template<typename Type, typename... OtherTypes>
		struct type_index_impl<false, Type, OtherTypes...>
		{
			static std::size_t index(Type const&, OtherTypes const&... other_params)
			{
				return type_index_dispatch<OtherTypes...>::index(other_params...);
			}
		};

		template<typename Type>
		struct type_index_impl<false, Type>
		{
			static std::size_t index(Type const&)
			{
				return 0;
			}
		};

	}

	/// provides the function 'index' which returns a unique index for the given type combination
	/**
		The given template parameters are forming the input parameters of the index-function.
		Each parameter can be a handle or non-handle type (see is_handle). A non-handle type
		is treated as a handle with just one type. When calling the index function the given type
		combination will define a unique index (here 'unique' means the returned integer identifies
		the given type combination for the given list of types).
	*/
	template<typename... Lists>
	struct type_index
		: public _md_detail::type_index_dispatch<Lists...>
	{ };

	template<>
	struct type_index<>
	{
		static std::size_t index()
		{
			return 0; // return dummy index
		}
	};



	template<typename Functor, typename TypeList> struct dispatch_function;

	template<typename Functor, template<typename...> class List, typename... Types>
	struct dispatch_function<Functor, List<Types...>>
	{
		static auto function(Functor functor, typename _md_detail::replace<Types,void const*>::type... params)
		{
			return functor(*const_cast<Types*>(reinterpret_cast<Types const*>(params))...);
		}
	};


	template<typename Functor, typename ListOfLists> struct function_table;

	template<typename Functor, template<typename...> class List, typename FirstList, typename... OtherLists>
	struct function_table<Functor, List<FirstList, OtherLists...>>
	{
		static auto get(std::size_t index)
		{
			// take the first type combination to determine the return type
			using function_t = decltype(&dispatch_function<Functor, FirstList>::function);

			// build static function table
			static function_t table[] = { dispatch_function<Functor, FirstList>::function, dispatch_function<Functor, OtherLists>::function... };

			return table[index];
		}
	};



	namespace _md_detail
	{
		template<typename Object>
		void const* get_object_address(Object&& object, std::false_type)
		{
			return reinterpret_cast<void const*>(&object);
		}

		template<typename Object>
		void const* get_object_address(Object&& object, std::true_type)
		{
			return get_object_address(object);
		}
	}

	template<typename Object>
	void const* get_object(Object&& object)
	{
		return _md_detail::get_object_address(std::forward<Object>(object), is_handle_t< std::remove_reference_t< Object > >{} );
	}


	// dispatcher
	template<
		typename    Functor, // functor to be called with the recovered types
		typename... Types    // a list of types, where the types can be handle and non-handle types
	>
	struct dispatcher
	{
		static auto function(std::size_t index)
		{
			return
				function_table<
					Functor,
					typename meta::apply_list<
						meta::cartesian_product,
						typename meta::map<
							make_list,
							_md_detail::list<
								typename std::remove_cv<Types>::type...
							>
						>::type
					>::type::type
				>::get(index);
		}

		template<typename _Functor, typename... Objects>
		static auto dispatch(_Functor&& functor, Objects&&... objects)
		{
			std::size_t index = type_index<typename std::remove_cv<Types>::type...>::index(std::forward<Objects>(objects)...);
			return function(index)(std::forward<_Functor>(functor), get_object(std::forward<Objects>(objects))...);
		}
	};

	/// case where no parameters are given
	/**
		same as directly calling functor()
	*/
	template<typename Functor>
	struct dispatcher<Functor>
	{
		static auto dispatch(Functor functor)
		{
			return functor();
		}
	};



	/// convenient function to have types deduced for dispatcher
	/**
		The return type is determined by the functor called with the first combination of parameters.
		Other parameter-type combinations must have the same return type.
	*/
	template<
		typename Functor, // functor which accepts as many parameters as given lists (and every type combination of these lists)
		typename... Types // a list of types, where the types can be handle and non-handle types
	>
	auto dispatch(Functor&& functor, Types&&... objects)
	{
		return
			dispatcher<
				Functor,
				typename std::remove_reference<Types>::type...
			>::dispatch(
				std::forward<Functor>(functor),
				std::forward<Types>(objects)...
			);
	}

}

#endif
