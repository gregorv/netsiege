
set(CAMPAIGN_SRC
	serverlogic.cpp
	manager.cpp
)
include_directories(${ANGELSCRIPT_INCLUDE_DIRS})
add_library(campaign STATIC ${CAMPAIGN_SRC})

