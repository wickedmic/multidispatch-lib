cmake_minimum_required(VERSION 2.8.11)
cmake_policy(VERSION 2.8.11)

project(utility_tests)

# test names
set(test_list
	test_apply_list
	test_bind
	test_cartesian_product
	test_filter
	test_fold
	test_index_of
	test_map
	test_multidispatch
	test_variant
	test_visitor
	test_boost_variant
)

# compiler specific settings
message(STATUS "compiler-id: ${CMAKE_CXX_COMPILER_ID}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
	message(STATUS "compiler flags (CMAKE_CXX_GFLAGS) is now '${CMAKE_CXX_FLAGS}'")

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
	message(STATUS "compiler flags now '${CMAKE_CXX_FLAGS}'")

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")

else()
	message("warning: unknown compiler")

endif()

enable_testing()

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIR})

include_directories("../include/utility/")

foreach(test ${test_list})
	add_executable(${test} "${test}.cpp")
	target_link_libraries(${test} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
	add_test(${test} ${test})
endforeach()

