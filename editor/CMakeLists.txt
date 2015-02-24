# netsiege - A multiplayer point and click adventure
# Copyright (C) 2015 Gregor Vollmer <gregor@celement.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


cmake_minimum_required(VERSION 3.0)
project(nedit) 


find_package(Qt5 REQUIRED Core Gui Widgets OpenGL X11Extras)
find_package(Boost REQUIRED system filesystem)
# find_package(OGRE REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")

# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
include_directories(${Qt5Widget_INCLUDES}
                    ${Qt5OpenGL_INCLUDES}
                    ${CMAKE_BINARY_DIR}
                    ${OGRE_INCLUDE_DIR}
                    ${OGRE_Terrain_INCLUDE_DIR}
                    ${OGRE_Paging_INCLUDE_DIR}
                    ${Boost_INCLUDE_DIR})

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Widgets_DEFINITIONS})

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5EXECUTABLE_COMPILE_FLAGS} -fPIC -Wno-overloaded-virtual")

qt5_wrap_ui(UI_HEADERS ui/main.ui ui/layereditor.ui ui/new.ui ui/sloteditor.ui)


set(nedit_SRCS main.cpp walker.cpp walkercamera.cpp ogrebase.cpp qogrewidget.cpp
editorviewwidget.cpp editorcamera.cpp undocommands.cpp brush.cpp brushdrawer.cpp
terrainlayerlistmodel.cpp editableterrainpagedworldsection.cpp terrainlayer.cpp
campaignmanager.cpp sloteditor.cpp terrainmaterial.cpp)

# qt5_automoc(${nedit_SRCS})
add_executable(nedit ${nedit_SRCS} ${UI_HEADERS})
target_link_libraries(nedit ${Qt5Widget_LIBRARIES}
                            ${Qt5OpenGL_LIBRARIES}
                            ${OGRE_LIBRARIES}
                            ${OGRE_Terrain_LIBRARIES}
                            ${OGRE_Paging_LIBRARIES}
                            ${Boost_LIBRARIES}
                            ${Qt5X11Extras_LIBRARIES}
                            tinyxml)
set_target_properties(nedit PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set_target_properties(nedit PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
# cotire(nedit)
install(TARGETS nedit RUNTIME DESTINATION bin)
