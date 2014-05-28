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

#ifndef NDEBUG_H
#define NDEBUG_H

#include <string>

class nDebug;

class nDebug
{
public:
  nDebug();
  virtual ~nDebug();
  nDebug &operator<<(std::string s);
  nDebug &operator<<(int number);
  nDebug &operator<<(float fnumber);
  
private:
  bool m_env_flag;
};

#endif