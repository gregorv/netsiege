
if(BUILD_TESTS)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

  add_executable(omstest omstest.cpp)
  target_link_libraries(omstest oms)
  qt5_use_modules(omstest Core Test)

  add_test(NAME omstest COMMAND omstest)
endif(BUILD_TESTS)