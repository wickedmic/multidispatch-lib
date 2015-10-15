#ifndef INCLGUARD_register_boost_variant_hpp
#define INCLGUARD_register_boost_variant_hpp

#include <type_traits>
#include <boost/variant.hpp>
#include "handle_interface.hpp"
#include "filter.hpp"
#include "size.hpp"

/// --- is_handle ---
namespace md
{
	template<typename... Types>
	struct is_handle<boost::variant<Types...>>
		: public std::true_type
	{ };

	template<typename... Types>
	struct is_handle<boost::variant<Types...> const>
		: public std::true_type
	{ };
}

/// --- type_list ---
namespace md
{
	template<typename ...> struct list;

	namespace _detail
	{
		template<typename>
		struct is_type : public std::true_type
		{ };

		template<>
		struct is_type<boost::detail::variant::void_> : public std::false_type
		{ };
	}

	template<typename... Types>
	struct type_list<boost::variant<Types...>>
	{
		using type = typename meta::filter<_detail::is_type, list<Types...>>::type;
	};

}

/// --- get_object_address ---
namespace md
{
	namespace _detail
	{
		template<typename... Types>
		struct get_object_address_visitor;

		template<typename FirstType, typename... OtherTypes>
		struct get_object_address_visitor<FirstType, OtherTypes...>
			: public get_object_address_visitor<OtherTypes...>
		{
			using base = get_object_address_visitor<OtherTypes...>;
			using base::operator();

			void const* operator()(FirstType const& object) const
			{
				return reinterpret_cast<void const*>(&object);
			}
		};

		template<typename Type>
		struct get_object_address_visitor<Type>
			: public boost::static_visitor<void const*>
		{
			void const* operator()(Type const& object) const
			{
				return reinterpret_cast<void const*>(&object);
			}
		};
	}
}

namespace boost
{
	template<typename... Types>
	void const* get_object_address(boost::variant<Types...> const& v)
	{
		using md::_detail::get_object_address_visitor;
		return boost::apply_visitor(get_object_address_visitor<Types...>{}, v);
	}
}

// --- type_id ---
namespace md
{
	namespace _detail
	{
		template<std::size_t N, typename List>
		struct type_id_visitor;

		template<std::size_t N, template<typename...> class List, typename FirstType, typename... OtherTypes>
		struct type_id_visitor<N, List<FirstType, OtherTypes...>>
			: public type_id_visitor<N, List<OtherTypes...>>
		{
			using base = type_id_visitor<N, List<OtherTypes...>>;
			using base::operator();

			std::size_t operator()(FirstType const&) const
			{
				return N - sizeof...(OtherTypes) - 1;
			}
		};

		template<std::size_t N, template<typename...> class List, typename Type>
		struct type_id_visitor<N, List<Type>>
			: public boost::static_visitor<std::size_t>
		{
			std::size_t operator()(Type const&) const
			{
				return N - 1;
			}
	   	};
	}
}

namespace boost
{
	template<typename... Types>
	std::size_t type_id(boost::variant<Types...> const& variant)
	{
		using type_list = typename md::type_list<boost::variant<Types...>>::type;
		return boost::apply_visitor(
			md::_detail::type_id_visitor<
				meta::size<type_list>::value,
				type_list
			>{},
			variant);
	}
}

#endif
