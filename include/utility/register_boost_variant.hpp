#ifndef INCLGUARD_register_boost_variant_hpp
#define INCLGUARD_register_boost_variant_hpp

#include <boost/variant.hpp>

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

namespace boost
{
	template<typename... Types>
	struct get_object_address_visitor_impl;

	template<typename FirstType, typename... OtherTypes>
	struct get_object_address_visitor_impl<FirstType, OtherTypes...>
		: public get_object_address_visitor_impl<OtherTypes...>
	{
		using base = get_object_address_visitor_impl<OtherTypes...>;
		using base::operator();

		void const* operator()(FirstType& object) const
		{
			return reinterpret_cast<void const*>(&object);
		}
	};

	template<>
	struct get_object_address_visitor_impl<>
		: public boost::static_visitor<void const*>
	{ };


	template<typename VariantType>
	struct get_object_address_visitor;

	template<typename... Types>
	struct get_object_address_visitor<boost::variant<Types...>>
		: public get_object_address_visitor_impl<Types...>
	{ }

	template<typename... Types>
	void const* get_object_address(boost::variant<Types...> const& v)
	{
		return boost::apply_visitor(get_object_address_visitor<boost::visitor<Types...>>{}, v);
	}
}

#endif
