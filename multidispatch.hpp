#include <cstdlib>
#include <memory>
#include <index_of.hpp>

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

//	template<typename... Args1>
//	struct table
//	{
//		template<typename... Args2>
//		constexpr auto foo()
//		{
//			return [](void* p1, void* p2){ somefunc(static_cast<Head1*>(p1), static_cast<Head2*>(p1)); };
//		}
//	};

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

}
