#include <cstdlib>
#include <memory>
#include <index_of.hpp>
#include <boost/mpl/eval_if.hpp>

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


	template<typename Type, typename Functor>
	struct dispatch_function
	{
		static void func(void* p1, Functor functor)
		{
			functor(*reinterpret_cast<Type*>(p1));
		}

		using function_t = void(*)(void*, Functor);
		constexpr static function_t value = func;
	};


	template<typename TypeList1, typename Functor> struct dispatcher;

	template<template<typename...> class List, typename... Types, typename Functor>
	struct dispatcher<List<Types...>, Functor>
	{
		using function_t = void(*)(void*, Functor);
		//using table_t = function_t[sizeof...(Types2...)];

		static function_t function(std::size_t index)
		{
			static function_t vtable[] = { dispatch_function<Types, Functor>::value... };
			return vtable[index];
		}

		static void dispatch(handle<List<Types...>>& h, Functor functor)
		{
			function(h.type())(h.get(), functor);
		}
	};

	template<typename List, typename Functor>
	void dispatch(handle<List>& handle, Functor functor)
	{
		dispatcher<List, Functor>::dispatch(handle, functor);
	}


	/// append
	template<typename TypeList, typename Type> struct append;

	template<template<typename...> class List, typename... Types, typename Type>
	struct append<List<Types...>, Type>
	{
		using type = List<Types..., Type>;
	};


	/// concat
	template<typename List1, typename List2> struct concat;

	template<template<typename...> class List1, typename... Types1, template<typename...> class List2, typename... Types2>
	struct concat<List1<Types1...>, List2<Types2...>>
	{
		using type = List1<Types1..., Types2...>;
	};

	template<typename List1, typename List2>
	using concat_t = typename concat<List1, List2>::type;

	/// prepends Type to each list in ListOfLists
	template<typename Type, typename ListOfLists> struct foreach_prepend;

	template<typename Type, template<typename...> class ListOfLists, template<typename...> class SubList, typename... SubListEntries, typename... OtherSubLists>
	struct foreach_prepend<Type, ListOfLists<SubList<SubListEntries...>, OtherSubLists...>>
	{
		using type =
			typename concat<
				ListOfLists<SubList<Type, SubListEntries...>>,
				typename foreach_prepend<Type, ListOfLists<OtherSubLists...>>::type
			>::type;
	};

	template<typename Type, template<typename...> class ListOfLists, template<typename...> class SubList, typename... SubListEntries>
	struct foreach_prepend<Type, ListOfLists<SubList<SubListEntries...>>>
	{
		using type = ListOfLists<SubList<Type, SubListEntries...>>;
	};

	template<typename Type, template<typename...> class ListOfLists>
	struct foreach_prepend<Type, ListOfLists<>>
	{
		using type = ListOfLists<>;
	};


	/// if_
	template<bool, typename TrueType, typename FalseType>
	struct if_
	{
		using type = FalseType;
	};


	template<typename TrueType, typename FalseType>
	struct if_<true, TrueType, FalseType>
	{
		using type = TrueType;
	};


	/// build_tuple
	template<typename... List> struct cartesian_product;

	template<template<typename...> class List, typename Type, typename... Types, typename... OtherLists>
	struct cartesian_product<List<Type, Types...>, OtherLists...>
	{
		using type =
			typename concat<
				typename foreach_prepend<
					Type,
					typename if_<
						sizeof...(OtherLists) != 0,
						typename cartesian_product<OtherLists...>::type,
						List<List<>>
					>::type
				>::type,
				typename cartesian_product<
					List<Types...>,
					OtherLists...
				>::type
			>::type;
	};

	template<template<typename...> class List, typename... OtherLists>
	struct cartesian_product<List<>, OtherLists...>
	{
		using type = List<>;
	};

	template<template<typename...> class List>
	struct cartesian_product<List<>>
	{
		using type = List<>;
	};

	template<>
	struct cartesian_product<>
	{
		// sadly we don't know the list type here, so we return void
		// and compensate this 'wrong' type by using if_ (see above)
		using type = void;
	};

}
