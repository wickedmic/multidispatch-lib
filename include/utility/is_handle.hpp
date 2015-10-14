#ifndef INCLGUARD_is_handle_hpp
#define INCLGUARD_is_handle_hpp

#include <type_traits>

namespace md
{
	// is_handle
	template<typename>
	struct is_handle
		: public std::false_type
	{ };

	template<typename Type>
	using is_handle_t = typename is_handle<Type>::type;
}

#endif

