
set(oms_SRC objectmanager.cpp
            gameobject.cpp
            objects.pb.cc
   )

pkg_search_module(PROTOBUF REQUIRED protobuf)

add_custom_command(OUTPUT objects.pb.cc objects.pb.h PRE_BUILD
                   DEPENDS objects.proto
                   COMMAND ${PROTOBUF_COMPILER} ${CMAKE_SOURCE_DIR}/oms/objects.proto --cpp_out=${CMAKE_BINARY_DIR} -I${CMAKE_SOURCE_DIR}
                  )

include_directories(${PROTOBUF_INCLUDE_DIRS} ${OGRE_INCLUDE_DIRS} ${CMAKE_BINARY_DIR})

add_library(oms STATIC ${oms_SRC})
target_link_libraries(oms ${PROTOBUF_LIBRARIES})

add_subdirectory(tests)