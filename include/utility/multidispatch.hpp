#include <cstdlib>
#include <memory>
#include <type_traits>
#include "index_of.hpp"
#include "concat.hpp"
#include "prepend.hpp"
#include "map.hpp"
#include "cartesian_product.hpp"
#include "size.hpp"

namespace md
{
	struct empty_handle_exception {};

	template<typename TypeList> struct handle;

	template<template<typename...> class List, typename... Types>
	struct handle<List<Types...>>
	{
	private:
		/// interface for carried types
		struct concept
		{
			virtual ~concept() = default;

			virtual concept* copy() const = 0;
			virtual std::size_t type() const = 0;
			virtual void const* get() const = 0;
		};

		/// implementation of the concept interface
		template<typename Object>
		struct model : public concept
		{
			// ctor
			template<typename _Object>
			model(_Object&& object)
				: object(std::forward<_Object>(object))
			{ }

			virtual concept* copy() const override { return new model(*this); }
			virtual std::size_t type() const override { return meta::index_of<List<Types...>, Object>::value; }
			virtual void const* get() const override { return reinterpret_cast<void const*>(&object); }

			Object object;
		};

	public:
		handle() = default;

		template<typename Object>
		handle(Object&& object)
			: _object(new model<typename std::remove_reference<Object>::type>(std::forward<Object>(object)))
		{ }

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


		/// returns the index of the carried type
		/**
			The index is the position in the given type list.
		*/
		std::size_t type() const
		{
			if(_object)
				return _object->type();
			else
				throw empty_handle_exception{};
		}


		/// returns a const void pointer to the carried object
		/**
			If the handle is empty nullptr will be returned.

			This function is intended for internal use. If you just want
			to access the carried type use the cast() function to access
			the carried type safely.
		*/
		void const* get() const
		{
			if(_object)
				return _object->get();
			else
				return nullptr;
		}

		/// sets the given object as the given type
		/**
			RequestedType must be in the list of types of this handle.
			The given object must be convertible to the RequestedType.
		*/
		template<typename RequestedType, typename ActualType>
		void set(ActualType&& object)
		{
			_object.reset(new model<RequestedType>(std::forward<ActualType>(object)));
		}

		/// removes the carried object making the handle empty
		void reset()
		{
			_object.reset();
		}

	private:
		std::unique_ptr<concept> _object;
	};


	/// returns a pointer to the carried type
	/**
		If the carried type corresponds to the requested type a valid
		pointer	is returned, nullptr otherwise. Use handle::type() to
		get the index into the handle's type list to identify the carried type.
	*/
	template<typename Type, template<typename...> class List, typename... Types>
	Type* cast(handle<List<Types...>> const& handle)
	{
		if(meta::index_of<List<Types...>, Type>::value == handle.type())
			return const_cast<Type*>(reinterpret_cast<Type const*>(handle.get()));
		else
			return nullptr;
	}


	namespace _md_detail
	{
		template<typename, typename Type> struct replace
		{
			using type = Type;
		};
	}



	// type_index
	template<typename... Lists> struct type_index;

	template<typename FirstList, typename... Lists>
	struct type_index<FirstList, Lists...>
	{
		/// returns the index of a set of types (given by thier ids)
		static std::size_t index(std::size_t type_id, typename _md_detail::replace<Lists,std::size_t>::type... type_ids)
		{
			return type_id * meta::size<typename meta::cartesian_product<Lists...>::type>::value + type_index<Lists...>::index(type_ids...);
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
		static auto function(Functor functor, typename _md_detail::replace<Types,void const>::type*... params)
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



	/// dispatcher
	template<
		typename    Functor,  // functor to be call with the recovered types
		typename... TypeLists // lists of types describing the available type for each parameter
	>
	struct dispatcher
	{
		static auto function(std::size_t index)
		{
			return function_table<Functor, typename meta::cartesian_product<TypeLists...>::type>::get(index);
		}

		template<typename _Functor, typename... Handles>
		static auto dispatch(_Functor&& functor, Handles&&... handles)
		{
			std::size_t index = type_index<TypeLists...>::index(handles.type()...);
			return function(index)(std::forward<_Functor>(functor), handles.get()...);
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



	/// get_list
	template<typename> struct get_list;

	template<template<typename> class Handle, typename List>
	struct get_list<Handle<List>>
	{
		using type = List;
	};



	/// convenient function to have types deduced for dispatcher
	/**
		The return type is determined by the functor called with the first combination of parameters.
		Other parameter-type combinations must have the same return type.
	*/
	template<
		typename Functor, // functor which accepts as many parameters as given lists (and every type combination of these lists)
		typename... Handles // TODO: pack of lists containing the types available of the respective handle
	>
	auto dispatch(Functor&& functor, Handles&&... handles)
	{
		return
			dispatcher<
				Functor,
				typename get_list<typename std::remove_cv<typename std::remove_reference<Handles>::type>::type>::type...
			>::dispatch(
				std::forward<Functor>(functor),
				std::forward<Handles>(handles)...
			);
	}

}
