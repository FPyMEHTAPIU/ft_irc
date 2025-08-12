cmake_minimum_required(VERSION 3.14)
project(ft_irc)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

# Username test executable
add_executable(
  username_test
  tests/username_test.cc
  ../src/utils/validateClientName.cpp
)
target_link_libraries(
  username_test 
  GTest::gtest_main
)
gtest_discover_tests(username_test)

# Channel name test executable
add_executable(
  channelname_test
  tests/channelname_test.cc
  ../src/utils/validateChannelName.cpp
)
target_link_libraries(
  channelname_test 
  GTest::gtest_main
)
gtest_discover_tests(channelname_test)

# Password test executable
add_executable(
  password_test
  tests/password_test.cc
  ../src/parser/parser.cpp
)
target_link_libraries(
  password_test 
  GTest::gtest_main
)
gtest_discover_tests(password_test)

# Port test executable
add_executable(
  port_test
  tests/port_test.cc
  ../src/parser/parser.cpp
)
target_link_libraries(
  port_test 
  GTest::gtest_main
)
gtest_discover_tests(port_test)