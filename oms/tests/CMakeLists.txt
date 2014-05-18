
if(BUILD_TESTS)
  include(${QT_USE_FILE})
  QT4_WRAP_CPP(omstest_MOC omstest.h)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

  add_executable(omstest omstest.cpp ${omstest_MOC})
  target_link_libraries(omstest oms ${QT_LIBRARIES})
endif(BUILD_TESTS)