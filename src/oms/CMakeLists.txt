
set(oms_SRC objectmanager.cpp
            gameobject.cpp
            objects.pb.cc
   )

add_custom_command(OUTPUT objects.pb.cc objects.pb.h PRE_BUILD
                   DEPENDS objects.proto ${CMAKE_BINARY_DIR}/src/script/script.pb.h
                   COMMAND ${PROTOBUF_COMPILER} ${CMAKE_SOURCE_DIR}/src/oms/objects.proto --cpp_out=${CMAKE_BINARY_DIR} -I${CMAKE_SOURCE_DIR}
                  )

add_library(oms STATIC ${oms_SRC})
target_link_libraries(oms ${PROTOBUF_LIBRARIES} script)

add_subdirectory(tests)
