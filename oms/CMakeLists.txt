
set(oms_SRC objectmanager.cpp
            gameobject.cpp
            objects.pb.cc
   )

pkg_search_module(PROTOBUF REQUIRED protobuf)

include_directories(${PROTOBUF_INCLUDE_DIRS} ${OGRE_INCLUDE_DIRS})

add_library(oms STATIC ${oms_SRC})
target_link_libraries(oms ${PROTOBUF_LIBRARIES})

add_subdirectory(tests)