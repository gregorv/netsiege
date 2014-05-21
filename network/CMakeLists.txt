
find_package(Boost REQUIRED)

include_directories(${CMAKE_BINARY_DIR})

add_custom_command(OUTPUT network.pb.cc PRE_BUILD
                   DEPENDS network.proto
                   COMMAND ${PROTOBUF_COMPILER} ${CMAKE_SOURCE_DIR}/network/network.proto --cpp_out=${CMAKE_BINARY_DIR} -I${CMAKE_SOURCE_DIR}
                   )
add_library(network networkserver.cpp networkclient.cpp clientsession.cpp network.pb.cc)
add_subdirectory(tests)