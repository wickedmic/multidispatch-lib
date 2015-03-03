#include <type_traits>

namespace utility
{
	/// visitor base class
	template<typename...> class visitor_base;

	template<typename Visitable, typename... FurtherVisitables>
	struct visitor_base<Visitable, FurtherVisitables...> : public visitor_base<FurtherVisitables ...>
	{
		using visitor_base<FurtherVisitables...>::_visit;
		virtual void _visit(Visitable& visitable) const = 0;
	};

	template<typename Visitable>
	struct visitor_base<Visitable>
	{
		virtual void _visit(Visitable& visitable) const = 0;
	};

	namespace detail
	{
		/// helper class which implements the virtual functions from visitor_base and forwards them to the derived class
		template<typename...> class visitor_impl;

		template<typename Derived, typename VisitorBase, typename Visitable, typename... FurtherVisitables>
		struct visitor_impl<Derived, VisitorBase, Visitable, FurtherVisitables...> : public visitor_impl<Derived, VisitorBase, FurtherVisitables...>
		{
			virtual void _visit(Visitable& visitable) const override
			{
				static_cast<Derived const*>(this)->visit(visitable);
			}
		};

		template<typename Derived, typename VisitorBase, typename Visitable>
		struct visitor_impl<Derived, VisitorBase, Visitable> : public VisitorBase
		{
			virtual void _visit(Visitable& visitable) const override
			{
				static_cast<Derived const*>(this)->visit(visitable);
			}
		};
	}

	/// base class for visitor implementations
	template<typename Derived, typename VisitorBase> struct visitor;

	template<typename Derived, template<typename...> class VisitorBase, typename... Visitables>
	struct visitor<Derived, VisitorBase<Visitables...>> : public detail::visitor_impl<Derived, VisitorBase<Visitables...>, Visitables...>
	{
	};


	/// visitor base class which accepts const visitables
	template<typename... Visitables>
	using const_visitor_base = visitor_base<std::add_const_t<Visitables>...>;

	/// meta function to build a const visitor from a non const visitor
	template<typename Visitor> struct make_const_visitor;

	template<template<typename...> class Visitor, typename... Visitables>
	struct make_const_visitor<Visitor<Visitables...>>
	{
		using type = const_visitor_base<Visitables...>;
	};

	template<typename Visitor>
	using make_const_visitor_t = typename make_const_visitor<Visitor>::type;

	/// inverse function to make_const_visitor
	template<typename Visitor> struct make_non_const_visitor;

	template<template<typename...> class Visitor, typename... Visitables>
	struct make_non_const_visitor<Visitor<Visitables...>>
	{
		using type = visitor_base<std::remove_const_t<Visitables>...>;
	};

	template<typename Visitor>
	using make_non_const_visitor_t = typename make_non_const_visitor<Visitor>::type;
}
