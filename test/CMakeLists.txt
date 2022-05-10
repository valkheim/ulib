file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE test_sources "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")
file(GLOB_RECURSE test_headers "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h")

add_executable(ulib_test ${sources} ${test_sources} ${test_headers})

target_include_directories(ulib_test
PUBLIC test
    PUBLIC ../../ulib/include/public
)

target_link_libraries(
  ulib_test
  ulib
  gtest_main
)
