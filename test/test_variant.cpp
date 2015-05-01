#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE variant
#include <boost/test/unit_test.hpp>

#include <tuple>
#include "variant.hpp"

BOOST_AUTO_TEST_CASE(variant_empty_test)
{
	using namespace utility;

	variant<int,float> h;
	BOOST_CHECK_EQUAL( static_cast<bool>(h), false );
	BOOST_CHECK( h.get() == nullptr );
	BOOST_CHECK_THROW( h.type(), empty_variant_exception );
}

BOOST_AUTO_TEST_CASE(variant_value_test)
{
	utility::variant<int,double,bool> variant;
	BOOST_CHECK( (static_cast<bool>(variant) == false) );

	variant = static_cast<int>(1234);
	BOOST_CHECK( (variant.type() == 0) );
	BOOST_CHECK( (*static_cast<int const*>(variant.get()) == 1234) );

	variant = static_cast<double>(5.678);
	BOOST_CHECK( (variant.type() == 1) );
	BOOST_CHECK( (*static_cast<double const*>(variant.get()) == 5.678) );

	variant = static_cast<bool>(true);
	BOOST_CHECK( (variant.type() == 2) );
	BOOST_CHECK( (*static_cast<bool const*>(variant.get()) == true) );
}

BOOST_AUTO_TEST_CASE(variant_cast_test)
{
	utility::variant<int, float, bool> variant;

	variant = 1234;
	BOOST_REQUIRE( (utility::cast_variant<int>(variant) != nullptr) );
	BOOST_CHECK(  (*utility::cast_variant<int>(variant) == 1234) );

	variant = 5.678f;
	BOOST_REQUIRE( (utility::cast_variant<float>(variant) != nullptr) );
	BOOST_CHECK(  (*utility::cast_variant<float>(variant) == 5.678f) );

	variant = true;
	BOOST_REQUIRE( (utility::cast_variant<bool>(variant) != nullptr) );
	BOOST_CHECK(  (*utility::cast_variant<bool>(variant) == true) );

	BOOST_CHECK( (utility::cast_variant<int>(variant) == nullptr) );

	utility::variant<int, float> const const_variant = 1234;
	*utility::cast_variant<int>(const_variant) = 5678;
	BOOST_CHECK_EQUAL( *utility::cast_variant<int>(const_variant), 5678 );
}

BOOST_AUTO_TEST_CASE(variant_get_set_reset_test)
{
	utility::variant<int, float> h;
	BOOST_CHECK( h.get() == nullptr );

	h.set<int>(3);
	BOOST_CHECK( h.get() != nullptr );
	BOOST_CHECK_EQUAL( *utility::cast_variant<int>(h), 3 );

	h.set<float>(4);
	BOOST_CHECK( h.get() != nullptr );
	BOOST_CHECK_EQUAL( *utility::cast_variant<float>(h), 4.0f );

	h.reset();
	BOOST_CHECK( h.get() == nullptr );
}

