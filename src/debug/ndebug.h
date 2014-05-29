/*
 * netsiege - A multiplayer point and click adventure
 * Copyright (C) 2014 Gregor Vollmer <gregor@celement.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NETSIEGE_NDEBUG_H
#define NETSIEGE_NDEBUG_H

#include "config.h"
#include <iostream>

extern bool g_disableDebugOutput;

#ifndef PERMANENTLY_DISABLE_DEBUG_OUTPUT
 #define nDebug if(g_disableDebugOutput) {} \
else \
std::cerr << __FILE__ << ":" << __LINE__ << ": "
#else
 #define nDebug if(1) {} \
else \
std::cerr
#endif

#endif//NETSIEGE_NDEBUG_H
