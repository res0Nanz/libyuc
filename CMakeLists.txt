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
    find_program (lcov_program NAMES lcov)
    if(NOT EXISTS ${lcov_program} OR CMAKE_BUILD_TYPE MATCHES Release)
	message("-- Code coverage disabled")
    else()
	message("-- Code coverage enabled")
	include(CodeCoverage.cmake)
        append_coverage_compiler_flags()
	setup_target_for_coverage_lcov(
	    NAME coverage
            LCOV_ARGS --no-external
            BASE_DIRECTORY ${PROJECT_SOURCE_DIR}
	    EXECUTABLE ctest -j ${n_cores}
            EXCLUDE "*.cc" "*.inc"
	    )
    endif()
    enable_testing()

    find_package(GTest REQUIRED)
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
                    target_link_libraries(test-${module_name} PRIVATE GTest::gtest_main)
		    target_compile_definitions(test-${module_name}
			PRIVATE TEST_SRC_DIR="${module}")
                    gtest_discover_tests(test-${module_name} PROPERTIES RUN_SERIAL TRUE)
		endif()
	    endif()
	endif()
    endif()
endforeach()
