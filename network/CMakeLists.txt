
find_package(Boost REQUIRED)

add_library(network gameclient.cpp gameserver.cpp)
add_subdirectory(tests)