

add_custom_command(OUTPUT script.pb.cc script.pb.h PRE_BUILD
                   DEPENDS script.proto
                   COMMAND ${PROTOBUF_COMPILER} ${CMAKE_SOURCE_DIR}/src/script/script.proto --cpp_out=${CMAKE_BINARY_DIR} -I${CMAKE_SOURCE_DIR}
                   )

set(SCRIPT_SRC
	scriptengine.cpp
	scriptfile.cpp
	scriptfilemanager.cpp
	script.pb.cc
	serializer.cpp
)
add_library(script STATIC ${SCRIPT_SRC})

