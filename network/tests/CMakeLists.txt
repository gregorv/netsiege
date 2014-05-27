
if(BUILD_TESTS)
  set(CMAKE_AUTOMOC ON)
  add_executable(netsynctest netsynctest.cpp)
  include_directories(${Qt5Core_INCLUDES} ${Qt5Test_INCLUDES})
  target_link_libraries(netsynctest oms network  ${PROTOBUF_LIBRARIES} boost_system)
  qt5_use_modules(netsynctest Core Test)
  add_test(NAME netsynctest COMMAND netsynctest)
endif(BUILD_TESTS)