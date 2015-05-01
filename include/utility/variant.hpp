#ifndef INCLGUARD_variant_hpp
#define INCLGUARD_variant_hpp

#include <cstddef>
#include <utility>
#include <memory>
#include "index_of.hpp"

namespace utility
{
	struct empty_variant_exception {};

	template<typename...>
	struct variant
	{
	private:
		/// interface for carried types
		struct concept
		{
			virtual ~concept() = default;

			virtual concept* copy() const = 0;
			virtual std::size_t type() const = 0;
			virtual void const* get() const = 0;
		};

		/// implementation of the concept interface
		template<typename Object>
		struct model : public concept
		{
			// ctor
			template<typename _Object>
			model(_Object&& object)
				: object(std::forward<_Object>(object))
			{ }

			virtual concept* copy() const override { return new model(*this); }
			virtual std::size_t type() const override { return meta::index_of<variant, Object>::value; }
			virtual void const* get() const override { return reinterpret_cast<void const*>(&object); }

			Object object;
		};

	public:
		variant() = default;

		template<typename Object>
		variant(Object&& object)
			: _object(new model<typename std::remove_reference<Object>::type>(std::forward<Object>(object)))
		{ }

		variant(variant const& other) : _object(other._object->copy()) { }

		variant(variant&& other) noexcept = default;

		variant& operator= (variant const& other)
		{
			variant tmp(other);
			*this = std::move(tmp);
			return *this;
		}

		variant& operator= (variant&& other) noexcept = default;

		operator bool () const { return static_cast<bool>(_object); }


		/// returns the index of the carried type
		/**
			The index is the position in the given type list.
		*/
		std::size_t type() const
		{
			if(_object)
				return _object->type();
			else
				throw empty_variant_exception{};
		}


		/// returns a const void pointer to the carried object
		/**
			If the variant is empty nullptr will be returned.

			This function is intended for internal use. If you just want
			to access the carried type use the cast() function to access
			the carried type safely.
		*/
		void const* get() const
		{
			if(_object)
				return _object->get();
			else
				return nullptr;
		}

		/// sets the given object as the given type
		/**
			RequestedType must be in the list of types of this variant.
			The given object must be convertible to the RequestedType.
		*/
		template<typename RequestedType, typename ActualType>
		void set(ActualType&& object)
		{
			_object.reset(new model<RequestedType>(std::forward<ActualType>(object)));
		}

		/// removes the carried object making the variant empty
		void reset()
		{
			_object.reset();
		}

	private:
		std::unique_ptr<concept> _object;
	};


	/// returns a pointer to the carried type
	/**
		If the carried type corresponds to the requested type a valid
		pointer	is returned, nullptr otherwise.

		\note Use variant::type() to get the index into the variant's type list to identify the carried type.
		\note If the given variant is empty this function will not throw an exception but also return nullptr.
	*/
	template<typename Type, typename... Types>
	Type* cast_variant(utility::variant<Types...> const& variant)
	{
		if(variant.get() && meta::index_of<utility::variant<Types...>, Type>::value == variant.type())
			return const_cast<Type*>(reinterpret_cast<Type const*>(variant.get()));
		else
			return nullptr;
	}
}

#endif

