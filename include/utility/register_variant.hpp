#include "multidispatch.hpp"
#include "variant.hpp"

namespace md
{
	template<typename... Types>
	struct is_handle<utility::variant<Types...>>
		: public std::true_type
	{ };

	template<typename... Types>
	struct is_handle<utility::variant<Types...> const>
		: public std::true_type
	{ };
}

namespace utility
{
	template<typename... Types>
	void const* get_object_address(utility::variant<Types...> const& v)
	{
		return v.get();
	}
}
