
set(CAMPAIGN_SRC
	scriptengine.cpp
	serverlogic.cpp
	scriptfile.cpp
	scriptfilemanager.cpp
	manager.cpp
)
include_directories(${ANGELSCRIPT_INCLUDE_DIRS})
add_library(campaign STATIC ${CAMPAIGN_SRC})

