
set(tinyxml_SRC tinystr.cpp tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTIXML_USE_STL")

add_library(tinyxml ${tinyxml_SRC})
