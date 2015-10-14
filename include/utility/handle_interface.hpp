#ifndef INCLGUARD_handle_interface_hpp
#define INCLGUARD_handle_interface_hpp

#include <type_traits>

// This file defines the interface for handle types.
//
// A handle is a union container, which carries a specific
// type at runtime. A handle type defines a finite list of types
// it can carry. Each type is identified by a unique index, which
// can be queried from the handle at runtime to deduce its type.
//
// A handle type must implement the following functions.
// - the meta function is_handle, which takes the handle type and returns true_type
// - the meta function type_list, which takes the handle type and returns a type list,
//     which defines the kind of types the handle can carry and thier index.
//     If the handle itself is such a type list, the specialization of the function
//     can be omitted (see below).
// - the runtime function type_id, which takes a handle object and returns the index
//     of the carried type. The zero based index of a type is given by its position
//     in the type list returned by the meta function type_list.
// - the runtime function get_object_address, which takes a handle object and returns
//     a void const* to the carried object.

namespace md
{
	/// returns true_type for handle type
	/**
	  	Implement a template specialization for your handle type
		which returns true_type.
	*/
	template<typename>
	struct is_handle
		: public std::false_type
	{ };

	/// convenience function for is_handle
	template<typename Type>
	using is_handle_t = typename is_handle<Type>::type;


	/// returns a suitable list of types
	/**
		This meta function is an interface to support type lists
		which are not build with variadic templates. These type
		lists are often build with a fixed number of arguments,
		where unused parameters are marked with a 'void' type.
		This meta function provides a facility to convert such a
		type list into a type which uses variadic templates.

		By default it is assumed that the given type is a list
		build with variadic templates.
	*/
	template<typename Handle>
	struct type_list
	{
		using type = Handle;
	};
}

#endif

