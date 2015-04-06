#include <cstdlib>
#include <memory>
#include "index_of.hpp"
#include "concat.hpp"
#include "prepend.hpp"
#include "map.hpp"
#include "cartesian_product.hpp"

namespace md
{
	template<typename TypeList> struct handle;

	template<template<typename...> class List, typename... Types>
	struct handle<List<Types...>>
	{
	private:
		struct concept
		{
			virtual ~concept() = default;
			virtual concept* copy() = 0;
			virtual std::size_t type() = 0;
			virtual void* get() = 0;
		};

		template<typename Object>
		struct model : public concept
		{
			model(Object object) : object(std::move(object)) { }
			virtual concept* copy() { return new model(*this);}
			virtual std::size_t type() { return utility::index_of<List<Types...>, Object>::value; }
			virtual void* get() { return reinterpret_cast<void*>(&object); }

			Object object;
		};

	public:
		handle() = default;

		template<typename Object>
		handle(Object object) : _object(new model<Object>(std::move(object))) { }

		handle(handle const& other) : _object(other._object->copy()) { }

		handle(handle&& other) noexcept = default;

		handle& operator= (handle const& other)
		{
			handle tmp(other);
			*this = std::move(tmp);
			return *this;
		}

		handle& operator= (handle&& other) noexcept = default;

		operator bool () const { return static_cast<bool>(_object); }

		std::size_t type() { return _object->type(); }
		void* get() { return _object->get(); }

	private:
		std::unique_ptr<concept> _object;
	};


	namespace _md_detail
	{
		template<typename, typename Type> struct replace
		{
			using type = Type;
		};
	}


	/// returns the number of elements in List
	template<typename> struct size;

	template<template<typename...> class List, typename Type, typename... Types>
	struct size<List<Type, Types...>>
	{
		static const std::size_t value = size<List<Types...>>::value + 1;
	};

	template<template<typename...> class List>
	struct size<List<>>
	{
		static const std::size_t value = 0;
	};


	// type_index
	template<typename... Lists> struct type_index;

	template<typename FirstList, typename... Lists>
	struct type_index<FirstList, Lists...>
	{
		/// returns the index of a set of types (given by thier ids)
		static std::size_t index(std::size_t type_id, typename _md_detail::replace<Lists,std::size_t>::type... type_ids)
		{
			return type_id * size<typename meta::cartesian_product<Lists...>::type>::value + type_index<Lists...>::index(type_ids...);
		}
	};

	template<typename List>
	struct type_index<List>
	{
		static std::size_t index(std::size_t type_id)
		{
			return type_id;
		}
	};

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
		static auto function(Functor functor, typename _md_detail::replace<Types,void>::type*... params)
		{
			return functor(*reinterpret_cast<Types*>(params)...);
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

	template<
		typename Functor, // functor to be call with the recovered types
		typename... TypeLists // lists of type describing the available type for each parameter
	>
	struct dispatcher
	{
		static auto function(std::size_t index)
		{
			return function_table<Functor, typename meta::cartesian_product<TypeLists...>::type>::get(index);
		}

		static auto dispatch(Functor functor, handle<TypeLists>&... handles)
		{
			std::size_t index = type_index<TypeLists...>::index(handles.type()...);
			return function(index)(functor, handles.get()...);
		}
	};

	/// case where no handles are given
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
	template<
		typename Functor, // functor which accepts as many parameters as given lists (and every type combination of these lists)
		typename... Lists // pack of lists containing the types available of the respective handle
	>
	auto dispatch(Functor functor, handle<Lists>&... handles)
	{
		return dispatcher<Functor, Lists...>::dispatch(functor, handles...);
	}

}
