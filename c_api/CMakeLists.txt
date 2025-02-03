# c_api/CMakeLists.txt

add_subdirectory(src)

set(RRG_TEST_EXECUTABLE rrg_test)

add_executable(${RRG_TEST_EXECUTABLE} rrg_test.c)
target_link_libraries(${RRG_TEST_EXECUTABLE} PRIVATE rrg)
target_include_directories(${RRG_TEST_EXECUTABLE} PRIVATE include)
target_compile_definitions(${RRG_TEST_EXECUTABLE} PRIVATE RRG_DEBUG)
