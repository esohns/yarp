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
#include "stdafx.h"

#include "rpg_map_pathfinding_tools.h"

#include <iostream>

#include "ace/Log_Msg.h"

#include "rpg_common_macros.h"

#include "rpg_map_common_tools.h"

void
RPG_Map_Pathfinding_Tools::findPath (const RPG_Map_Size_t& size_in,
                                     const RPG_Map_Positions_t& obstacles_in,  // walls / closed doors
                                     const RPG_Map_Position_t& start_in,       // start position
                                     enum RPG_Map_Direction startDirection_in, // initial direction
                                     const RPG_Map_Position_t& end_in,         // end position
                                     RPG_Map_Path_t& path_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Map_Pathfinding_Tools::findPath"));

  // initialize return value(s)
  path_out.clear ();

  // sanity check
  if (start_in == end_in)
    return; // nothing to do...

  RPG_Map_AStar_ClosedPath_t closedPath;
  RPG_Map_AStar_OpenPath_t openPath;
  RPG_Map_AStar_NodesIterator_t position;

  struct RPG_Map_AStar_Position start;
  start.position = start_in;
  start.last_position = start_in;
  openPath.insert (std::make_pair (start, RPG_Map_Common_Tools::distance (start_in, end_in)));

  RPG_Map_AStar_Node_t up, right, down, left;
  RPG_Map_AStar_NodeList_t neighbours;
  bool already_visited = false;
  while (!openPath.empty ())
  {
    // *NOTE*: use front (--> "cheapest") node first
    // add current position to the closed path
    closedPath.push_back (*openPath.begin ());

    // finished ?
    if (closedPath.back ().first.position == end_in)
      break;

    // compute neighbours
    neighbours.clear ();
    up.first = (*openPath.begin ()).first;
    up.first.last_position = (*openPath.begin ()).first.position;
    up.first.position.second -= ((up.first.position.second == 0) ? 0 : 1);
    right.first = (*openPath.begin ()).first;
    right.first.last_position = (*openPath.begin ()).first.position;
    right.first.position.first += ((right.first.position.first == (size_in.first - 1)) ? 0 : 1);
    down.first = (*openPath.begin ()).first;
    down.first.last_position = (*openPath.begin ()).first.position;
    down.first.position.second += ((down.first.position.second == (size_in.second - 1)) ? 0 : 1);
    left.first = (*openPath.begin ()).first;
    left.first.last_position = (*openPath.begin ()).first.position;
    left.first.position.first -= ((left.first.position.first == 0) ? 0 : 1);
    // ...and costs
    up.second = RPG_Map_Common_Tools::distance (up.first.position, end_in);
    right.second = RPG_Map_Common_Tools::distance (right.first.position, end_in);
    down.second = RPG_Map_Common_Tools::distance (down.first.position, end_in);
    left.second = RPG_Map_Common_Tools::distance (left.first.position, end_in);
    // *NOTE*: enforce the constraint of an "initial" starting direction
    // --> "leave" a room to find another door
    // start position ?
    if ((closedPath.back ().first.position == start_in) &&
        (startDirection_in != RPG_MAP_DIRECTION_INVALID))
    {
      switch (startDirection_in)
      {
        case DIRECTION_UP:
          neighbours.push_back (up); break;
        case DIRECTION_RIGHT:
          neighbours.push_back (right); break;
        case DIRECTION_DOWN:
          neighbours.push_back (down); break;
        case DIRECTION_LEFT:
          neighbours.push_back (left); break;
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid direction (was \"%s\"), continuing\n"),
                      ACE_TEXT (RPG_Map_DirectionHelper::RPG_Map_DirectionToString (startDirection_in).c_str ())));
          ACE_ASSERT (false);
          break;
        }
      } // end SWITCH
    } // end IF
    else
    {
      // reachable ?
      if ((obstacles_in.find (up.first.position) == obstacles_in.end ()) ||
          (up.first.position == end_in))
        neighbours.push_back (up);
      if ((obstacles_in.find (right.first.position) == obstacles_in.end ()) ||
          (right.first.position == end_in))
        neighbours.push_back (right);
      if ((obstacles_in.find (down.first.position) == obstacles_in.end ()) ||
          (down.first.position == end_in))
        neighbours.push_back (down);
      if ((obstacles_in.find (left.first.position) == obstacles_in.end ()) ||
          (left.first.position == end_in))
        neighbours.push_back (left);
    } // end ELSE

    for (RPG_Map_AStar_NodeListConstIterator_t iterator = neighbours.begin ();
         iterator != neighbours.end ();
         iterator++)
    {
      // already in closed path ? --> discard
      already_visited = false;
      for (RPG_Map_AStar_NodeListConstIterator_t iterator2 = closedPath.begin ();
           iterator2 != closedPath.end ();
           iterator2++)
        if ((*iterator).first.position == (*iterator2).first.position)
        {
          already_visited = true;
          break;
        } // end IF
      if (already_visited)
        continue;

      // already in open path ? --> update if necessary
      position = openPath.find (*iterator);
      if (position != openPath.end ())
      {
        if ((*iterator).second < (*position).second)
        {
          // *TODO*: can this be done without a cast ?
          const_cast<unsigned int&> ((*position).second) = (*iterator).second;
        } // end IF
      } // end IF
      else
      {
        // insert node into the list of potential next steps
        openPath.insert (*iterator);
      } // end ELSE
    } // end FOR

    // finished with this node
    openPath.erase (closedPath.back ());

//     // debug info
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("open path[%u node(s)]:\n"),
//                openPath.size()));
//     unsigned int index = 0;
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

  // compute (partial) trail
  if (closedPath.size () == 1)
  { ACE_ASSERT (closedPath.front ().first.position == start_in);

    // --> must be locked in by obstacles, no trail

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("could not find a path [%u,%u] --> [%u,%u], returning\n"),
                start_in.first,
                start_in.second,
                end_in.first,
                end_in.second));
    return;
  } // end IF

  unsigned int index = 0;
  RPG_Map_Direction direction = RPG_MAP_DIRECTION_INVALID;
  // start at the end
  RPG_Map_AStar_NodeListConstIterator_t current_node = closedPath.end ();
  current_node--;

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("step[#%u]: (%u,%u - @%u)\n"),
//              index,
//              (*current_node).first.position.first,
//              (*current_node).first.position.second,
//              (*current_node).second));
  RPG_Map_AStar_NodeListConstIterator_t previous_node = closedPath.begin ();
  path_out.push_front (std::make_pair ((*current_node).first.position,
                                       RPG_MAP_DIRECTION_INVALID));
  for (;
       (*current_node).first.position != start_in;
       index++)
  {
    // find previous node
    for (previous_node = closedPath.begin ();
         previous_node != closedPath.end ();
         previous_node++)
      if ((*previous_node).first.position ==
          (*current_node).first.last_position)
        break;
    ACE_ASSERT (previous_node != closedPath.end ());

    // compute direction
    if ((*current_node).first.position.second ==
        (*previous_node).first.position.second)
      if ((*current_node).first.position.first ==
          ((*previous_node).first.position.first + 1))
        direction = DIRECTION_RIGHT;
      else
        direction = DIRECTION_LEFT;
    else
      if ((*current_node).first.position.second ==
          ((*previous_node).first.position.second + 1))
        direction = DIRECTION_DOWN;
      else
        direction = DIRECTION_UP;

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("step[#%u]: (%u,%u - @%u) --> %s\n"),
//                index,
//                (*previous_node).first.position.first,
//                (*previous_node).first.position.second,
//                (*previous_node).second,
//                RPG_Map_Common_Tools::direction2String(direction).c_str()));

    path_out.push_front (std::make_pair ((*previous_node).first.position,
                                         direction));

    current_node = previous_node;
  } // end FOR
//   path_out.push_front(std::make_pair((*current_node).first.position, direction));

  //// debug info
  //if (closedPath.back().first.position != end_in)
  //  ACE_DEBUG((LM_DEBUG,
  //             ACE_TEXT("could not find a path [%u,%u] --> [%u,%u], continuing\n"),
  //             start_in.first,
  //             start_in.second,
  //             end_in.first,
  //             end_in.second));
}

void
RPG_Map_Pathfinding_Tools::findPath (const RPG_Map_Position_t& start_in,
                                     const RPG_Map_Position_t& end_in,
                                     RPG_Map_PositionList_t& path_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Map_Pathfinding_Tools::findPath"));

  // initialize return value(s)
  path_out.clear ();

  int dx    = std::abs (static_cast<int> (end_in.first)  -
                        static_cast<int> (start_in.first));
  int dy    = std::abs (static_cast<int> (end_in.second) -
                        static_cast<int> (start_in.second));
  int x     = start_in.first;
  int y     = start_in.second;
  int n     = 1 + dx + dy;
  int x_inc = (end_in.first  > start_in.first)  ? 1 : -1;
  int y_inc = (end_in.second > start_in.second) ? 1 : -1;
  int error = dx - dy;
  dx       *= 2;
  dy       *= 2;

  for (;
       n > 0;
       --n)
  {
    path_out.push_back (std::make_pair (x, y));

    if (error > 0)
    {
      x     += x_inc;
      error -= dy;
    } // end IF
    else
    {
      y     += y_inc;
      error += dx;
    } // end ELSE
  } // end FOR
}

enum RPG_Map_Direction
RPG_Map_Pathfinding_Tools::getDirection (const RPG_Map_Position_t& startPosition_in,
                                         const RPG_Map_Position_t& endPosition_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Map_Pathfinding_Tools::getDirection"));

  // sanity check
  if (startPosition_in == endPosition_in)
    return RPG_MAP_DIRECTION_INVALID;

  int distance_x =
    std::abs (static_cast<int> (startPosition_in.first - endPosition_in.first));
  int distance_y =
    std::abs (static_cast<int> (startPosition_in.second - endPosition_in.second));

  if (startPosition_in.first > endPosition_in.first)
  {
    if (startPosition_in.second > endPosition_in.second)
      return (distance_x >= distance_y ? DIRECTION_LEFT : DIRECTION_UP);

    if (startPosition_in.second == endPosition_in.second)
      return DIRECTION_LEFT;

    return (distance_x >= distance_y ? DIRECTION_LEFT : DIRECTION_DOWN);
  } // end IF

  if (startPosition_in.first == endPosition_in.first)
    return (startPosition_in.second > endPosition_in.second ? DIRECTION_UP
                                                            : DIRECTION_DOWN);

  if (startPosition_in.second > endPosition_in.second)
    return (distance_x >= distance_y ? DIRECTION_RIGHT : DIRECTION_UP);

  if (startPosition_in.second == endPosition_in.second)
    return DIRECTION_RIGHT;

  return (distance_x >= distance_y ? DIRECTION_RIGHT : DIRECTION_DOWN);
}

void
RPG_Map_Pathfinding_Tools::print (const RPG_Map_Path_t& path_in,
                                  const struct RPG_Map_FloorPlan& plan_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Map_Pathfinding_Tools::print"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("path\n[%u,%u] --> [%u,%u]: %u steps\n"),
              path_in.front ().first.first, path_in.front ().first.second,
              path_in.back ().first.first, path_in.back ().first.second,
              path_in.size () - 1));

  RPG_Map_PathConstIterator_t path_iterator;
  RPG_Map_Position_t current_position;
  struct RPG_Map_Door current_position_door;
  bool done = false;
  for (unsigned int y = 0;
       y < plan_in.size_y;
       y++)
  {
    for (unsigned int x = 0;
         x < plan_in.size_x;
         x++)
    {
      current_position = std::make_pair (x, y);
      current_position_door.position = current_position;
      done = false;

      // part of the path ?
      for (path_iterator = path_in.begin ();
           path_iterator != path_in.end ();
           path_iterator++)
        if ((*path_iterator).first == current_position)
        {
          if ((*path_iterator).first == path_in.front ().first)
            std::clog << 'A'; // start position
          else if ((*path_iterator).first == path_in.back ().first)
            std::clog << 'B'; // end position
          else
            std::clog << 'x'; // path step

          done = true;

          break;
        } // end IF
      if (done)
        continue;

      // unmapped, floor, wall, or door ?
      if (plan_in.unmapped.find (current_position) != plan_in.unmapped.end ())
        std::clog << ' '; // unmapped
      else if (plan_in.walls.find (current_position) != plan_in.walls.end ())
        std::clog << '#'; // wall
      else if (plan_in.doors.find (current_position_door) != plan_in.doors.end ())
        std::clog << '='; // door
      else
        std::clog << '.'; // floor
    } // end FOR
    std::clog << std::endl;
  } // end FOR
}
