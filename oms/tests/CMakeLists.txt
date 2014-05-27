
if(BUILD_TESTS)
  set(CMAKE_AUTOMOC ON)
  add_executable(omstest omstest.cpp)
  target_link_libraries(omstest oms)
  qt5_use_modules(omstest Core Test)

  add_test(NAME omstest COMMAND omstest)
endif(BUILD_TESTS)