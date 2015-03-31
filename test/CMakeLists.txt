cmake_minimum_required(VERSION 2.8.11)
cmake_policy(VERSION 2.8.11)

project(utility_tests)

# header files
set(
	painter_HEADER
)


# files to compile
set(
	painter_SRCS
)


if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE DEBUG )
endif()

# compiler specific settings
message(STATUS "compiler-id: ${CMAKE_CXX_COMPILER_ID}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
	message(STATUS "compiler flags (CMAKE_CXX_GFLAGS) is now '${CMAKE_CXX_FLAGS}'")

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

	message(WARNING "todo: decide which stl to use; on mac os add '-stdlib=libc++' to CXXFLAGS")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
	message(STATUS "compiler flags now '${CMAKE_CXX_FLAGS}'")

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")

else()
	message("warning: unknown compiler")

endif()

#distinct between 32bit and 64bit
get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)

# pkg-config support (pkg_search_module/pkg_check_modules)
find_package(PkgConfig QUIET)

# windows sdk
if(WIN32)
	get_filename_component(WindowsSDK_REGPATH "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows;CurrentInstallFolder]" ABSOLUTE)
	unset(WindowsSDK_PATH_HINTS)
	list(APPEND WindowsSDK_PATH_HINTS "${WindowsSDK_PATH}" "$ENV{WindowsSDKDIR}" "${WindowsSDK_REGPATH}")
	foreach(path_hint ${WindowsSDK_PATH_HINTS})
		list(APPEND WindowsSDK_INCLUDE_DIR_HINTS "${path_hint}/include")
		if(LIB64)
			list(APPEND WindowsSDK_LIBRARY_DIR_HINTS "${path_hint}/lib/x64")
		else()
			list(APPEND WindowsSDK_LIBRARY_DIR_HINTS "${path_hint}/lib")
		endif()
	endforeach()

	list(APPEND CMAKE_PREFIX_PATH "${WindowsSDK_PATH_HINTS}")

	find_path(WindowsSDK_INCLUDE_DIR Windows.h HINTS "${WindowsSDK_INCLUDE_DIR_HINTS}")
	find_path(WindowsSDK_LIBRARY_DIR winmm.lib HINTS "${WindowsSDK_LIBRARY_DIR_HINTS}")
	find_library(GLU_LIB NAMES glu glu32 HINTS "${WindowsSDK_LIBRARY_DIR}")
endif()


enable_testing()

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

include_directories("../include/utility/")

add_executable(test_visitor "test_visitor.cpp")
target_link_libraries(test_visitor)
add_test(test_visitor test_visitor)

add_executable(test_multidispatch "test_multidispatch.cpp")
target_link_libraries(test_multidispatch)
add_test(test_multidispatch test_multidispatch)

add_executable(test_index_of "test_index_of.cpp")
target_link_libraries(test_index_of)
add_test(test_index_of test_index_of)

add_executable(test_fold "test_fold.cpp")
target_link_libraries(test_fold)
add_test(test_fold test_fold)
