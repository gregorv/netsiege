
find_package(Boost REQUIRED)

include_directories(${CMAKE_BINARY_DIR})

add_custom_command(OUTPUT network.pb.cc network.pb.h PRE_BUILD
                   DEPENDS network.proto
                   COMMAND ${PROTOBUF_COMPILER} ${CMAKE_SOURCE_DIR}/src/network/network.proto --cpp_out=${CMAKE_BINARY_DIR} -I${CMAKE_SOURCE_DIR}
                   )
add_library(network clientsession.cpp network.pb.cc networkserver.cpp networkclient.cpp network.cpp rpcdispatcher.cpp rpcpackage.cpp)
add_subdirectory(tests)
target_link_libraries(network oms)
