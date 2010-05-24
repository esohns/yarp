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
#include "rpg_map_common_tools.h"

#include "rpg_map_defines.h"

#include <rpg_dice.h>

#include <ace/Log_Msg.h>

#include <sstream>
#include <iostream>

void
RPG_Map_Common_Tools::createDungeonLevel(const unsigned long& dimensionX_in,
                                         const unsigned long& dimensionY_in,
                                         const unsigned long& numRooms_in,
                                         const bool& wantSquareRooms_in,
                                         const bool& maximizeArea_in,
                                         const unsigned long& minArea_in,
                                         const unsigned long& maxDoorsPerRoom_in,
                                         RPG_Map_DungeonLevel& level_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::createDungeonLevel"));

  // init return value(s)
  level_out.doors.clear();
  level_out.walls.clear();

  // step1: segment area into numRooms_in partition(s)
  RPG_Map_Partition_t partition;
  makePartition(dimensionX_in,
                dimensionY_in,
                numRooms_in,
                partition);

  // step2: form rooms within partition(s)
  RPG_Map_ZoneList_t rooms;
  makeRooms(partition,
            wantSquareRooms_in,
            maximizeArea_in,
            minArea_in,
            rooms);
  // debug info
  displayRooms(dimensionX_in,
               dimensionY_in,
               rooms);

  // step3: connect rooms to form the level
  connectRooms(rooms,
               maxDoorsPerRoom_in,
               level_out);
  // debug info
  displayDungeonLevel(dimensionX_in,
                      dimensionY_in,
                      level_out);
}

void
RPG_Map_Common_Tools::displayDungeonLevel(const unsigned long& dimensionX_in,
                                          const unsigned long& dimensionY_in,
                                          const RPG_Map_DungeonLevel& level_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::displayDungeonLevel"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("level...\n")));

  RPG_Map_Position_t current_position;

  for (unsigned long y = 0;
       y < dimensionY_in;
       y++)
  {
    for (unsigned long x = 0;
         x < dimensionX_in;
         x++)
    {
      current_position = std::make_pair(x, y);

      // wall or door ?
      if (level_in.walls.find(current_position) != level_in.walls.end())
        std::clog << ACE_TEXT("#");
      else if (level_in.doors.find(current_position) != level_in.doors.end())
        std::clog << ACE_TEXT("=");
      else
        std::clog << ACE_TEXT(".");
    } // end FOR
    std::clog << std::endl;
  } // end FOR
}

void
RPG_Map_Common_Tools::makePartition(const unsigned long& dimensionX_in,
                                    const unsigned long& dimensionY_in,
                                    const unsigned long& numRooms_in,
                                    RPG_Map_Partition_t& partition_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::makePartition"));

  // init return value(s)
  partition_out.clear();

  // step1: distribute numRooms_in "seed" points randomly across the plane
  RPG_Map_Positions_t seed_points;
  RPG_Dice_RollResult_t result_x, result_y;
  do
  {
    result_x.clear(); result_y.clear();
    RPG_Dice::generateRandomNumbers(dimensionX_in,
                                    numRooms_in,
                                    result_x);
    RPG_Dice::generateRandomNumbers(dimensionY_in,
                                    numRooms_in,
                                    result_y);
    for (unsigned long i = 0;
         i < numRooms_in;
         i++)
    {
      seed_points.insert(std::make_pair(result_x[i] - 1,
                                        result_y[i] - 1));
      // enough data ?
      if (seed_points.size() == numRooms_in)
        break;
    } // end FOR

    // enough data ?
    if (seed_points.size() == numRooms_in)
      break;
  } while (true); // try again

  // *NOTE*: it would be nice if the partition could have some kind of metric or
  // "even" layout

  // --> *TODO*: howto enforce a minimum average distance ?
  // ideas:
  // - one way would be to compare the areas of the resulting partition
  // for rough equality
  // - another would be to do it at this stage (by enforcing some minimal distance
  // between the seed points ?)

  // *NOTE*; the min. avg. distance is zero (the general case) and
  // *NOTE*: the max avg. distance depends on x/y and n
  // *NOTE*: for the norm square (1x1), it converges around 0.5 (radius of the
  // "inner circle" ?)
  // --> in "discrete" space, things get (even) trickier...

  // init partition (with "seed" points)
  unsigned long index = 0;
  RPG_Map_PositionsIterator_t seed_iter;
  RPG_Map_Zone_t zone;
  for (seed_iter = seed_points.begin();
       seed_iter != seed_points.end();
       seed_iter++, index++)
  {
    zone.clear();
    zone.insert(*seed_iter);
    partition_out.push_back(zone);

    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("seed [#%u]: (%u,%u)\n"),
               index,
               (*seed_iter).first,
               (*seed_iter).second));
  } // end FOR

  // step2: iterate over the plane finding the "nearest neighbour" claiming
  // each cell
  // --> in case of a conflict, chance decides (fair, isn't it ?)
  // *NOTE*: "conflict" cells can generate segregated "islands"
  unsigned long min = 0;
  unsigned long distance = 0;
  RPG_Map_Positions_t neighbours;
  RPG_Map_Position_t current;
  RPG_Map_Positions_t conflicts;
  RPG_Map_PositionsIterator_t nearest_neighbour;
  RPG_Dice_RollResult_t result;
  RPG_Map_PartitionIterator_t partition_iter;
  for (unsigned long y = 0;
       y < dimensionY_in;
       y++)
  {
    for (unsigned long x = 0;
         x < dimensionX_in;
         x++)
    {
      current = std::make_pair(x, y);

      min = std::numeric_limits<unsigned long>::max();
      neighbours.clear();
      for (seed_iter = seed_points.begin();
           seed_iter != seed_points.end();
           seed_iter++)
      {
        // find all "nearest neighbours"
        distance = dist2Positions(current, *seed_iter);
        if (distance < min)
        {
          // new minimum
          min = distance;
          neighbours.clear();
          neighbours.insert(*seed_iter);
        } // end IF
        else if (distance == min)
        {
          // sanity check
          ACE_ASSERT(!neighbours.empty());

          // new neighbour
          neighbours.insert(*seed_iter);
        } // end IF
      } // end FOR
      // sanity check
      ACE_ASSERT(!neighbours.empty());

      // settle conflicts (if any)
      if (neighbours.size() == 1)
      {
//         // debug info
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("(%u,%u) --> seed: (%u,%u)\n"),
//                    x, y,
//                    (*neighbours.begin()).first,
//                    (*neighbours.begin()).second));

        // no conflict
        for (partition_iter = partition_out.begin();
             partition_iter != partition_out.end();
             partition_iter++)
        {
          if ((*partition_iter).find(*neighbours.begin()) != (*partition_iter).end())
          {
            (*partition_iter).insert(current);
            break;
          } // end IF
        } // end FOR
      } // end IF
      else
      {
//         // debug info
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("conflict for (%u,%u): %u nearest neighbours\n"),
//                    x, y,
//                    neighbours.size()));

        // conflict
        conflicts.insert(current);

        // resolve by choosing a neighbour at random
        result.clear();
        RPG_Dice::generateRandomNumbers(neighbours.size(),
                                        1,
                                        result);
        nearest_neighbour = neighbours.begin();
        std::advance(nearest_neighbour, result[0] - 1);

//         // debug info
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("(%u,%u) --> seed: (%u,%u)\n"),
//                    x, y,
//                    (*nearest_neighbour).first,
//                    (*nearest_neighbour).second));

        for (partition_iter = partition_out.begin();
             partition_iter != partition_out.end();
             partition_iter++)
        {
          if ((*partition_iter).find(*nearest_neighbour) != (*partition_iter).end())
          {
            (*partition_iter).insert(current);
            break;
          } // end IF
        } // end FOR
      } // end ELSE
    } // end FOR
  } // end FOR

  // step3: find/resolve any "islands"
  RPG_Map_PositionList_t current_island;
  RPG_Map_PositionListIterator_t current_island_iter;
  RPG_Map_Positions_t neighbour_cells;
  RPG_Map_Position_t current_neighbour;
  RPG_Map_PositionsIterator_t current_neighbour_iter;
  RPG_Map_Position_t current_seed;
  RPG_Map_PartitionIterator_t member_partition;
  RPG_Map_PartitionIterator_t neighbour_partition;
  RPG_Map_PositionsIterator_t conflict_iter;
  bool found_seed = false;
  bool accounted_for = false;
  while (!conflicts.empty())
  {
    // debug info
    displayPartition(dimensionX_in,
                     dimensionY_in,
                     seed_points,
                     conflicts,
                     partition_out);

    for (conflict_iter = conflicts.begin();
         conflict_iter != conflicts.end();
         conflict_iter++)
    {
      current = *conflict_iter;
      // find corresponding partition / seed
      for (member_partition = partition_out.begin();
           member_partition != partition_out.end();
           member_partition++)
        if ((*member_partition).find(*conflict_iter) != (*member_partition).end())
          break;
      // sanity check
      ACE_ASSERT(member_partition != partition_out.end());
      current_seed = std::make_pair(std::numeric_limits<unsigned long>::max(),
                                    std::numeric_limits<unsigned long>::max());
      for (seed_iter = seed_points.begin();
           seed_iter != seed_points.end();
           seed_iter++)
      {
        if ((*member_partition).find(*seed_iter) != (*member_partition).end())
        {
          current_seed = *seed_iter;
          break;
        } // end IF
      } // end FOR
      // sanity check
      ACE_ASSERT(seed_iter != seed_points.end());
      ACE_ASSERT(seed_points.find(current_seed) != seed_points.end());

      // has it become an "island" ?
      // --> iff the (compact) area has been cut off from the seed cell ("mainland")

      // step1: "grow" the cell
      current_island.clear();
      current_island.push_back(current);
      found_seed = false;
      for (current_island_iter = current_island.begin();
           current_island_iter != current_island.end();
           current_island_iter++)
      {
        // check four neighbouring cells (as long as they exist)
        neighbour_cells.clear();
        if ((*current_island_iter).first < (dimensionX_in - 1))
        {
          current_neighbour = *current_island_iter;
          current_neighbour.first++; // east
          neighbour_cells.insert(current_neighbour);
        } // end IF
        if ((*current_island_iter).first > 0)
        {
          current_neighbour = *current_island_iter;
          current_neighbour.first--; // west
          neighbour_cells.insert(current_neighbour);
        } // end IF
        if ((*current_island_iter).second < (dimensionY_in - 1))
        {
          current_neighbour = *current_island_iter;
          current_neighbour.second++; // south
          neighbour_cells.insert(current_neighbour);
        } // end IF
        if ((*current_island_iter).second > 0)
        {
          current_neighbour = *current_island_iter;
          current_neighbour.second--; // north
          neighbour_cells.insert(current_neighbour);
        } // end IF
        for (current_neighbour_iter = neighbour_cells.begin();
             current_neighbour_iter != neighbour_cells.end();
             current_neighbour_iter++)
        {
          // if this neighbour IS the seed point of the conflicting cell
          // --> stop immediately
          if ((*current_neighbour_iter) == current_seed)
          {
            found_seed = true;
            break;
          } // end IF

          // neighbour already accounted for (i.e. have we "come" this way ?)
          // --> continue
          accounted_for = false;
          for (RPG_Map_PositionListIterator_t current_island_iter2 = current_island.begin();
               current_island_iter2 != current_island.end();
               current_island_iter2++)
            if ((*current_island_iter2) == (*current_neighbour_iter))
            {
              accounted_for = true;
              break;
            } // end IF
          if (accounted_for)
            continue; // try next neighbour

          // find corresponding partition
          for (neighbour_partition = partition_out.begin();
               neighbour_partition != partition_out.end();
               neighbour_partition++)
            if ((*neighbour_partition).find(*current_neighbour_iter) != (*neighbour_partition).end())
              break;
          // sanity check
          ACE_ASSERT(neighbour_partition != partition_out.end());

          // part of the same "island" ?
          if (member_partition == neighbour_partition)
            current_island.push_back(*current_neighbour_iter);
        } // end FOR
      } // end FOR

      // step2: check whether the seed point was found
      if (found_seed)
      {
        // NOT and island (any more) --> nothing to do
        conflicts.erase(current);
        continue;
      } // end IF

//       // debug info
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("(%u,%u) is an island...\n"),
//                  current.first,
//                  current.second));

      // step3: (try to) dissolve this island
      // --> simply assign it to one of the neighbours ? NO !
      // *NOTE*: some neighbours may not qualify for "ownership"
      // --> to progress, the cell is assigned a DIFFERENT partition
      min = std::numeric_limits<unsigned long>::max();
      neighbours.clear();
      for (seed_iter = seed_points.begin();
           seed_iter != seed_points.end();
           seed_iter++)
      {
        // find all "nearest neighbours"
        distance = dist2Positions(current, *seed_iter);
        if (distance < min)
        {
          // new minimum
          min = distance;
          neighbours.clear();
          neighbours.insert(*seed_iter);
        } // end IF
        else if (distance == min)
        {
          // sanity check
          ACE_ASSERT(!neighbours.empty());

          // new neighbour
          neighbours.insert(*seed_iter);
        } // end IF
      } // end FOR
      // sanity check
      ACE_ASSERT(!neighbours.empty());
      do
      {
        result.clear();
        RPG_Dice::generateRandomNumbers(neighbours.size(),
                                        1,
                                        result);
        nearest_neighbour = neighbours.begin();
        std::advance(nearest_neighbour, result[0] - 1);
        // find corresponding partition
        for (partition_iter = partition_out.begin();
             partition_iter != partition_out.end();
             partition_iter++)
        {
          if ((*partition_iter).find(*nearest_neighbour) != (*partition_iter).end())
            break;
        } // end FOR
        // sanity check
        ACE_ASSERT(partition_iter != partition_out.end());
      } while (partition_iter == member_partition);
      (*member_partition).erase(current);
      (*partition_iter).insert(current);
    } // end FOR
  } // end WHILE

  // debug info
  displayPartition(dimensionX_in,
                   dimensionY_in,
                   seed_points,
                   conflicts,
                   partition_out);
}

void
RPG_Map_Common_Tools::displayPartition(const unsigned long& dimensionX_in,
                                       const unsigned long& dimensionY_in,
                                       const RPG_Map_Positions_t& seedPositions_in,
                                       const RPG_Map_Positions_t& conflicts_in,
                                       const RPG_Map_Partition_t& partition_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::displayPartition"));

  RPG_Map_Position_t current_position;

  // print conflicts separately
  // *TODO*: use boldface --> ANSI graphics ?
  if (!conflicts_in.empty())
  {
    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("partition (%u conflict(s))...\n"),
               conflicts_in.size()));

    for (unsigned long y = 0;
         y < dimensionY_in;
         y++)
    {
      for (unsigned long x = 0;
           x < dimensionX_in;
           x++)
      {
        current_position = std::make_pair(x, y);

        if (seedPositions_in.find(current_position) != seedPositions_in.end())
          std::clog << ACE_TEXT("@");
        else if (conflicts_in.find(current_position) != conflicts_in.end())
          std::clog << ACE_TEXT("X");
        else
          std::clog << ACE_TEXT(".");
      } // end FOR
      std::clog << std::endl;
    } // end FOR
  } // end IF
  else
  {
    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("partition...\n")));

    unsigned long index = 0;
    std::ostringstream converter;
    for (unsigned long y = 0;
         y < dimensionY_in;
         y++)
    {
      for (unsigned long x = 0;
           x < dimensionX_in;
           x++)
      {
        current_position = std::make_pair(x, y);
        index = 0;
        for (RPG_Map_PartitionConstIterator_t iter = partition_in.begin();
            iter != partition_in.end();
            iter++, index++)
        {
          if ((*iter).find(current_position) != (*iter).end())
          {
            if (seedPositions_in.find(current_position) != seedPositions_in.end())
              std::clog << ACE_TEXT("@");
            else
            {
              converter.str(ACE_TEXT(""));
              converter << index;
              std::clog << converter.str();
            } // end ELSE

            break;
          } // end IF
        } // end FOR
      } // end FOR
      std::clog << std::endl;
    } // end FOR
  } // end ELSE
}

void
RPG_Map_Common_Tools::makeRooms(const RPG_Map_Partition_t& partition_in,
                                const bool& wantSquareRooms_in,
                                const bool& maximizeArea_in,
                                const unsigned long& minArea_in,
                                RPG_Map_ZoneList_t& rooms_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::makeRooms"));

  // init return value(s)
  rooms_out.clear();

  // place a room into every partition...

  // step1: make some calculations...
  RPG_Map_SquareList_t maxSquares;
  RPG_Map_PartitionConstIterator_t partition_iter;
  RPG_Map_ZoneConstIterator_t zone_iterator;
  if (wantSquareRooms_in)
  {
    // make square room(s)
    RPG_Map_ZoneConstIterator_t zone_iterator2;
    RPG_Map_ZoneConstIterator_t pointer;
//     RPG_Map_ZoneConstIterator_t last;
    RPG_Map_Square_t maxSquare;
    unsigned long maxArea = 0;
    unsigned long current_area = 0;
    unsigned long max_breadth = 0;
    unsigned long current_row = 0;
    unsigned long last_x = 0;
    for (partition_iter = partition_in.begin();
         partition_iter != partition_in.end();
         partition_iter++)
    {
      // step1: find coordinates of the maximum-size square room(s)
      // --> can fit in smaller rooms anytime...
      maxArea = 0;
//       last = (*partition_iter).end(); last--;
      for (zone_iterator = (*partition_iter).begin();
           zone_iterator != (*partition_iter).end();
           zone_iterator++)
      {
        max_breadth = 0;
        current_row = (*zone_iterator).second;
        last_x = (*zone_iterator).first;
        for (zone_iterator2 = zone_iterator;
             zone_iterator2 != (*partition_iter).end();
             zone_iterator2++)
        {
          // still on the top row ?
          if ((*zone_iterator2).second == ((*zone_iterator).second))
          {
            // check: for consecutive positions WITHIN the same row:
            // if (previous_cell.x + 1) is NOT EQUAL to the current.x
            // i.e. the current cell is not adjacent to the previous one
            // --> we've already found the largest square for this position
            if (last_x != (*zone_iterator2).first)
              break; // start next position
            last_x++;

            // --> compute enclosed area and continue
            current_area = area2Positions(*zone_iterator, *zone_iterator2);
            if (maxArea < current_area)
            {
              maxArea = current_area;
              maxSquare.ul = *zone_iterator;
              maxSquare.lr = *zone_iterator2;
            } // end IF

            // check next cell
            continue;
          } // end IF

          // we're NOT on the top row anymore...
          ACE_ASSERT((*zone_iterator2).second != (*zone_iterator).second);

          // step1: compute max. breadth of current square
          // --> compute the distance to the LAST cell on the top row
          if (max_breadth == 0)
          {
            // reached the start of the second row
            ACE_ASSERT((*zone_iterator2).second == ((*zone_iterator).second + 1));

            // retrieve the last cell of the top row
            pointer = zone_iterator2;
            pointer--;
            // compute max breadth
            max_breadth = (((*pointer).first - (*zone_iterator).first) + 1);
            ACE_ASSERT(max_breadth);
          } // end IF

          // check: IMMEDIATELY on breaking to a new row:
          // if our position.x is LARGER than our current_square.ul.x
          // --> we've already found the largest square for this position
          if ((*zone_iterator2).second > current_row)
          {
            last_x = (*zone_iterator2).first;
            current_row++;
            ACE_ASSERT((*zone_iterator2).second == current_row);

            if ((*zone_iterator2).first > (*zone_iterator).first)
              break; // start next position

            // check: if the last_cell.x of the previous row was
            // SMALLER than current_square.ul.x
            // --> we've already found the largest square for this position

            // retrieve the last cell of the previous row
            pointer = zone_iterator2;
            pointer--;
            if ((*pointer).first < (*zone_iterator).first)
              break; // start next position

            // otherwise, perhaps max_breadth needs subtraction...
            if ((((*pointer).first - (*zone_iterator).first) + 1) < max_breadth)
              max_breadth = (((*pointer).first - (*zone_iterator).first) + 1);
            ACE_ASSERT(max_breadth);
          } // end IF
          else
          {
            // as long as the current_cell.x is EQUAL OR SMALLER than
            // current_square.ul.x (i.e. we're left/at the reference), ignore
            // any skips between consecutive positions (see below)
            if ((*zone_iterator2).first <= (*zone_iterator).first)
              last_x = (*zone_iterator2).first;
            else
              last_x++;
          } // end ELSE

          // check: check if we're within the square spanned by
          // [current_square.ul, (current_square.ul.x + (max_breadth - 1), y)]
          if (((*zone_iterator2).first > ((*zone_iterator).first + max_breadth - 1)) ||
              ((*zone_iterator2).first < (*zone_iterator).first))
            continue; // no, we're not --> check next cell

          // we're within the current square

          // check: for consecutive positions WITHIN the same row:
          // if (previous_cell.x + 1) is NOT EQUAL to the current.x
          // i.e. the current cell is not adjacent to the previous one
          // --> we've already found the largest square for this position
          if (last_x != (*zone_iterator2).first)
            break; // start next position

          // step2: we're within the current square
          // --> compute enclosed area
          current_area = area2Positions(*zone_iterator, *zone_iterator2);
          if (maxArea < current_area)
          {
            maxArea = current_area;
            maxSquare.ul = *zone_iterator;
            maxSquare.lr = *zone_iterator2;
          } // end IF
        } // end FOR
      } // end FOR

      // debug info
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("max. square [(%u,%u),(%u,%u)]: %u cell(s)\n"),
                 maxSquare.ul.first,
                 maxSquare.ul.second,
                 maxSquare.lr.first,
                 maxSquare.lr.second,
                 maxArea));

      maxSquares.push_back(maxSquare);
    } // end FOR
  } // end ELSE

  // step2: create rooms
  RPG_Map_Zone_t current_zone;
  if (maximizeArea_in)
  {
    if (wantSquareRooms_in)
    {
      RPG_Map_SquareListConstIterator_t squares_iter = maxSquares.begin();
      for (partition_iter = partition_in.begin();
           partition_iter != partition_in.end();
           partition_iter++, squares_iter++)
      {
        current_zone.clear();
        for (zone_iterator = (*partition_iter).begin();
             zone_iterator != (*partition_iter).end();
             zone_iterator++)
        {
          if (positionInSquare(*zone_iterator, *squares_iter))
            current_zone.insert(*zone_iterator);
        } // end FOR

        rooms_out.push_back(current_zone);

//         // debug info
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("zone [%u cell(s)]: bounded by [(%u,%u),(%u,%u)] --> %u cell(s)\n"),
//                    current_zone.size(),
//                    (*squares_iter).ul.first,
//                    (*squares_iter).ul.second,
//                    (*squares_iter).lr.first,
//                    (*squares_iter).lr.second,
//                    area2Positions((*squares_iter).ul,
//                                   (*squares_iter).lr)));

        ACE_ASSERT(current_zone.size() == area2Positions((*squares_iter).ul,
                                                         (*squares_iter).lr));
      } // end FOR
    } // end IF
    else
    {
      // --> the partitions ARE the rooms
      // *NOTE*: there will be NO corridor(s), only door(s)...
      rooms_out = partition_in;
    } // end ELSE
  } // end IF
  else
  {
    // *TODO*
    ACE_ASSERT(false);
  } // end ELSE

  // step3: enforce any other constraint(s)
  if (minArea_in)
  {
    unsigned long index = 0;
    RPG_Map_ZoneListConstIterator_t zones_iterator;
    for (zones_iterator = rooms_out.begin();
         zones_iterator != rooms_out.end();
         zones_iterator++, index++)
    {
      if ((*zones_iterator).size() < minArea_in)
      {
        // debug info
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("zone [#%u] is too small (%u < %u)...\n"),
                   index,
                   (*zones_iterator).size(),
                   minArea_in));

          // *TODO*
        ACE_ASSERT(false);
      } // end IF
    } // end FOR
  } // end IF
}

void
RPG_Map_Common_Tools::displayRooms(const unsigned long& dimensionX_in,
                                   const unsigned long& dimensionY_in,
                                   const RPG_Map_ZoneList_t& rooms_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::displayRooms"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("room(s)...\n")));

  RPG_Map_Position_t current_position;

  unsigned long index = 0;
  std::ostringstream converter;
  bool found_position = false;
  for (unsigned long y = 0;
       y < dimensionY_in;
       y++)
  {
    for (unsigned long x = 0;
         x < dimensionX_in;
         x++)
    {
      current_position = std::make_pair(x, y);
      index = 0;
      found_position = false;
      for (RPG_Map_ZoneListConstIterator_t iter = rooms_in.begin();
           iter != rooms_in.end();
           iter++, index++)
      {
        if ((*iter).find(current_position) != (*iter).end())
        {
          found_position = true;
          converter.str(ACE_TEXT(""));
          converter << index;
          std::clog << converter.str();
          break;
        } // end IF
      } // end FOR
      if (found_position)
        continue;
      // position not part of any room
      std::clog << ACE_TEXT(".");
    } // end FOR
    std::clog << std::endl;
  } // end FOR
}

void
RPG_Map_Common_Tools::connectRooms(const RPG_Map_ZoneList_t& rooms_in,
                                   const unsigned long& maxDoorsPerRoom_in,
                                   RPG_Map_DungeonLevel& level_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::connectRooms"));

  // init return value(s)
  level_out.doors.clear();
  level_out.walls.clear();

  // step1: make walls
  RPG_Map_ZoneList_t shells;
  RPG_Map_Zone_t current_room;
  RPG_Map_ZoneListConstIterator_t list_iterator;
  RPG_Map_ZoneConstIterator_t zone_iterator;
  RPG_Map_Position_t position;
  unsigned long max_x = 0;
  unsigned long max_y = 0;
  for (list_iterator = rooms_in.begin();
       list_iterator != rooms_in.end();
       list_iterator++)
  {
    current_room.clear();

    // step1: find outer shell
    // compute max_x, max_y
    for (zone_iterator = (*list_iterator).begin();
         zone_iterator != (*list_iterator).end();
         zone_iterator++)
    {
      if ((*zone_iterator).first > max_x)
        max_x = (*zone_iterator).first;
      if ((*zone_iterator).second > max_y)
        max_y = (*zone_iterator).second;
    } // end FOR

    // start at top left position and go around clockwise
    // *NOTE*: as the set is sorted, start with the first element
    current_room.insert(*((*list_iterator).begin()));
    // as long as possible, proceed to the right and down, alternatingly
    position = *((*list_iterator).begin());
    while ((position.first != max_x) ||
            (position.second != max_y))
    {
      // as long as possible, proceed right
      position.first++;
      while ((position.first != (max_x + 1)) &&
             ((*list_iterator).find(position) != (*list_iterator).end()))
      {
        // part of the room
        current_room.insert(position);
        position.first++;
      } // end WHILE
      position.first--;
      // as long as possible, proceed downward
      position.second++;
      while ((position.second != (max_y + 1)) &&
             ((*list_iterator).find(position) != (*list_iterator).end()))
      {
        // part of the room
        current_room.insert(position);
        position.second++;
      } // end WHILE
      position.second--;
    } // end WHILE
    // part of the room
    current_room.insert(position);
    // as long as possible, proceed to the left and up, alternatingly
    while ((position.first != (*((*list_iterator).begin())).first) &&
           (position.second != (*((*list_iterator).begin())).second))
    {
      // as long as possible, proceed left
      position.first--;
      while ((position.first != ((*((*list_iterator).begin())).first - 1)) &&
             ((*list_iterator).find(position) != (*list_iterator).end()))
      {
        // part of the room
        current_room.insert(position);
        position.first--;
      } // end WHILE
      position.first++;
      // as long as possible, proceed upward
      position.second--;
      while ((position.second != ((*((*list_iterator).begin())).second - 1)) &&
             ((*list_iterator).find(position) != (*list_iterator).end()))
      {
        // part of the room
        current_room.insert(position);
        position.second--;
      } // end WHILE
      position.second++;
    } // end WHILE
    ACE_ASSERT(position == *((*list_iterator).begin()));

    shells.push_back(current_room);
  } // end FOR

  // step2: make doors
  // *NOTE*: every room needs at least one (possibly secret) door...
  // *NOTE*: doors connect rooms
  RPG_Map_ZoneList_t doors;
  RPG_Map_Zone_t current_doors;
  unsigned long num_doors = 0;
  RPG_Dice_RollResult_t result;
  RPG_Dice_RollResult_t result_y;
  unsigned long min_distance = std::numeric_limits<unsigned long>::max();
  unsigned long distance = 0;
  unsigned long max_num_doors = 0;
  RPG_Map_Position_t nearest_neighbour;
  RPG_Map_Positions_t neighbours;
  RPG_Map_PositionsConstIterator_t neighbour_iterator;
  bool position_suitable = false;
  for (list_iterator = shells.begin();
       list_iterator != shells.end();
       list_iterator++)
  {
    current_doors.clear();

    // step1: generate (random) number of doors
    result.clear();
    // *NOTE*: enforce a physical distance between any 2 doors
    max_num_doors = numDoorPositions(*list_iterator);
    RPG_Dice::generateRandomNumbers((maxDoorsPerRoom_in ? ((maxDoorsPerRoom_in > max_num_doors) ? max_num_doors
                                                                                                : maxDoorsPerRoom_in)
                                                        : max_num_doors),
                                    1,
                                    result);
    num_doors = result[0];

    // step2: position doors
    // compute max_x, max_y
    for (zone_iterator = (*list_iterator).begin();
         zone_iterator != (*list_iterator).end();
         zone_iterator++)
    {
      if ((*zone_iterator).first > max_x)
        max_x = (*zone_iterator).first;
      if ((*zone_iterator).second > max_y)
        max_y = (*zone_iterator).second;
    } // end FOR

    while (current_doors.size() < num_doors)
    {
      position_suitable = false;

      // step3: generate (random) coordinates
      result.clear(); result_y.clear();
      RPG_Dice::generateRandomNumbers((max_x + 1),
                                      1,
                                      result);
      RPG_Dice::generateRandomNumbers((max_y + 1),
                                      1,
                                      result_y);
      position = std::make_pair(result[0] - 1,
                                result_y[0] - 1);
      // the position is probably not on the perimeter
      // --> find the nearest neighbour
      if ((*list_iterator).find(position) == (*list_iterator).end())
      {
        min_distance = std::numeric_limits<unsigned long>::max();
        for (zone_iterator = (*list_iterator).begin();
             zone_iterator != (*list_iterator).end();
             zone_iterator++)
        {
          distance = dist2Positions(*zone_iterator, position);
          if (distance < min_distance)
          {
            nearest_neighbour = *zone_iterator;
            min_distance = distance;
          } // end IF
        } // end FOR
        position = nearest_neighbour;
      } // end IF

      // it's on the perimeter

      // step4: is this position a suitable spot for a door ?
      // *NOTE*: for now, corners are unsuitable...
      // --> check whether the position has two (!) neighbours "facing" different
      // directions
      for (zone_iterator = (*list_iterator).begin();
           zone_iterator != (*list_iterator).end();
           zone_iterator++)
      {
        if (dist2Positions(*zone_iterator, position) == 1)
          neighbours.insert(*zone_iterator);
      } // end FOR
      ACE_ASSERT(neighbours.size() == 2);
      nearest_neighbour = position;
      for (neighbour_iterator = neighbours.begin();
           neighbour_iterator != neighbours.end();
           neighbour_iterator++)
      {
        if (nearest_neighbour == position)
        {
          // first iteration, store a neighbour
          nearest_neighbour = *neighbour_iterator;
          continue;
        } // end IF

        // find relation between three positions
        if (((position.first == nearest_neighbour.first) &&
             (nearest_neighbour.first == (*neighbour_iterator).first)) ||
            ((position.second == nearest_neighbour.second) &&
             (nearest_neighbour.second == (*neighbour_iterator).second)))
        {
          // OK, they align
          position_suitable = true;
          break;
        } // end IF
      } // end FOR

      if (position_suitable)
      {
        current_doors.insert(position);
//         (*list_iterator).erase(position); // not a wall anymore...
      } // end IF
    } // end WHILE

    doors.push_back(current_doors);
  } // end FOR

  // step3: make corridors
  RPG_Map_ZoneList_t corridors;
  RPG_Map_Zone_t current_corridor;
  // *TODO*:
  ACE_ASSERT(false);

  // step4: put everything together
  for (list_iterator = shells.begin();
       list_iterator != shells.end();
       list_iterator++)
  {
    for (zone_iterator = (*list_iterator).begin();
         zone_iterator != (*list_iterator).end();
         zone_iterator++)
    {
      level_out.walls.insert(*zone_iterator);
    } // end FOR
  } // end FOR
  RPG_Map_ZoneConstIterator_t doors_iterator;
  for (list_iterator = doors.begin();
       list_iterator != doors.end();
       list_iterator++)
  {
    for (doors_iterator = (*list_iterator).begin();
         doors_iterator != (*list_iterator).end();
         doors_iterator++)
    {
      level_out.doors.insert(*doors_iterator);
    } // end FOR
  } // end FOR
}

const unsigned long
RPG_Map_Common_Tools::dist2Positions(const RPG_Map_Position_t& position1_in,
                                     const RPG_Map_Position_t& position2_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::dist2Positions"));

  return (::abs(position1_in.first - position2_in.first) +
          ::abs(position1_in.second - position2_in.second));
}

const unsigned long
RPG_Map_Common_Tools::area2Positions(const RPG_Map_Position_t& position1_in,
                                     const RPG_Map_Position_t& position2_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::area2Positions"));

  return ((::abs(position1_in.first - position2_in.first) + 1) *
          (::abs(position1_in.second - position2_in.second) + 1));
}

const bool
RPG_Map_Common_Tools::positionInSquare(const RPG_Map_Position_t& position_in,
                                       const RPG_Map_Square_t& square_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::positionInSquare"));

  return ((position_in.first >= square_in.ul.first) &&
          (position_in.first <= square_in.lr.first) &&
          (position_in.second >= square_in.ul.second) &&
          (position_in.second <= square_in.lr.second));
}

const unsigned long
RPG_Map_Common_Tools::numDoorPositions(const RPG_Map_Zone_t& room_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::numDoorPositions"));

  unsigned long result = 0;

  // *TODO*
  ACE_ASSERT(false);

  return result;
}
