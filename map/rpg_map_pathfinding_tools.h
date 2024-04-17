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

#ifndef RPG_MAP_PATHFINDING_TOOLS_H
#define RPG_MAP_PATHFINDING_TOOLS_H

#include <list>
#include <set>

#include "ace/Global_Macros.h"

#include "rpg_map_common.h"

class RPG_Map_Pathfinding_Tools
{
 public:
  // "classic" A* algorithm
  static void findPath (const RPG_Map_Size_t&,      // dimension x/y
                        const RPG_Map_Positions_t&, // obstacles
                        const RPG_Map_Position_t&,  // start position
                        enum RPG_Map_Direction,     // optional: starting direction
                        const RPG_Map_Position_t&,  // end position
                        RPG_Map_Path_t&);           // return value: path

  // "classic" raytrace algorithm (aka "line-of-sight" / "field-of-view")
  static void findPath (const RPG_Map_Position_t&, // start position
                        const RPG_Map_Position_t&, // end position
                        RPG_Map_PositionList_t&);  // return value: path

  // *NOTE*: returns a ("best-guess") estimate of the relative direction from A to B
  static enum RPG_Map_Direction getDirection (const RPG_Map_Position_t&,  // start position
                                              const RPG_Map_Position_t&); // end position

  static void print (const RPG_Map_Path_t&,            // path
                     const struct RPG_Map_FloorPlan&); // floor plan

  // *NOTE*: these must be public (required by operator==, see below)
  struct RPG_Map_AStar_Position
  {
    RPG_Map_Position_t position;
    RPG_Map_Position_t last_position;

    inline RPG_Map_AStar_Position& operator= (const struct RPG_Map_AStar_Position& rhs_in)
    {
      this->position = rhs_in.position;
      this->last_position = rhs_in.last_position;
      return *this;
    }
    inline bool operator== (const struct RPG_Map_AStar_Position& rhs_in) const
    { return (this->position == rhs_in.position); }
  };
  typedef std::pair<struct RPG_Map_AStar_Position, unsigned int> RPG_Map_AStar_Node_t;

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Map_Pathfinding_Tools ())
  ACE_UNIMPLEMENTED_FUNC (virtual ~RPG_Map_Pathfinding_Tools ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Map_Pathfinding_Tools (const RPG_Map_Pathfinding_Tools&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Map_Pathfinding_Tools& operator= (const RPG_Map_Pathfinding_Tools&))

  typedef std::list<RPG_Map_AStar_Node_t> RPG_Map_AStar_NodeList_t;
  typedef RPG_Map_AStar_NodeList_t::const_iterator RPG_Map_AStar_NodeListConstIterator_t;
  typedef RPG_Map_AStar_NodeList_t RPG_Map_AStar_ClosedPath_t;
  // *NOTE*: std::less<_Key> uses default operator< for std::pair<>, which sorts
  // the trail according to positions, then cost
  // --> don't want this for the Open Paths
  struct node_compare
   : public std::binary_function<RPG_Map_AStar_Node_t,
                                 RPG_Map_AStar_Node_t,
                                 bool>
  {
    inline bool operator() (const RPG_Map_AStar_Node_t& __first,
                            const RPG_Map_AStar_Node_t& __second) const
    {
      return ((__first.second < __second.second) ||
              (!(__second.second < __first.second) && (__first.first.position < __second.first.position)));
    }
  };
  typedef std::set<RPG_Map_AStar_Node_t, node_compare> RPG_Map_AStar_Nodes_t;
  typedef RPG_Map_AStar_Nodes_t::const_iterator RPG_Map_AStar_NodesConstIterator_t;
  typedef RPG_Map_AStar_Nodes_t::iterator RPG_Map_AStar_NodesIterator_t;
  typedef RPG_Map_AStar_Nodes_t RPG_Map_AStar_OpenPath_t;
};

static bool
operator== (const RPG_Map_Pathfinding_Tools::RPG_Map_AStar_Node_t& __x,
            const RPG_Map_Pathfinding_Tools::RPG_Map_AStar_Node_t& __y)
{ return (__x.first == __y.first); };

#endif
