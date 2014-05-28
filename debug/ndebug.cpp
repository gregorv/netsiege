/*
 * netsiege - A multiplayer point and click adventure
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

#include "config.h"
#include <iostream>
#include <string>
#include <stdlib.h>

nDebug::nDebug()
  : m_env_flag(true)
{
  char *p = getenv("NETSIEGE_NO_DEBUG_OUTPUT");
  std::string debug_env_flag;
  if(p==0); 
  else
  {
    debug_env_flag = std::string(p);
    if(debug_env_flag == std::string("TRUE") || debug_env_flag == std::string("1") || debug_env_flag == std::string("true")) 
      m_env_flag = false;
  }
}

nDebug& nDebug::operator<<(float fnumber)
{
#ifndef DISABLE_DEBUG_OUTPUT
  if(m_env_flag) std::cout << fnumber << " ";
#endif
  return *this;
}

nDebug& nDebug::operator<<(std::string s)
{
#ifndef DISABLE_DEBUG_OUTPUT
  if(m_env_flag)
  {
    std::cout << s.c_str();
    if(s.at(s.length()-1) != ' ') std::cout << " ";
  }
#endif
  return *this;
}

nDebug& nDebug::operator<<(int number)
{
#ifndef DISABLE_DEBUG_OUTPUT
  if(m_env_flag) std::cout << number << " ";
#endif
  return *this;
}

nDebug::~nDebug()
{
#ifndef DISABLE_DEBUG_OUTPUT
  if(m_env_flag) std::cout << "\n";
#endif
}
