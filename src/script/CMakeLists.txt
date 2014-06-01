
set(SCRIPT_SRC
	scriptengine.cpp
	scriptfile.cpp
	scriptfilemanager.cpp
)
include_directories(${ANGELSCRIPT_INCLUDE_DIRS})
add_library(script STATIC ${SCRIPT_SRC})

