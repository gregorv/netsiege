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

#include "ndebug.h"

#include <string>
#include <cstdlib>

bool ndebug::g_disableDebugOutput = true;
int ndebug::g_debugVerbosity = 1;
bool ndebug::g_coloredOutput = true;

#ifndef PERMANENTLY_DISABLE_DEBUG_OUTPUT
class ConfigureDebug {
public:
    ConfigureDebug() {
#ifdef DEBUG
        ndebug::g_disableDebugOutput = false;
#endif
        auto debugVar = std::getenv("DEBUG");
        if(debugVar) {
            auto verbosity = std::strtol(debugVar, 0, 10);
            ndebug::g_disableDebugOutput = verbosity == 0;
            ndebug::g_debugVerbosity = verbosity;
        }

        auto colorVar = std::getenv("DEBUG_COLOR");
        if(colorVar) {
            auto color = std::strtol(colorVar, 0, 10);
            ndebug::g_coloredOutput = color != 0;
        }
    }
};
ConfigureDebug debugCfg;


std::ostream& logInfo()
{
    return std::cout << "\x1b[37;1mINFO: \x1b[0m";
}

std::ostream& logError()
{
    return std::cerr << "\x1b[31;1mERR: \x1b[0m";
}

std::ostream& logWarning()
{
    return std::cerr << "\x1b[33;1mWARN: \x1b[0m";
}


#endif//ndef PERMANENTLY_DISABLE_DEBUG_OUTPUT
