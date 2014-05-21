
if(BUILD_TESTS)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

  add_executable(netsynctest netsynctest.cpp)
  target_link_libraries(netsynctest oms network)
  qt5_use_modules(netsynctest Core Test)
  add_test(NAME netsynctest COMMAND netsynctest)
endif(BUILD_TESTS)