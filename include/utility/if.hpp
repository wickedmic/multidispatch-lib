#ifndef INCLGUARD_if_hpp
#define INCLGUARD_if_hpp
#include <type_traits>

namespace meta
{
	/// if_
	template<typename, typename TrueType, typename FalseType>
	struct if_
	{
		using type = FalseType;
	};


	template<typename TrueType, typename FalseType>
	struct if_<std::true_type, TrueType, FalseType>
	{
		using type = TrueType;
	};

	// if_c
	template<bool, typename TrueType, typename FalseType>
	struct if_c
	{
		using type = FalseType;
	};


	template<typename TrueType, typename FalseType>
	struct if_c<true, TrueType, FalseType>
	{
		using type = TrueType;
	};
}

#endif

