##################################################################
#
# Coverage Unit Test build recipe
#
# This CMake file contains the recipe for building the example unit tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

#
#
# NOTE on the subdirectory structures here:
#
# - "inc" provides local header files shared between the coveragetest,
#    wrappers, and overrides source code units
# - "coveragetest" contains source code for the actual unit test cases
#    The primary objective is to get line/path coverage on the FSW 
#    code units.
#

add_cfe_coverage_stubs(example-internal stubs/example_device_stubs.c stubs/libuart_stubs.c) 
target_link_libraries(coverage-example-internal-stubs ut_core_api_stubs ut_assert)
target_include_directories(coverage-example-internal-stubs PUBLIC $<TARGET_PROPERTY:example,INCLUDE_DIRECTORIES>)

 
# Use the UT assert public API, and allow direct
# inclusion of source files that are normally private
include_directories(${PROJECT_SOURCE_DIR}/fsw/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(${hwlib_MISSION_DIR}/fsw/public_inc)
include_directories(${PROJECT_SOURCE_DIR}/../../../../fsw/osal/ut_assert/inc)


# Add a coverage test executable called "example-ALL" that 
# covers all of the functions in example_app.  
#
# Also note in a more complex app/lib the coverage test can also
# be broken down into smaller units (in which case one should use
# a unique suffix other than "ALL" for each unit).  For example,
# OSAL implements a separate coverage test per source unit.
add_cfe_coverage_test(example ALL 
    "coveragetest/coveragetest_example_app.c"
    "../src/example_app.c"
)

add_cfe_coverage_test(example DEVICE 
    "coveragetest/coveragetest_example_device.c"
    "../../shared/example_device.c"
    "stubs/libuart_stubs.c"
)

add_cfe_coverage_dependency(example ALL example-internal)


# The example uses library functions provided by example_lib so must be linked
# with the example_lib stub library (this is mainly just an example of how this 
# can be done).
#add_cfe_coverage_dependency(example ALL example_lib)

