cmake_minimum_required(VERSION 3.4)

if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(LIBYUC
    VERSION 2.0.0
    LANGUAGES CXX
    DESCRIPTION "Useful headers for C++ projects")

include(GNUInstallDirs)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
message(">> CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 17)

if(BUILD_TESTING)
    message("-- Test enabled")
    if(NOT CMAKE_BUILD_TYPE MATCHES Release)
	message("-- Code coverage enabled")
	include(CodeCoverage.cmake)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_COMPILER_FLAGS}")
	setup_target_for_coverage_lcov(
	    NAME coverage
	    EXECUTABLE ctest -j ${n_cores}
	    EXCLUDE "/usr/*"
	    "*/gtest/*"
	    "*.cc"
	    )
    else()
	message("-- Code coverage disabled for Release build")
    endif()
    enable_testing()

    find_package(Threads REQUIRED)
    # Enable ExternalProject CMake module
    include(ExternalProject)

    # Download and install GoogleTest
    ExternalProject_Add(
	gtest
	GIT_REPOSITORY https://github.com/google/googletest.git
	UPDATE_COMMAND ""
	INSTALL_COMMAND ""
	LOG_DOWNLOAD ON
	LOG_CONFIGURE ON
	LOG_BUILD ON)

    # Get GTest source and binary directories from CMake project
    ExternalProject_Get_Property(gtest source_dir binary_dir)

    # Create a libgtest target to be used as a dependency by test programs
    add_library(libgtest IMPORTED STATIC GLOBAL)
    add_dependencies(libgtest gtest)
    set_target_properties(libgtest PROPERTIES
	"IMPORTED_LOCATION" "${binary_dir}/lib/libgtest.a"
	"IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}")
    include_directories("${source_dir}/googletest/include")
else()
    message("-- Test disabled")
endif()

set(LIBYUC_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include")
# file(MAKE_DIRECTORY "${LIBYUC_INCLUDE_DIR}/yuc")

file(GLOB modules "${CMAKE_CURRENT_SOURCE_DIR}/*")
foreach(module ${modules})
    get_filename_component(module_name ${module} NAME)
    if(IS_DIRECTORY ${module} AND EXISTS ${module}/${module_name})
	message(">> Indexing module: ${module_name}")
	#	file(CREATE_LINK "${module}/${module_name}" "${LIBYUC_INCLUDE_DIR}/yuc/${module_name}")
	install(FILES "${module}/${module_name}"
	    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yuc")
	if (BUILD_TESTING)
	    if(EXISTS "${module}/CMakeLists.txt")
		add_subdirectory("${module}")
	    else()
		file(GLOB module_srcs ${module}/*.cc)
		if (module_srcs)
		    message("-- Adding test for: ${module_name}")
		    add_executable(test-${module_name} ${module_srcs})
		    target_link_libraries(test-${module_name} libgtest)
		    target_compile_definitions(test-${module_name}
			PRIVATE TEST_SRC_DIR="${module}")
		    add_test(NAME ${module_name} COMMAND test-${module_name})
		endif()
	    endif()
	endif()
    endif()
endforeach()
