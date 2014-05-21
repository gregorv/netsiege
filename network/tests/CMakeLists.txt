
if(BUILD_TESTS)
  include(${QT_USE_FILE})
  QT4_WRAP_CPP(netsynctest_MOC netsynctest.h)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

  add_executable(netsynctest netsynctest.cpp ${netsynctest_MOC})
  target_link_libraries(netsynctest oms network ${QT_LIBRARIES})
  add_test(NAME netsynctest COMMAND netsynctest)
endif(BUILD_TESTS)