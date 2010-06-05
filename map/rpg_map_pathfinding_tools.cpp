/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "rpg_map_pathfinding_tools.h"

#include "rpg_map_common_tools.h"

#include <ace/Log_Msg.h>

const bool
RPG_Map_Pathfinding_Tools::findPath(const unsigned long& dimensionX_in,
                                    const unsigned long& dimensionY_in,
                                    const RPG_Map_Positions_t& obstacles_in, // walls
                                    const RPG_Map_Position_t& start_in, // start position
                                    const RPG_Map_Position_t& end_in, // end position
                                    RPG_Map_Path_t& path_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Pathfinding_Tools::findPath"));

  // init return value(s)
  path_out.clear();

  RPG_Map_AStar_ClosedPath_t closedPath;
  RPG_Map_AStar_OpenPath_t openPath;
  RPG_Map_AStar_NodesIterator_t position;

  RPG_Map_Position_t current = start_in;
  openPath.insert(std::make_pair(current, 0));

  RPG_Map_AStar_Node_t up, right, down, left;
  RPG_Map_AStar_NodeList_t neighbours;
  bool already_visited = false;
  while (!openPath.empty() &&
         (closedPath.back().first != end_in))
  {
    // get cheapest node
    current = (*openPath.begin()).first;
    openPath.erase(openPath.begin());

    // compute neighbours
    neighbours.clear();
    up.first = current; up.first.second -= ((up.first.second == 0) ? 0 : 1);
    right.first = current; right.first.first += ((right.first.first == (dimensionX_in - 1)) ? 0 : 1);
    down.first = current; down.first.second += ((down.first.second == (dimensionY_in - 1)) ? 0 : 1);
    left.first = current; left.first.first -= ((left.first.first == 0) ? 0 : 1);
    // ...and costs
    up.second = RPG_Map_Common_Tools::dist2Positions(up.first, end_in);
    right.second = RPG_Map_Common_Tools::dist2Positions(right.first, end_in);
    down.second = RPG_Map_Common_Tools::dist2Positions(down.first, end_in);
    left.second = RPG_Map_Common_Tools::dist2Positions(left.first, end_in);
    // reachable ?
    if (obstacles_in.find(up.first) == obstacles_in.end())
      neighbours.push_back(up);
    if (obstacles_in.find(right.first) == obstacles_in.end())
      neighbours.push_back(right);
    if (obstacles_in.find(down.first) == obstacles_in.end())
      neighbours.push_back(down);
    if (obstacles_in.find(left.first) == obstacles_in.end())
      neighbours.push_back(left);

    for (RPG_Map_AStar_NodeListConstIterator_t iterator = neighbours.begin();
         iterator != neighbours.end();
         iterator++)
    {
      // already in closed path ? --> discard
      already_visited = false;
      for (RPG_Map_AStar_NodeListConstIterator_t iterator2 = closedPath.begin();
           iterator2 != closedPath.end();
           iterator2++)
        if ((*iterator).first == (*iterator2).first)
        {
          already_visited = true;
          break;
        } // end IF
      if (already_visited)
        continue;

      // already in open path ? --> update if necessary
      position = openPath.find(*iterator);
      if (position != openPath.end())
      {
        if ((*position).second < (*iterator).second)
        {
          // *TODO*: can this be done without a cast ?
          ACE_const_cast(unsigned long&, (*position).second) = (*iterator).second;
        } // end IF
      } // end IF
    } // end FOR

    // add selected position to closed path
    closedPath.push_back(std::make_pair(current, 0));
  } // end WHILE

  // compute trail
  unsigned long index = 0;
  RPG_Map_Direction_t direction = INVALID;
  RPG_Map_Position_t last_position = (*(closedPath.begin())).first;
  RPG_Map_AStar_NodeListConstIterator_t iterator = closedPath.begin();
  iterator++;
  for (;
       iterator != closedPath.end();
       iterator++, index++)
  {
    // compute direction
    if ((*iterator).first.second == last_position.second)
      if ((*iterator).first.first == (last_position.first + 1))
        direction = RIGHT;
      else
        direction = LEFT;
    else
      if ((*iterator).first.second == (last_position.second + 1))
        direction = DOWN;
      else
        direction = UP;

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("step[#%u]: (%u,%u - @%u) --> %u\n"),
               index,
               (*iterator).first.first,
               (*iterator).first.second,
               (*iterator).second,
               RPG_Map_Common_Tools::direction2String(direction).c_str()));

    path_out.push_back(direction);

    last_position = (*iterator).first;
  } // end FOR

  return true;
}
