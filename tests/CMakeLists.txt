
include_directories(${CMAKE_SOURCE_DIR}/bandit)
add_library(bandit_main main.cpp)

add_executable(rint_tests rint_ut.cpp)
target_link_libraries(rint_tests bandit_main)

add_test(NAME rint COMMAND rint_tests)
