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
                                    const RPG_Map_Direction& startDirection_in, // initial direction
                                    const RPG_Map_Position_t& end_in, // end position
                                    RPG_Map_Path_t& path_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Pathfinding_Tools::findPath"));

  // init return value(s)
  path_out.clear();

  RPG_Map_AStar_ClosedPath_t closedPath;
  RPG_Map_AStar_OpenPath_t openPath;
  RPG_Map_AStar_NodesIterator_t position;

  RPG_Map_AStar_Position_t start;
  start.position = start_in;
  start.last_position = start_in;
  openPath.insert(std::make_pair(start,
                                 RPG_Map_Common_Tools::dist2Positions(start_in, end_in)));

  RPG_Map_AStar_Node_t up, right, down, left;
  RPG_Map_AStar_NodeList_t neighbours;
  bool already_visited = false;
  while (!openPath.empty())
  {
    // *NOTE*: use front (--> "cheapest") node first
    // add current position to the closed path
    closedPath.push_back(*openPath.begin());
    // finished ?
    if (closedPath.back().first.position == end_in)
      break;

    // compute neighbours
    neighbours.clear();
    up.first = (*openPath.begin()).first;
    up.first.last_position = (*openPath.begin()).first.position;
    up.first.position.second -= ((up.first.position.second == 0) ? 0 : 1);
    right.first = (*openPath.begin()).first;
    right.first.last_position = (*openPath.begin()).first.position;
    right.first.position.first += ((right.first.position.first == (dimensionX_in - 1)) ? 0 : 1);
    down.first = (*openPath.begin()).first;
    down.first.last_position = (*openPath.begin()).first.position;
    down.first.position.second += ((down.first.position.second == (dimensionY_in - 1)) ? 0 : 1);
    left.first = (*openPath.begin()).first;
    left.first.last_position = (*openPath.begin()).first.position;
    left.first.position.first -= ((left.first.position.first == 0) ? 0 : 1);
    // ...and costs
    up.second = RPG_Map_Common_Tools::dist2Positions(up.first.position, end_in);
    right.second = RPG_Map_Common_Tools::dist2Positions(right.first.position, end_in);
    down.second = RPG_Map_Common_Tools::dist2Positions(down.first.position, end_in);
    left.second = RPG_Map_Common_Tools::dist2Positions(left.first.position, end_in);
    // *NOTE*: enforce the constraint of an "initial" starting direction
    // --> "leave" a room to find another door
    // start position ?
    if ((closedPath.back().first.position == start_in) &&
        (startDirection_in != INVALID))
    {
      switch (startDirection_in)
      {
        case UP:
        {
          neighbours.push_back(up);
          break;
        }
        case RIGHT:
        {
          neighbours.push_back(right);
          break;
        }
        case DOWN:
        {
          neighbours.push_back(down);
          break;
        }
        case LEFT:
        {
          neighbours.push_back(left);
          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid direction (was \"%s\", %u), continuing\n"),
                     RPG_Map_Common_Tools::direction2String(startDirection_in).c_str(),
                     startDirection_in));

          break;
        }
      } // end SWITCH
    } // end IF
    else
    {
      // reachable ?
      if ((obstacles_in.find(up.first.position) == obstacles_in.end()) ||
          (up.first.position == end_in))
        neighbours.push_back(up);
      if ((obstacles_in.find(right.first.position) == obstacles_in.end()) ||
          (right.first.position == end_in))
        neighbours.push_back(right);
      if ((obstacles_in.find(down.first.position) == obstacles_in.end()) ||
          (down.first.position == end_in))
        neighbours.push_back(down);
      if ((obstacles_in.find(left.first.position) == obstacles_in.end()) ||
          (left.first.position == end_in))
        neighbours.push_back(left);
    } // end ELSE

    for (RPG_Map_AStar_NodeListConstIterator_t iterator = neighbours.begin();
         iterator != neighbours.end();
         iterator++)
    {
      // already in closed path ? --> discard
      already_visited = false;
      for (RPG_Map_AStar_NodeListConstIterator_t iterator2 = closedPath.begin();
           iterator2 != closedPath.end();
           iterator2++)
        if ((*iterator).first.position == (*iterator2).first.position)
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
        if ((*iterator).second < (*position).second)
        {
          // *TODO*: can this be done without a cast ?
          ACE_const_cast(unsigned long&, (*position).second) = (*iterator).second;
        } // end IF
      } // end IF
      else
      {
        // insert node into the list of potential next steps
        openPath.insert(*iterator);
      } // end ELSE
    } // end FOR

    // finished with this node
    openPath.erase(closedPath.back());

//     // debug info
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("open path[%u node(s)]:\n"),
//                openPath.size()));
//     unsigned long index = 0;
//     for (RPG_Map_AStar_NodesConstIterator_t iterator = openPath.begin();
//          iterator != openPath.end();
//          iterator++, index++)
//     {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("node[#%u]: (%u,%u), last(%u,%u), cost:%u\n"),
//                  index,
//                  (*iterator).first.position.first,
//                  (*iterator).first.position.second,
//                  (*iterator).first.last_position.first,
//                  (*iterator).first.last_position.second,
//                  (*iterator).second));
//     } // end FOR
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("closed path[%u node(s)]:\n"),
//                closedPath.size()));
//     index = 0;
//     for (RPG_Map_AStar_NodeListConstIterator_t iterator = closedPath.begin();
//          iterator != closedPath.end();
//          iterator++, index++)
//     {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("node[#%u]: (%u,%u), last(%u,%u), cost:%u\n"),
//                  index,
//                  (*iterator).first.position.first,
//                  (*iterator).first.position.second,
//                  (*iterator).first.last_position.first,
//                  (*iterator).first.last_position.second,
//                  (*iterator).second));
//     } // end FOR
  } // end WHILE

  // found a solution ?
  if (closedPath.back().first.position != end_in)
    return false;

  // compute trail
  unsigned long index = 0;
  RPG_Map_Direction direction = INVALID;
  // start at the end
  RPG_Map_AStar_NodeListConstIterator_t current_node = closedPath.end();
  current_node--;
  ACE_ASSERT((*current_node).first.position == end_in);
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("step[#%u]: (%u,%u - @%u)\n"),
//              index,
//              (*current_node).first.position.first,
//              (*current_node).first.position.second,
//              (*current_node).second));
  RPG_Map_AStar_NodeListConstIterator_t previous_node = closedPath.begin();
  path_out.push_front(std::make_pair((*current_node).first.position, INVALID));
  for (;
       (*current_node).first.position != start_in;
       index++)
  {
    // find previous node
    for (previous_node = closedPath.begin();
         previous_node != closedPath.end();
         previous_node++)
      if ((*previous_node).first.position == (*current_node).first.last_position)
        break;
    ACE_ASSERT(previous_node != closedPath.end());

    // compute direction
    if ((*current_node).first.position.second == (*previous_node).first.position.second)
      if ((*current_node).first.position.first == ((*previous_node).first.position.first + 1))
        direction = RIGHT;
      else
        direction = LEFT;
    else
      if ((*current_node).first.position.second == ((*previous_node).first.position.second + 1))
        direction = DOWN;
      else
        direction = UP;

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("step[#%u]: (%u,%u - @%u) --> %s\n"),
//                index,
//                (*previous_node).first.position.first,
//                (*previous_node).first.position.second,
//                (*previous_node).second,
//                RPG_Map_Common_Tools::direction2String(direction).c_str()));

    path_out.push_front(std::make_pair((*previous_node).first.position, direction));

    current_node = previous_node;
  } // end FOR
  path_out.push_front(std::make_pair((*current_node).first.position, direction));

  return true;
}
