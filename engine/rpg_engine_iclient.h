/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RPG_ENGINE_ICLIENT_H
#define RPG_ENGINE_ICLIENT_H

#include "rpg_engine_common.h"

// engine --> UI client interface
class RPG_Engine_IClient
{
 public:
  // exposed interface
  virtual void notify (enum RPG_Engine_Command,
                       const struct RPG_Engine_ClientNotificationParameters&,
                       bool = true) = 0; // lock (engine) ?
};

#endif // RPG_ENGINE_ICLIENT_H
