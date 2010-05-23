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

#include <rpg_dice.h>

#include <ace/Log_Msg.h>

#include <sstream>
#include <iostream>

void
RPG_Map_Common_Tools::createDungeonLevel(const unsigned long& dimensionX_in,
                                         const unsigned long& dimensionY_in,
                                         const unsigned long& numRooms_in,
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
  makeRooms(partition, rooms);

  // step3: connect rooms to form the dungeon
  connectRooms(rooms, level_out);
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
               ACE_TEXT("partition has %u conflict(s)...\n"),
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
  std::clog << std::endl;
}

void
RPG_Map_Common_Tools::makeRooms(const RPG_Map_Partition_t& partition_in,
                                RPG_Map_ZoneList_t& rooms_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::makeRooms"));

  // init return value(s)
  rooms_out.clear();

}

void
RPG_Map_Common_Tools::connectRooms(const RPG_Map_ZoneList_t& rooms_in,
                                   RPG_Map_DungeonLevel& level_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::connectRooms"));

  // init return value(s)
  level_out.doors.clear();
  level_out.walls.clear();

}

const unsigned long
RPG_Map_Common_Tools::dist2Positions(const RPG_Map_Position_t& position1_in,
                                     const RPG_Map_Position_t& position2_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::dist2Positions"));

  return (::abs(position1_in.first - position2_in.first) +
          ::abs(position1_in.second - position2_in.second));
}
