/*
 * netsiege - A multiplayer point and click adventure
 * Copyright (C) 2014 Gregor Vollmer <gregor@celement.de>
 * Copyright (C) 2014 Alexander Kraus <alexander.kraus@student.kit.edu>
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

namespace ndebug {

extern bool g_disableDebugOutput;
extern bool g_coloredOutput;
extern int g_debugVerbosity;

}

#ifndef PERMANENTLY_DISABLE_DEBUG_OUTPUT
 #define nDebugL(x) if(ndebug::g_disableDebugOutput) {} \
else if(x <= ndebug::g_debugVerbosity)\
std::cerr << (ndebug::g_coloredOutput?"\x1b[34;1m":"") << __FILE__ << ":" << __LINE__ << ": " << (ndebug::g_coloredOutput?"\x1b[0m":"")
#else
 #define nDebugL(x) if(1) {} \
else \
std::cerr
#endif

#define nDebug nDebugL(1)
#define nDebugVerbose nDebugL(2)

std::ostream& logInfo();
std::ostream& logWarning();
std::ostream& logError();

#endif//NETSIEGE_NDEBUG_H
