#ifndef INCLGUARD_prepend_hpp
#define INCLGUARD_prepend_hpp

namespace meta
{
	/// prepend
	template<typename Item, typename List> struct prepend;

	template<typename Item, template<typename...> class List, typename... Items>
	struct prepend<Item, List<Items...>>
	{
		using type = List<Item, Items...>;
	};

	template<typename Item, typename List>
	using prepend_t = typename prepend<Item, List>::type;
}

#endif
