#ifndef INCLGUARD_register_boost_variant_hpp
#define INCLGUARD_register_boost_variant_hpp

#include <boost/variant.hpp>
#include "is_handle.hpp"

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

namespace boost // for adl
{
	template<typename... Types>
	void const* get_object_address(boost::variant<Types...> const& v)
	{
		using md::_detail::get_object_address_visitor;
		return boost::apply_visitor(get_object_address_visitor<Types...>{}, v);
	}
}

#endif
