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

// *NOTE*: workaround for MSVC quirk with min/max
#if defined (_MSC_VER)
#define NOMINMAX
#endif

#include "rpg_map_common_tools.h"

#include "rpg_map_direction.h"
#include "rpg_map_doorstate.h"
#include "rpg_map_data.h"
#include "rpg_map_parser_driver.h"
#include "rpg_map_pathfinding_tools.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
#include "rpg_common_file_tools.h"

#include "rpg_dice.h"

#include <ace/Time_Value.h>
#include <ace/Log_Msg.h>

#include <sstream>
#include <iostream>
#include <stack>
#include <iterator>

// init statics
RPG_Map_DirectionToStringTable_t
RPG_Map_DirectionHelper::myRPG_Map_DirectionToStringTable;
RPG_Map_DoorStateToStringTable_t
RPG_Map_DoorStateHelper::myRPG_Map_DoorStateToStringTable;

void
RPG_Map_Common_Tools::initStringConversionTables()
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::initStringConversionTables"));

  RPG_Map_DirectionHelper::init();
  RPG_Map_DoorStateHelper::init();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Map_Common_Tools: initialized string conversion tables...\n")));
}

void
RPG_Map_Common_Tools::createFloorPlan(const unsigned int& dimensionX_in,
                                      const unsigned int& dimensionY_in,
                                      const unsigned int& numAreas_in,
                                      const bool& wantSquareRooms_in,
                                      const bool& maximizeRooms_in,
                                      const unsigned int& minRoomArea_in,
                                      const bool& wantDoors_in,
                                      const bool& wantCorridors_in,
                                      const bool& doorFillsPosition_in,
                                      const unsigned int& maxDoorsPerRoom_in,
                                      RPG_Map_Positions_t& seedPositions_out,
                                      RPG_Map_FloorPlan_t& floorPlan_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::createFloorPlan"));

  // init return value(s)
  seedPositions_out.clear();
  floorPlan_out.size_x = dimensionX_in;
  floorPlan_out.size_y = dimensionY_in;
  floorPlan_out.unmapped.clear();
  floorPlan_out.walls.clear();
  floorPlan_out.doors.clear();
  floorPlan_out.rooms_are_square = wantSquareRooms_in;

  RPG_Map_Partition_t partition;
  RPG_Map_Positions_t conflicts;
  RPG_Map_AreaList_t boundaries, rooms, doors;
  bool some_rooms_empty = true;
  bool no_rooms_empty = false;
  unsigned int index = 1;
  do
  {
    partition.clear();
    conflicts.clear();
    rooms.clear();
    doors.clear();
    boundaries.clear();
    some_rooms_empty = true;
    no_rooms_empty = false;

    // step1: segment area into numRooms_in partition(s) (--> and room(s))
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("segmenting area...\n")));

    makePartition(dimensionX_in,
                  dimensionY_in,
                  numAreas_in,
                  (wantCorridors_in ? true : false), // resolve conflicts !
                  conflicts,
                  partition);

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("segmenting area...DONE\n")));

    if (wantDoors_in)
    {
      // step2: form rooms within partition(s)
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("making rooms...\n")));

      makeRooms(dimensionX_in,
                dimensionY_in,
                partition,
                wantSquareRooms_in,
                (wantCorridors_in ? true : false), // requires separated rooms !
                (doorFillsPosition_in ? true : false), // requires cropped rooms !
                maximizeRooms_in,
                minRoomArea_in,
                rooms,
                boundaries);

      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("making rooms...DONE\n")));

      // check: all rooms are not empty ?
      no_rooms_empty = true;
      for (RPG_Map_AreaListConstIterator_t arealist_iter = rooms.begin();
           arealist_iter != rooms.end();
           arealist_iter++)
        if ((*arealist_iter).empty())
        {
          no_rooms_empty = false;
          break;
        } // end IF
      if (no_rooms_empty)
        some_rooms_empty = false;

      if (some_rooms_empty)
      {
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("result[%u] is invalid, retrying...\n"),
//                    index));
      } // end IF
      else
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("making doors...\n")));

        makeDoors(dimensionX_in,
                  dimensionY_in,
                  boundaries,
                  doorFillsPosition_in,
                  maxDoorsPerRoom_in,
                  doors);

        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("making doors...DONE\n")));
      } // end ELSE
    } // end IF
    else
      break; // there are no rooms...

    index++;
  } while (some_rooms_empty);

  if (!rooms.empty() && wantCorridors_in)
  {
//     displayRooms(dimensionX_in,
//                  dimensionY_in,
//                  rooms);

    // step3: create corridors_bounds
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("making corridors...\n")));

    connectRooms(dimensionX_in,
                 dimensionY_in,
                 boundaries,
                 doors,
                 rooms,
                 floorPlan_out);

     ACE_DEBUG((LM_DEBUG,
                ACE_TEXT("making corridors...DONE\n")));
  } // end IF
  else
  {
    // *NOTE*: in this case, conflict areas represent walls...
    // *TODO*: what if some areas are not reachable ?
    // --> create some doors ?
    floorPlan_out.walls = conflicts;
  } // end ELSE

  // step4: determine "seed" positions
  RPG_Map_AreaListConstIterator_t rooms_iter = rooms.begin();
  RPG_Map_AreaListConstIterator_t boundaries_iter = boundaries.begin();
  RPG_Dice_RollResult_t roll_result;
  RPG_Map_AreaConstIterator_t area_iterator;
  for (RPG_Map_PartitionConstIterator_t iterator = partition.begin();
       iterator != partition.end();
       iterator++, rooms_iter++, boundaries_iter++)
  {
    if (rooms.empty())
    {
      RPG_Map_Positions_t partition_area, perimeter;
      RPG_Map_PositionList_t valid_area;
      RPG_Map_Common_Tools::perimeter((*iterator).area, perimeter);
      partition_area.insert((*iterator).area.begin(), (*iterator).area.end());
      RPG_Map_PositionListIterator_t area_iterator2 =
          std::set_difference(partition_area.begin(), partition_area.end(),
                              perimeter.begin(), perimeter.end(),
                              valid_area.begin());

      // find a position within (!) the partition...
      roll_result.clear();
      RPG_Dice::generateRandomNumbers(valid_area.size(),
                                      1,
                                      roll_result);
      std::advance(area_iterator2, roll_result.front() - 1);

      seedPositions_out.insert(*area_iterator2);
    } // end IF
    else
    {
      // *NOTE*: iff the actual seed position does not lie within (!) the
      // corresponding room, choose a random position that does...
      if (((*rooms_iter).find((*iterator).seed)      != (*rooms_iter).end())      &&
          ((*boundaries_iter).find((*iterator).seed) == (*boundaries_iter).end()))
      {
        seedPositions_out.insert((*iterator).seed);
        continue;
      } // end IF

      // find a position within (!) the room...
      do
      {
        roll_result.clear();
        area_iterator = (*rooms_iter).begin();
        RPG_Dice::generateRandomNumbers((*rooms_iter).size(),
                                        1,
                                        roll_result);
        std::advance(area_iterator, roll_result.front() - 1);
      } while ((*boundaries_iter).find(*area_iterator) !=
               (*boundaries_iter).end()); // try again ?

      seedPositions_out.insert(*area_iterator);
    } // end ELSE
  } // end FOR
}

void
RPG_Map_Common_Tools::makePartition(const unsigned int& dimensionX_in,
                                    const unsigned int& dimensionY_in,
                                    const unsigned int& numRooms_in,
                                    const bool& resolveConflicts_in,
                                    RPG_Map_Positions_t& conflicts_out,
                                    RPG_Map_Partition_t& partition_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::makePartition"));

  // init return value(s)
  conflicts_out.clear();
  partition_out.clear();

  // step1: distribute numRooms_in "seed" points randomly across the plane
  RPG_Dice_RollResult_t result_x, result_y;
  RPG_Map_Positions_t seed_positions;
  do
  {
    seed_positions.clear();
    result_x.clear(); result_y.clear();
    RPG_Dice::generateRandomNumbers(dimensionX_in,
                                    numRooms_in,
                                    result_x);
    RPG_Dice::generateRandomNumbers(dimensionY_in,
                                    numRooms_in,
                                    result_y);
    for (unsigned int i = 0;
         i < numRooms_in;
         i++)
    {
      seed_positions.insert(std::make_pair(result_x[i] - 1,
                                           result_y[i] - 1));
      // enough data ?
      if (seed_positions.size() == numRooms_in)
        break;
    } // end FOR
    // enough data ?
    if (seed_positions.size() == numRooms_in)
      break;
  } while (true); // try again

  // *NOTE*: it would be nice if the partition could have some kind of metric or
  // "even" layout

  // --> *TODO*: howto enforce a minimum average distance ?
  // ideas:
  // - one way would be to compare the areas of the resulting partition
  // for rough equality
  // - another would be to do it at this stage (by enforcing some minimal
  //   distance between the seed points ?)

  // *NOTE*; the min. avg. distance is zero (the general case) and
  // *NOTE*: the max avg. distance depends on x/y and n
  // *NOTE*: for the norm square (1x1), it converges around 0.5 (radius of the
  // "inner circle" ?)
  // --> in "discrete" space, things get (even) trickier...

  // init partition (with "seed" points)
  unsigned int index = 0;
  RPG_Map_PositionsIterator_t seed_iter;
  RPG_Map_Zone_t zone;
  for (seed_iter = seed_positions.begin();
       seed_iter != seed_positions.end();
       seed_iter++, index++)
  {
    zone.area.clear();
    zone.area.insert(*seed_iter);
    zone.seed = *seed_iter;
    partition_out.push_back(zone);

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("seed [#%u]: (%u,%u)\n"),
//                index,
//                (*seed_iter).first,
//                (*seed_iter).second));
  } // end FOR

  // step2: iterate over the plane finding the "nearest neighbour" claiming
  // each cell
  // --> in case of a conflict, chance decides (fair, isn't it ?)
  // *NOTE*: "conflict" cells can generate segregated "islands"
  unsigned int min = 0;
  unsigned int distance = 0;
  RPG_Map_Positions_t neighbours;
  RPG_Map_Position_t current_position;
  RPG_Map_PositionsIterator_t nearest_neighbour;
  RPG_Dice_RollResult_t result;
  RPG_Map_PartitionIterator_t partition_iter;
  for (unsigned int y = 0;
       y < dimensionY_in;
       y++)
  {
    for (unsigned int x = 0;
         x < dimensionX_in;
         x++)
    {
      current_position = std::make_pair(x, y);

      min = std::numeric_limits<unsigned int>::max();
      neighbours.clear();
      for (seed_iter = seed_positions.begin();
           seed_iter != seed_positions.end();
           seed_iter++)
      {
        // find all "nearest neighbours"
        distance = RPG_Map_Common_Tools::distance(current_position, *seed_iter);
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
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("(%u,%u) --> seed: (%u,%u)\n"),
//                    x, y,
//                    (*neighbours.begin()).first,
//                    (*neighbours.begin()).second));

        // no conflict
        for (partition_iter = partition_out.begin();
             partition_iter != partition_out.end();
             partition_iter++)
          if ((*partition_iter).area.find(*neighbours.begin()) !=
              (*partition_iter).area.end())
          {
            (*partition_iter).area.insert(current_position);
            break;
          } // end IF
      } // end IF
      else
      {
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("conflict for (%u,%u): %u nearest neighbours\n"),
//                    x, y,
//                    neighbours.size()));

        // conflict
        conflicts_out.insert(current_position);

        // resolve by choosing a neighbour at random
        result.clear();
        RPG_Dice::generateRandomNumbers(neighbours.size(),
                                        1,
                                        result);
        nearest_neighbour = neighbours.begin();
        std::advance(nearest_neighbour, result[0] - 1);

//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("(%u,%u) --> seed: (%u,%u)\n"),
//                    x, y,
//                    (*nearest_neighbour).first,
//                    (*nearest_neighbour).second));

        for (partition_iter = partition_out.begin();
             partition_iter != partition_out.end();
             partition_iter++)
          if ((*partition_iter).area.find(*nearest_neighbour) !=
              (*partition_iter).area.end())
          {
            (*partition_iter).area.insert(current_position);
            break;
          } // end IF
      } // end ELSE
    } // end FOR
  } // end FOR

  // step3: find/resolve any "islands"
  RPG_Map_PositionList_t current_island;
  RPG_Map_PositionListIterator_t current_island_iter;
  RPG_Map_Positions_t neighbour_cells;
  RPG_Map_Position_t current_neighbour;
  RPG_Map_PositionsIterator_t current_neighbour_iter;
  RPG_Map_PartitionIterator_t member_partition;
  RPG_Map_PartitionIterator_t neighbour_partition;
  bool found_seed = false;
  bool accounted_for = false;
  if (resolveConflicts_in)
    while (!conflicts_out.empty())
    {
  //     // debug info
  //     displayPartition(dimensionX_in,
  //                      dimensionY_in,
  //                      conflicts_out,
  //                      seed_points,
  //                      partition_out);

      current_position = *conflicts_out.begin();

      // step0: find corresponding partition
      for (member_partition = partition_out.begin();
           member_partition != partition_out.end();
           member_partition++)
        if ((*member_partition).area.find(*conflicts_out.begin()) !=
            (*member_partition).area.end())
          break;
      // sanity check
      ACE_ASSERT(member_partition != partition_out.end());

      // has it become an "island" ?
      // --> iff the (compact) area has been cut off from the seed cell
      // ("mainland")

      // step1: "grow" the cell
      current_island.clear();
      current_island.push_back(current_position);
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
          if ((*current_neighbour_iter) == (*member_partition).seed)
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
            if ((*neighbour_partition).area.find(*current_neighbour_iter) !=
                (*neighbour_partition).area.end())
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
        conflicts_out.erase(current_position);
        continue;
      } // end IF

//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("(%u,%u) is an island...\n"),
//                  current.first,
//                  current.second));

      // step3: (try to) dissolve this island
      // --> simply assign it to one of the neighbours ? NO !
      // *NOTE*: some neighbours may not qualify for "ownership"
      // --> to progress, the cell is assigned a DIFFERENT partition
      min = std::numeric_limits<unsigned int>::max();
      neighbours.clear();
      for (seed_iter = seed_positions.begin();
           seed_iter != seed_positions.end();
           seed_iter++)
      {
        // find all "nearest neighbours"
        distance = RPG_Map_Common_Tools::distance(current_position, *seed_iter);
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
          if ((*partition_iter).area.find(*nearest_neighbour) !=
              (*partition_iter).area.end())
            break;
        } // end FOR
        // sanity check
        ACE_ASSERT(partition_iter != partition_out.end());
      } while (partition_iter == member_partition);
      (*member_partition).area.erase(current_position);
      (*partition_iter).area.insert(current_position);
    } // end WHILE

//   displayPartition(dimensionX_in,
//                    dimensionY_in,
//                    conflicts_out,
//                    seedPoints_out,
//                    partition_out);
}

void
RPG_Map_Common_Tools::displayPartition(const unsigned int& dimensionX_in,
                                       const unsigned int& dimensionY_in,
                                       const RPG_Map_Positions_t& conflicts_in,
                                       const RPG_Map_Positions_t& seedPositions_in,
                                       const RPG_Map_Partition_t& partition_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::displayPartition"));

  RPG_Map_Position_t current_position;

  // print conflicts separately
  // *TODO*: use boldface --> ANSI graphics ?
  if (!conflicts_in.empty())
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("partition (%u conflict(s))...\n"),
               conflicts_in.size()));

    for (unsigned int y = 0;
         y < dimensionY_in;
         y++)
    {
      for (unsigned int x = 0;
           x < dimensionX_in;
           x++)
      {
        current_position = std::make_pair(x, y);

        if (seedPositions_in.find(current_position) != seedPositions_in.end())
          std::clog << '@';
        else if (conflicts_in.find(current_position) != conflicts_in.end())
          std::clog << 'X';
        else
          std::clog << '.';
      } // end FOR
      std::clog << std::endl;
    } // end FOR
  } // end IF
  else
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("partition...\n")));

    unsigned int index = 0;
    std::ostringstream converter;
    for (unsigned int y = 0;
         y < dimensionY_in;
         y++)
    {
      for (unsigned int x = 0;
           x < dimensionX_in;
           x++)
      {
        current_position = std::make_pair(x, y);
        index = 0;
        for (RPG_Map_PartitionConstIterator_t iter = partition_in.begin();
            iter != partition_in.end();
            iter++, index++)
        {
          if ((*iter).area.find(current_position) != (*iter).area.end())
          {
            if (current_position == (*iter).seed)
            {
              std::clog << '@';
              continue;
            } // end IF

            converter.str(ACE_TEXT(""));
            converter << index;
            std::clog << converter.str();

            break;
          } // end IF
        } // end FOR
      } // end FOR
      std::clog << std::endl;
    } // end FOR
  } // end ELSE
}

void
RPG_Map_Common_Tools::findMaxSquare(const RPG_Map_Area_t& room_in,
                                    RPG_Map_Square_t& maxSquare_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::findMaxSquare"));

  // init return value(s)
  maxSquare_out.ul = std::make_pair(0, 0);
  maxSquare_out.lr = std::make_pair(0, 0);

  RPG_Map_AreaConstIterator_t pointer;
  unsigned int maxArea = 0;
  unsigned int current_area = 0;
  unsigned int max_breadth = 0;
  unsigned int current_row = 0;
  unsigned int running_x = 0;
  for (RPG_Map_AreaConstIterator_t iterator = room_in.begin();
       iterator != room_in.end();
       iterator++)
  {
    max_breadth = 0;
    current_row = (*iterator).second;
    running_x = (*iterator).first;
    for (RPG_Map_AreaConstIterator_t iterator2 = iterator;
         iterator2 != room_in.end();
         iterator2++)
    {
      // still on the top row ?
      if ((*iterator2).second == ((*iterator).second))
      {
        // check: for consecutive positions WITHIN the same row:
        //        if (previous_cell.x + 1) is NOT EQUAL to the current.x
        //        i.e. the current cell is not adjacent to the previous one
        //        --> skip forward to the next row (and resume checking !)
        if (running_x != (*iterator2).first)
        {
          while ((iterator2 != room_in.end()) &&
                 ((*iterator2).second == current_row))
            iterator2++;

          // finished with this position ?
          if (iterator2 == room_in.end())
            break; // start next position

          // check next cell
          continue;
        } // end IF
        else
          max_breadth++; // current cell is adjacent to the last one
        running_x++; // adjacent cell must have this x-coordinate

        // --> compute enclosed area and continue
        current_area = area2Positions(*iterator, *iterator2);
        if (maxArea < current_area)
        {
          maxArea = current_area;
          maxSquare_out.ul = *iterator;
          maxSquare_out.lr = *iterator2;
        } // end IF

        // check next cell
        continue;
      } // end IF

      // we're NOT on the top row anymore...
      ACE_ASSERT((*iterator2).second != (*iterator).second);

      // step1: compute max. breadth for current position
      // --> has been done implicitely while processing the top row (see above)
      ACE_ASSERT(max_breadth);

      // check: IMMEDIATELY on breaking to a new row:
      //        if our position.x is LARGER than our current_square.ul.x
      //        --> already found the largest square for this position
      if ((*iterator2).second > current_row)
      {
        running_x = (*iterator2).first;
        current_row++;
        if ((*iterator2).second != current_row)
        {
          // this room is NOT compact (reason: cropped ?)
          // --> already found the largest square for this position
          break; // start next position
        } // end IF

        if ((*iterator2).first > (*iterator).first)
          break; // start next position

        // retrieve the last cell of the previous row
        pointer = iterator2;
        pointer--;

        // optimization: if last_cell.x of the previous row was SMALLER than
        //               current_square.ul.x
        //               --> already found the largest square for this position
        if ((*pointer).first < (*iterator).first)
          break; // start next position

        // otherwise, perhaps max_breadth needs a reduction...
        if ((((*pointer).first - (*iterator).first) + 1) < max_breadth)
          max_breadth = (((*pointer).first - (*iterator).first) + 1);
        ACE_ASSERT(max_breadth);
      } // end IF
      else
      {
        // as long as the current_cell.x is EQUAL OR SMALLER than
        // current_square.ul.x (i.e. we're left/at the reference position),
        // ignore any skips between consecutive positions (see below)
        if ((*iterator2).first <= (*iterator).first)
          running_x = (*iterator2).first;
        else
          running_x++;
      } // end ELSE

      // check: check if we're within the square spanned by
      //        [current_square.ul,(current_square.ul.x + (max_breadth - 1),y)]
      if (((*iterator2).first > ((*iterator).first + max_breadth - 1)) ||
          ((*iterator2).first < (*iterator).first))
        continue; // no, we're not --> check next cell

      // we're within the current square

      // check: for consecutive positions WITHIN the same row:
      //        if (previous_cell.x + 1) is NOT EQUAL to the current.x
      //        i.e. the current cell is not adjacent to the previous one
      //        --> already found the largest square for this position
      if (running_x != (*iterator2).first)
        break; // start next position

      // --> compute enclosed area
      current_area = area2Positions(*iterator, *iterator2);
      if (maxArea < current_area)
      {
        maxArea = current_area;
        maxSquare_out.ul = *iterator;
        maxSquare_out.lr = *iterator2;
      } // end IF
    } // end FOR
  } // end FOR
}

void
RPG_Map_Common_Tools::makeRooms(const unsigned int& dimensionX_in,
                                const unsigned int& dimensionY_in,
                                const RPG_Map_Partition_t& partition_in,
                                const bool& wantSquareRooms_in,
                                const bool& wantRoomSeparation_in,
                                const bool& cropAreas_in,
                                const bool& maximizeRooms_in,
                                const unsigned int& minRoomArea_in,
                                RPG_Map_AreaList_t& rooms_out,
                                RPG_Map_AreaList_t& boundaries_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::makeRooms"));

  // init return value(s)
  rooms_out.clear();
  boundaries_out.clear();

  // place a room into every partition...

  unsigned int index = 0;
  RPG_Map_Area_t current_area;

  // step0a: make some preparations (crop)...
  unsigned int last_size = 0;
  index = 0;
  for (RPG_Map_PartitionConstIterator_t partition_iter = partition_in.begin();
       partition_iter != partition_in.end();
       partition_iter++, index++)
  {
    current_area.clear();

    // make a copy...
    current_area = (*partition_iter).area;

    // --> remove any cruft from the perimeter ?
    if (cropAreas_in)
    {
      // *NOTE*: crop until the size is stable
      // i.e. if (min. breadth/width is < 3/3), the whole room will be cropped
      // --> too slim/flat...
      do
      {
        last_size = current_area.size();

        //displayRoom(dimensionX_in,
        //            dimensionY_in,
        //            current_zone);

        crop(current_area);
      } while (last_size > current_area.size());

//       if (current_zone.empty())
//       {
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("zone[%u] has been cropped...\n"),
//                    index));
//       } // end IF
//       else if (current_zone.size() < (*partition_iter).size())
//       {
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("zone[%u]: cropped %u position(s)...\n"),
//                    index,
//                    ((*partition_iter).size() - current_zone.size())));
//       } // end ELSE
    } // end IF

    rooms_out.push_back(current_area);
  } // end FOR

  RPG_Map_AreaListIterator_t arealist_iter;

  // step0b: make some preparations (determine max square)...
  RPG_Map_SquareList_t maxSquares;
  if (wantSquareRooms_in)
  {
    // --> make square room(s)
    RPG_Map_Square_t maxSquare;
    index = 0;
    for (arealist_iter = rooms_out.begin();
         arealist_iter != rooms_out.end();
         arealist_iter++, index++)
    {
      maxSquare.ul = std::make_pair(0, 0);
      maxSquare.lr = std::make_pair(0, 0);

      // sanity check
      if ((*arealist_iter).empty())
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("max. square[%u]: doesn't exist\n"),
                   index));
      } // end IF
      else
      {
        // find coordinates of the maximum-size square room(s)
        // --> can fit in smaller rooms anytime...
        findMaxSquare(*arealist_iter, maxSquare);

//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("max. square[%u]: [(%u,%u),(%u,%u)] - %u cell(s)\n"),
//                    index,
//                    maxSquare.ul.first,
//                    maxSquare.ul.second,
//                    maxSquare.lr.first,
//                    maxSquare.lr.second,
//                    area2Positions(maxSquare.ul, maxSquare.lr)));
      } // end ELSE

      maxSquares.push_back(maxSquare);
    } // end FOR
  } // end IF

  RPG_Map_AreaConstIterator_t area_iter;

  // step1: create rooms
  if (maximizeRooms_in)
  {
    if (wantSquareRooms_in)
    {
      index = 0;
      RPG_Map_SquareListConstIterator_t squares_iter = maxSquares.begin();
      for (arealist_iter = rooms_out.begin();
           arealist_iter != rooms_out.end();
           arealist_iter++, squares_iter++, index++)
      {
        current_area.clear();
        for (area_iter = (*arealist_iter).begin();
             area_iter != (*arealist_iter).end();
             area_iter++)
        {
          if (positionInSquare(*area_iter, *squares_iter))
            current_area.insert(*area_iter);
        } // end FOR

        // sanity check
        ACE_ASSERT((current_area.empty() ? true // that's OK (but area2Positions yields 1)...
                                         : (current_area.size() == area2Positions((*squares_iter).ul,
                                                                                  (*squares_iter).lr))));
        *arealist_iter = current_area; // do in-place editing...

//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("zone[%u] has %u cell(s) - bounded by [(%u,%u),(%u,%u)] --> %u cell(s)\n"),
//                    index,
//                    current_zone.size(),
//                    (*squares_iter).ul.first,
//                    (*squares_iter).ul.second,
//                    (*squares_iter).lr.first,
//                    (*squares_iter).lr.second,
//                    area2Positions((*squares_iter).ul,
//                                   (*squares_iter).lr)));
      } // end FOR
    } // end IF
    else
    {
      // --> the partitions ARE the rooms
      // *NOTE*: there will be NO corridor(s), only door(s)...
      // --> nothing to do...
    } // end ELSE
  } // end IF
  else
  {
    // *NOTE*: rooms still may have been cropped...
    // --> nothing to do...
  } // end ELSE

  // step2: enforce any other constraint(s)

  // minimal area
  if (minRoomArea_in)
  {
    index = 0;
    for (arealist_iter = rooms_out.begin();
         arealist_iter != rooms_out.end();
         arealist_iter++, index++)
    {
      if ((*arealist_iter).size() < minRoomArea_in)
      {
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("zone[%u] is too small (%u < %u)...\n"),
//                    index,
//                    (*zones_iter).size(),
//                    minArea_in));

        // crop room (what else can we do ?)
        (*arealist_iter).clear();
      } // end IF
    } // end FOR
  } // end IF

  if (wantRoomSeparation_in)
  {
    // enforce a minimal separation to fit in corridors_bounds
    RPG_Map_Position_t up, right, down, left;
    RPG_Map_Directions_t crop_walls;
    RPG_Map_AreaListIterator_t arealist_iter2;
    index = 0;
    for (arealist_iter = rooms_out.begin();
         arealist_iter != rooms_out.end();
         arealist_iter++, index++)
    {
      crop_walls.clear();
      for (area_iter = (*arealist_iter).begin();
           area_iter != (*arealist_iter).end();
           area_iter++)
      {
        // compute neighbours
        up = *area_iter; up.second -= ((up.second == 0) ? 0 : 1);
        right =
            *area_iter; right.first += ((right.first == (dimensionX_in - 1)) ? 0 : 1);
        down =
            *area_iter; down.second += ((down.second == (dimensionY_in - 1)) ? 0 : 1);
        left = *area_iter; left.first -= ((left.first == 0) ? 0 : 1);

        // (direct) contact with ANOTHER room ?
        for (arealist_iter2 = rooms_out.begin();
             arealist_iter2 != rooms_out.end();
             arealist_iter2++)
        {
          // skip the CURRENT room
          if (arealist_iter2 == arealist_iter)
            continue;

          // *NOTE*: if there is contact, resize the larger one...
          // *TODO*: if this would crop the entire room, we should try to crop
          // the smaller one instead...
          if ((*arealist_iter).size() < (*arealist_iter2).size())
            continue;

          if ((*arealist_iter2).find(up) != (*arealist_iter2).end())
            crop_walls.insert(DIRECTION_UP);
          if ((*arealist_iter2).find(right) != (*arealist_iter2).end())
            crop_walls.insert(DIRECTION_RIGHT);
          if ((*arealist_iter2).find(down) != (*arealist_iter2).end())
            crop_walls.insert(DIRECTION_DOWN);
          if ((*arealist_iter2).find(left) != (*arealist_iter2).end())
            crop_walls.insert(DIRECTION_LEFT);
        } // end FOR
      } // end FOR

      // if necessary, crop the room
      if (!crop_walls.empty())
      {
        RPG_Map_Position_t upper_left, upper_right, lower_left, lower_right;
        RPG_Map_AreaConstIterator_t first, last;
        std::vector<RPG_Map_AreaIterator_t> positions;
        std::vector<RPG_Map_AreaIterator_t>::const_iterator positions_iterator;
        unsigned int column_x;
        RPG_Map_AreaIterator_t area_iter2;
        for (RPG_Map_DirectionsConstIterator_t crop_iter = crop_walls.begin();
             crop_iter != crop_walls.end();
             crop_iter++)
        {
          positions.clear();
          switch (*crop_iter)
          {
            case DIRECTION_UP:
            {
              // erase top row
              first = (*arealist_iter).begin();
              last = (*arealist_iter).begin();
              while ((*last).second == (*first).second)
                last++;
              // *NOTE*: std::set::erase removes [first, last) !
              (*arealist_iter).erase(first, last);
//               // create new top row
//               first = last;
//               last++;
//               for (unsigned int i = 1;
//                    i <= ((*last).first - (*first).first);
//                    i++)
//                 (*arealist_iter).insert(std::make_pair(((*first).first + i), (*first).second));

//               ACE_DEBUG((LM_DEBUG,
//                          ACE_TEXT("area[%u]: cropped top row...\n"),
//                          index));

              break;
            }
            case DIRECTION_RIGHT:
            {
              // erase rightmost column
              last = (*arealist_iter).end(); last--;
              column_x = (*last).first;
              for (area_iter2 = (*arealist_iter).begin();
                   area_iter2 != (*arealist_iter).end();
                   area_iter2++)
                if ((*area_iter2).first == column_x)
                  positions.push_back(area_iter2);
              for (positions_iterator = positions.begin();
                   positions_iterator != positions.end();
                   positions_iterator++)
                (*arealist_iter).erase(*positions_iterator);
//               // create new rightmost column
//               first = (*arealist_iter).begin();
//               last = (*arealist_iter).end(); last--;
//               for (unsigned int i = 1;
//                    i <= ((*last).second - (*first).second);
//                    i++)
//                 (*arealist_iter).insert(std::make_pair((*last).first, ((*last).second - i)));

//               ACE_DEBUG((LM_DEBUG,
//                          ACE_TEXT("area[%u]: cropped rightmost column...\n"),
//                          index));

              break;
            }
            case DIRECTION_DOWN:
            {
              // erase bottom row
              first = (*arealist_iter).begin();
              last = (*arealist_iter).end(); last--;
              while ((*first).second != (*last).second)
                first++;
              // *NOTE*: std::set::erase removes [first, last) !
              (*arealist_iter).erase(first, (*arealist_iter).end());
//               // create new bottom row
//               last = (*arealist_iter).end(); last--;
//               first = last;
//               first--;
//               for (unsigned int i = 1;
//                    i <= ((*last).first - (*first).first);
//                    i++)
//                 (*arealist_iter).insert(std::make_pair(((*first).first + i), (*first).second));

//               ACE_DEBUG((LM_DEBUG,
//                          ACE_TEXT("area[%u]: cropped bottom row...\n"),
//                          index));

              break;
            }
            case DIRECTION_LEFT:
            {
              // erase leftmost column
              first = (*arealist_iter).begin();
              column_x = (*first).first;
              for (area_iter2 = (*arealist_iter).begin();
                   area_iter2 != (*arealist_iter).end();
                   area_iter2++)
                if ((*area_iter2).first == column_x)
                  positions.push_back(area_iter2);
              for (positions_iterator = positions.begin();
                   positions_iterator != positions.end();
                   positions_iterator++)
                (*arealist_iter).erase(*positions_iterator);
//               // create new leftmost column
//               first = (*arealist_iter).begin();
//               last = (*arealist_iter).end(); last--;
//               for (unsigned int i = 1;
//                    i <= ((*last).second - (*first).second);
//                    i++)
//                 (*arealist_iter).insert(std::make_pair((*first).first, ((*last).second - i)));

//               ACE_DEBUG((LM_DEBUG,
//                          ACE_TEXT("area[%u]: cropped leftmost column...\n"),
//                          index));

              break;
            }
            default:
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("invalid direction (was \"%s\"), continuing\n"),
                         ACE_TEXT(RPG_Map_DirectionHelper::RPG_Map_DirectionToString(*crop_iter).c_str())));

              ACE_ASSERT(false);

              break;
            }
          } // end SWITCH
        } // end FOR

        //displayRoom(dimensionX_in,
        //            dimensionY_in,
        //            *zones_iter);

        // (re-)crop the entire room...
//         cropSquareBoundary(*zones_iter);
        crop(*arealist_iter);

//         if ((*zones_iter).empty())
//         {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("zone[%u] has been cropped entirely...\n"),
//                      index));
//         } // end IF
//       else if ((*zones_iter).size() < (*partition_iter).size())
//       {
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("zone[%u]: cropped %u additional position(s)...\n"),
//                    index,
//                    ((*partition_iter).size() - current_zone.size())));
//       } // end ELSE
      } // end IF
    } // end FOR
  } // end IF

  // step3: compute the perimeter
  RPG_Map_Positions_t positions;
  for (arealist_iter = rooms_out.begin();
       arealist_iter != rooms_out.end();
       arealist_iter++)
  {
    current_area.clear();
    perimeter(*arealist_iter, positions);
    current_area.insert(positions.begin(), positions.end());
    boundaries_out.push_back(current_area);
  } // end FOR
}

void
RPG_Map_Common_Tools::displayRooms(const unsigned int& dimensionX_in,
                                   const unsigned int& dimensionY_in,
                                   const RPG_Map_AreaList_t& rooms_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::displayRooms"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("room(s)...\n")));

  RPG_Map_Position_t current_position;

  unsigned int index = 0;
  std::ostringstream converter;
  bool found_position = false;
  for (unsigned int y = 0;
       y < dimensionY_in;
       y++)
  {
    for (unsigned int x = 0;
         x < dimensionX_in;
         x++)
    {
      current_position = std::make_pair(x, y);
      index = 0;
      found_position = false;
      for (RPG_Map_AreaListConstIterator_t iter = rooms_in.begin();
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
      std::clog << '.';
    } // end FOR
    std::clog << std::endl;
  } // end FOR
}

void
RPG_Map_Common_Tools::displayRoom(const unsigned int& dimensionX_in,
                                  const unsigned int& dimensionY_in,
                                  const RPG_Map_Area_t& room_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::displayRoom"));

  RPG_Map_AreaConstIterator_t iterator = room_in.end();
  for (unsigned int y = 0;
       y < dimensionY_in;
       y++)
  {
    for (unsigned int x = 0;
         x < dimensionX_in;
         x++)
    {
      iterator = room_in.find(std::make_pair(x, y));
      if (iterator != room_in.end())
        std::clog << '#';
      else
        std::clog << '.';
    } // end FOR
    std::clog << std::endl;
  } // end FOR
}

void
RPG_Map_Common_Tools::makeDoors(const unsigned int& dimensionX_in,
                                const unsigned int& dimensionY_in,
                                const RPG_Map_AreaList_t& boundaries_in,
                                const bool& doorFillsPosition_in,
                                const unsigned int& maxDoorsPerRoom_in,
                                RPG_Map_AreaList_t& doors_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::makeDoors"));

  // init return value(s)
  doors_out.clear();

  RPG_Map_AreaListConstIterator_t zonelist_iter;
  // *NOTE*: every room needs at least one (possibly secret) door
  // *NOTE*: doors cannot be situated on the boundary of the level
  // *NOTE*: doors can connect rooms directly (i.e. without a corridor)
  // *NOTE*: to ensure connectivity for n rooms, at least (n-1)*2 doors are
  //         needed...
  unsigned int total_doors = 0;
  unsigned int index = 0;

  RPG_Map_Area_t current_doors;
  unsigned int num_doors = 0;
  RPG_Dice_RollResult_t result;
  RPG_Map_PositionList_t doorPositions;
  RPG_Map_PositionListIterator_t doorPosition_iterator;
  while (total_doors < ((boundaries_in.size() - 1) * 2))
  {
    doors_out.clear();
    total_doors = 0;
    index = 0;
    for (zonelist_iter = boundaries_in.begin();
         zonelist_iter != boundaries_in.end();
         zonelist_iter++, index++)
    {
      current_doors.clear();

      // step0: sanity check(s)
      if ((*zonelist_iter).empty())
      {
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("room[%u] has no walls...\n"),
//                    index));

        // still, insert an (empty) set of doors...
        doors_out.push_back(current_doors);

        continue;
      } // end IF

      // step1: find suitable positions
      doorPositions.clear();
      findDoorPositions(*zonelist_iter,
                        doorFillsPosition_in,
                        doorPositions);
      // check: to be useful, doors must face "inwards"
      // --> remove all positions on the perimeter of the level
      doorPosition_iterator = doorPositions.begin();
      while (doorPosition_iterator != doorPositions.end())
      {
        if (((*doorPosition_iterator).first == 0) ||
            ((*doorPosition_iterator).first == (dimensionX_in - 1)) ||
            ((*doorPosition_iterator).second == 0) ||
            ((*doorPosition_iterator).second == (dimensionY_in - 1)))
          doorPosition_iterator = doorPositions.erase(doorPosition_iterator);
        else
          doorPosition_iterator++;
      } // end WHILE
      if (doorPositions.empty())
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("room[%u] cannot have doors, continuing\n"),
                   index));
        // debug info
        dump(*zonelist_iter);

        // still, insert an (empty) set of doors...
        doors_out.push_back(current_doors);

        continue;
      } // end IF

      // step2: generate (random) number of doors
      result.clear();
      if (maxDoorsPerRoom_in == std::numeric_limits<unsigned int>::max())
      {
        // for debugging purposes...
        num_doors = doorPositions.size();
        for (doorPosition_iterator = doorPositions.begin();
             doorPosition_iterator != doorPositions.end();
             doorPosition_iterator++)
          current_doors.insert(*doorPosition_iterator);
      } // end IF
      else
      {
        RPG_Dice::generateRandomNumbers((maxDoorsPerRoom_in ? ((maxDoorsPerRoom_in > doorPositions.size()) ? doorPositions.size()
                                                                                                           : maxDoorsPerRoom_in)
                                                            : doorPositions.size()),
                                        1,
                                        result);
        num_doors = result[0];
        // step3: choose num_doors from available positions
        while (current_doors.size() < num_doors)
        {
          result.clear();
          RPG_Dice::generateRandomNumbers(doorPositions.size(),
                                          1,
                                          result);
          doorPosition_iterator = doorPositions.begin();
          std::advance(doorPosition_iterator, result[0] - 1);

          current_doors.insert(*doorPosition_iterator);
    //       if (doorFillsPosition_in)
    //         (*zonelist_iter).erase(*doorPosition_iterator); // not a wall anymore...
        } // end WHILE
      } // end ELSE

      doors_out.push_back(current_doors);
      total_doors += num_doors;
    } // end FOR
  } // end WHILE
}

void
RPG_Map_Common_Tools::connectRooms(const unsigned int& dimensionX_in,
                                   const unsigned int& dimensionY_in,
                                   const RPG_Map_AreaList_t& boundaries_in,
                                   const RPG_Map_AreaList_t& doors_in,
                                   const RPG_Map_AreaList_t& rooms_in,
                                   RPG_Map_FloorPlan_t& level_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::connectRooms"));

  // init return value(s)
//   level_out.size_x = dimensionX_in;
//   level_out.size_y = dimensionY_in;
  level_out.unmapped.clear();
  level_out.walls.clear();
  level_out.doors.clear();

  RPG_Map_AreaListConstIterator_t zonelist_iter;
  RPG_Map_AreaConstIterator_t zone_iter;

  // step0: add room walls
  for (zonelist_iter = boundaries_in.begin();
       zonelist_iter != boundaries_in.end();
       zonelist_iter++)
    for (zone_iter = (*zonelist_iter).begin();
         zone_iter != (*zonelist_iter).end();
         zone_iter++)
      level_out.walls.insert(*zone_iter);

  // step1: add the doors
//   unsigned int index = 0;
//   unsigned int index2 = 0;
  RPG_Map_Door_t door;
  door.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                 std::numeric_limits<unsigned int>::max());
  door.outside = RPG_MAP_DIRECTION_INVALID;
  door.state = DOORSTATE_CLOSED; // *TODO*
  RPG_Map_AreaConstIterator_t doors_iterator;
  RPG_Map_AreaListConstIterator_t rooms_iter;
  rooms_iter = rooms_in.begin();
  for (zonelist_iter = doors_in.begin();
       zonelist_iter != doors_in.end();
       zonelist_iter++, rooms_iter++/*, index++*/)
  {
//     index2 = 0;
    for (doors_iterator = (*zonelist_iter).begin();
         doors_iterator != (*zonelist_iter).end();
         doors_iterator++/*, index2++*/)
    {
      door.position = *doors_iterator;
      door.outside = door2exitDirection(*rooms_iter,
                                        door.position);
      level_out.doors.insert(door);

//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("door[%u,%u] at (%u,%u)\n"),
//                  index, index2,
//                  (*doors_iterator).first,
//                  (*doors_iterator).second));
    } // end FOR
  } // end FOR

  // step2: make corridors
  RPG_Map_PositionsList_t corridors_bounds;
  RPG_Map_AreaListConstIterator_t zonelist_iter2;
  RPG_Map_AreaConstIterator_t doors_iter, doors_iter2;
  RPG_Map_Positions_t current_corridor;
  RPG_Map_Positions_t possible_positions, used_positions;
  RPG_Dice_RollResult_t result;
  RPG_Map_PositionsConstIterator_t target_door;
  RPG_Map_Path_t current_path;
  RPG_Map_PathConstIterator_t path_iter, last;
  RPG_Map_PathList_t paths;
  bool connectivity_established = false;
  bool path_already_established = false;
  rooms_iter = rooms_in.begin();
  for (zonelist_iter = doors_in.begin();
       zonelist_iter != doors_in.end();
       zonelist_iter++, rooms_iter++)
    for (doors_iter = (*zonelist_iter).begin();
         doors_iter != (*zonelist_iter).end();
         doors_iter++)
    {
      // already connected ?
      if (used_positions.find(*doors_iter) != used_positions.end())
        continue;

      current_path.clear();
      current_corridor.clear();
      possible_positions.clear();

      // step1: (if possible) choose a target door from a different room
      for (zonelist_iter2 = doors_in.begin();
           zonelist_iter2 != doors_in.end();
           zonelist_iter2++)
        for (doors_iter2 = (*zonelist_iter2).begin();
             doors_iter2 != (*zonelist_iter2).end();
             doors_iter2++)
          if (zonelist_iter2 == zonelist_iter)
            continue;
          else if (used_positions.find(*doors_iter2) == used_positions.end())
            possible_positions.insert(*doors_iter2);
      if (possible_positions.empty())
      {
        // no more (relevant) options for this room (!)
        // --> try again, ignore used positions (i.e. introduce some redundancy)
        for (zonelist_iter2 = doors_in.begin();
             zonelist_iter2 != doors_in.end();
             zonelist_iter2++)
          for (doors_iter2 = (*zonelist_iter2).begin();
               doors_iter2 != (*zonelist_iter2).end();
               doors_iter2++)
            if (zonelist_iter2 == zonelist_iter)
              continue;
            else
              possible_positions.insert(*doors_iter2);
      } // end IF
      result.clear();
      RPG_Dice::generateRandomNumbers(possible_positions.size(),
                                      1,
                                      result);
      target_door = possible_positions.begin();
      std::advance(target_door, result[0] - 1);

      // check whether this connection has already been established
      // i.e. in the opposite direction
      path_already_established = false;
      for (RPG_Map_PathListConstIterator_t paths_iter = paths.begin();
           paths_iter != paths.end();
           paths_iter++)
      {
        last = (*paths_iter).end(); last--;
        if (((*last).first == *doors_iter) &&
            ((*((*paths_iter).begin())).first == *target_door))
        {
          path_already_established = true;
          break;
        } // end IF
      } // end FOR
      if (path_already_established)
        continue; // nothing to do...

      // step2: find a path from one door to the other

      // *NOTE*: determine the starting direction in order
      // to "leave" the room immediately...
      RPG_Map_Pathfinding_Tools::findPath(std::make_pair(dimensionX_in,
                                                         dimensionY_in),
                                          level_out.walls,
                                          *doors_iter,
                                          door2exitDirection(*rooms_iter,
                                                             *doors_iter),
                                          *target_door,
                                          current_path);
      if (current_path.size() < 2)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("cannot find path [%u,%u] --> [%u,%u], continuing\n"),
                   (*doors_iter).first,
                   (*doors_iter).second,
                   (*target_door).first,
                   (*target_door).second));

        continue;
      } // end IF
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("found path from (%u,%u) to (%u,%u)\n"),
//                  (*doors_iter).first,
//                  (*doors_iter).second,
//                  (*target_door).first,
//                  (*target_door).second));

      // minimal connectivity ?
      if (!connectivity_established)
      {
        used_positions.insert(*doors_iter);
        used_positions.insert(*target_door);

        // connectivity established ? --> relax constraints
        if (used_positions.size() == level_out.doors.size())
        {
          // check: when minimal connectivity has been reached (i.e. every door
          //        has been attached to some corridor), relax this constraint
          used_positions.clear();

//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("connectivity has been established (%u path(s))...\n"),
//                      (paths.size() + 1)));

          connectivity_established = true;
        } // end IF
      } // end IF

      // step3: create a corridor along this path
      RPG_Map_Common_Tools::buildCorridor(current_path,
                                          current_corridor);
      corridors_bounds.push_back(current_corridor);
      paths.push_back(current_path);
    } // end FOR

  // step3: throw everything together
  RPG_Map_PositionsListIterator_t corridors_iter;
  for (corridors_iter = corridors_bounds.begin();
       corridors_iter != corridors_bounds.end();
       corridors_iter++)
       for (RPG_Map_PositionsConstIterator_t positions_iter = (*corridors_iter).begin();
            positions_iter != (*corridors_iter).end();
            positions_iter++)
        level_out.walls.insert(*positions_iter);

  // step4
  // - clear any rubble that may now block some corridors
  // - compute unmapped areas
  RPG_Map_PositionsList_t corridors = corridors_bounds;
  corridors_iter = corridors.begin();
  for (RPG_Map_PathListConstIterator_t paths_iter = paths.begin();
       paths_iter != paths.end();
       paths_iter++, corridors_iter++)
    for (RPG_Map_PathConstIterator_t path_iter = (*paths_iter).begin();
         path_iter != (*paths_iter).end();
         path_iter++)
    {
      if (level_out.walls.find((*path_iter).first) != level_out.walls.end())
      {
        level_out.walls.erase((*path_iter).first);

//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("cleared rubble at (%u,%u)...\n"),
//                    (*path_iter).first.first,
//                    (*path_iter).first.second));
      } // end IF

      // add position to current corridor
      (*corridors_iter).insert((*path_iter).first);
    } // end FOR

//   displayCorridors(dimensionX_in,
//                    dimensionY_in,
//                    rooms_in,
//                    doors_in,
//                    corridors);

  // step5: remove the walls corresponding to doors
  for (zonelist_iter = doors_in.begin();
       zonelist_iter != doors_in.end();
       zonelist_iter++)
    for (doors_iterator = (*zonelist_iter).begin();
         doors_iterator != (*zonelist_iter).end();
         doors_iterator++)
      level_out.walls.erase(*doors_iterator);

  // step6: compute unmapped areas
  RPG_Map_Position_t current_position;
  bool done = false;
  for (unsigned int y = 0;
       y < dimensionY_in;
       y++)
    for (unsigned int x = 0;
         x < dimensionX_in;
         x++)
    {
      current_position = std::make_pair(x, y);

      done = false;
      // part of a room ?
      for (zonelist_iter = rooms_in.begin();
           zonelist_iter != rooms_in.end();
           zonelist_iter++)
        if ((*zonelist_iter).find(current_position) != (*zonelist_iter).end())
        {
          done = true;
          break;
        } // end IF
      if (done)
        continue;

      // part of a corridor ?
      for (corridors_iter = corridors.begin();
           corridors_iter != corridors.end();
           corridors_iter++)
        if ((*corridors_iter).find(current_position) != (*corridors_iter).end())
        {
          done = true;
          break;
        } // end IF
      if (done)
        continue;

      // --> unmapped
      level_out.unmapped.insert(current_position);
    } // end FOR
}

void
RPG_Map_Common_Tools::buildCorridor(const RPG_Map_Path_t& path_in,
                                    RPG_Map_Positions_t& corridor_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::buildCorridor"));

  // init return value
  corridor_out.clear();

  RPG_Map_Position_t wall_position_1, wall_position_2, wall_position_3;

  // *NOTE*: paths extend from door to door...
  RPG_Map_Direction  last_direction = (*path_in.begin()).second;
  RPG_Map_PathConstIterator_t path_iter, last;
  path_iter = path_in.begin(); path_iter++;
  last = path_in.end(); last--;
  for (;
       path_iter != last;
       path_iter++)
  {
    // *NOTE*: on a CHANGE of direction add walls:
    // - in the direction of LAST travel
    // - in the OPPOSITE direction of CURRENT travel
    // - in the CORNER between these two
    // else, add walls flanking the CURRENT direction of travel on both sides
    if ((*path_iter).second != last_direction)
    {
      wall_position_1 = (*path_iter).first;
      switch (last_direction)
      {
        case DIRECTION_UP:
        {
          wall_position_1.second--;
          wall_position_3 = wall_position_1;
          if ((*path_iter).second == DIRECTION_RIGHT)
            wall_position_3.first--;
          else
            wall_position_3.first++;

          break;
        }
        case DIRECTION_DOWN:
        {
          wall_position_1.second++;
          wall_position_3 = wall_position_1;
          if ((*path_iter).second == DIRECTION_RIGHT)
            wall_position_3.first--;
          else
            wall_position_3.first++;

          break;
        }
        case DIRECTION_RIGHT:
        {
          wall_position_1.first++;
          wall_position_3 = wall_position_1;
          if ((*path_iter).second == DIRECTION_UP)
            wall_position_3.second++;
          else
            wall_position_3.second--;

          break;
        }
        case DIRECTION_LEFT:
        {
          wall_position_1.first--;
          wall_position_3 = wall_position_1;
          if ((*path_iter).second == DIRECTION_UP)
            wall_position_3.second++;
          else
            wall_position_3.second--;

          break;
        }
        case RPG_MAP_DIRECTION_INVALID:
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid direction (was \"%s\"), continuing\n"),
                     ACE_TEXT(RPG_Map_DirectionHelper::RPG_Map_DirectionToString(last_direction).c_str())));

          break;
        }
      } // end SWITCH
      wall_position_2 = (*path_iter).first;
      switch ((*path_iter).second)
      {
        case DIRECTION_UP:
          wall_position_2.second++; break;
        case DIRECTION_DOWN:
          wall_position_2.second--; break;
        case DIRECTION_RIGHT:
          wall_position_2.first--; break;
        case DIRECTION_LEFT:
          wall_position_2.first++; break;
        case RPG_MAP_DIRECTION_INVALID:
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid direction (was \"%s\"), continuing\n"),
                     ACE_TEXT(RPG_Map_DirectionHelper::RPG_Map_DirectionToString((*path_iter).second).c_str())));

          break;
        }
      } // end SWITCH
    } // end IF
    else
    {
      wall_position_1 = (*path_iter).first;
      wall_position_2 = (*path_iter).first;
      switch ((*path_iter).second)
      {
        case DIRECTION_UP:
        case DIRECTION_DOWN:
        {
          wall_position_1.first--;
          wall_position_2.first++;

          break;
        }
        case DIRECTION_RIGHT:
        case DIRECTION_LEFT:
        {
          wall_position_1.second--;
          wall_position_2.second++;

          break;
        }
        case RPG_MAP_DIRECTION_INVALID:
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid direction (was \"%s\"), continuing\n"),
                     ACE_TEXT(RPG_Map_DirectionHelper::RPG_Map_DirectionToString((*path_iter).second).c_str())));

          break;
        }
      } // end SWITCH
    } // end ELSE
    corridor_out.insert(wall_position_1);
    corridor_out.insert(wall_position_2);
    if ((*path_iter).second != last_direction)
      corridor_out.insert(wall_position_3);
    last_direction = (*path_iter).second;
  } // end FOR
}

void
RPG_Map_Common_Tools::buildSquare(const RPG_Map_Position_t& center_in,
                                  const RPG_Map_Size_t& size_in,
                                  const unsigned int& radius_in,
                                  const bool& fillArea_in,
                                  RPG_Map_Positions_t& area_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::buildSquare"));

  // init result(s)
  area_out.clear();

  unsigned int left_x, x, y, i;
  bool done;
  for (unsigned int r = 0;
       r <= radius_in;
       r++)
  {
    if (!fillArea_in &&
        (r != radius_in))
      continue;

    if (r == 0)
    {
      area_out.insert(center_in);
      continue;
    } // end IF

    // compute leftmost x
    left_x = center_in.first;
    left_x -= ((left_x == 0) ? 0 : ((left_x < r) ? left_x : r));
    x = left_x;
    // --> up
    done = false;
    for (i = 0, y = center_in.second;
         i <= r;
         i++, y--)
    {
      if (y == 0)
        done = true;
      
      area_out.insert(std::make_pair(x, y));

      if (done)
        break;
    } // end FOR
    // --> right
    if (done)
      done = false;
    else
      y++;
    for (x++;
         x <= (center_in.first + r); // *WARNING*: no overflow checking here !
         x++)
    {
      if (x == (size_in.first - 1))
        done = true;
      
      area_out.insert(std::make_pair(x, y));

      if (done)
        break;
    } // end FOR
    // --> down
    if (done)
      done = false;
    else
      x--;
    for (y++;
         y <= (center_in.second + r); // *WARNING*: no overflow checking here !
         y++)
    {
      if (y == (size_in.second - 1))
        done = true;
      
      area_out.insert(std::make_pair(x, y));

      if (done)
        break;
    } // end FOR
    // --> left
    if (done)
      done = false;
    else
      y--;
    for (x--;
         x >= left_x;
         x--)
    {
      if (x == 0)
        done = true;
      
      area_out.insert(std::make_pair(x, y));

      if (done)
        break;
    } // end FOR
    // --> up
    if (done)
      done = false;
    else
      x++;
    for (i = 0, y--;
         i <= (r - 1);
         i++, y--)
    {
      if (y == (center_in.second + 1))
        done = true;

      area_out.insert(std::make_pair(x, y));

      if (done)
        break;
    } // end FOR
    if (done)
    {
      done = false;
      y--;
    } // end IF
    else
      y++;
    ACE_ASSERT(y == center_in.second);
  } // end FOR
}

void
RPG_Map_Common_Tools::buildCircle(const RPG_Map_Position_t& center_in,
                                  const RPG_Map_Size_t& size_in,
                                  const unsigned int& radius_in,
                                  const bool& fillArea_in,
                                  RPG_Map_Positions_t& area_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::buildCircle"));

  // init result(s)
  area_out.clear();

  // sanity check(s)
  if (radius_in == 0)
  {
    area_out.insert(center_in);

    return;
  } // end IF
  else if (radius_in > RPG_MAP_CIRCLE_MAX_RADIUS)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid radius (was: %u), aborting\n"),
               radius_in));
    ACE_ASSERT(false);

    return;
  } // end IF

  // *CONSIDER*: calculating the positions by mirroring the positions of
  // just one quadrant...

  unsigned char* circle_radius_p =
      RPG_Map_CircleData + RPG_Map_CircleStart[radius_in];
  unsigned int i, j, current_y = center_in.second;
  // *WARNING*: no bounds are checked right-/downwards...
  // first quadrant (CCW)
  for (i = 0;
       ((i <= radius_in) &&
        (current_y != std::numeric_limits<unsigned int>::max()));
       i++, current_y--)
  {
    if (fillArea_in)
      for (j = 0;
           j < *(circle_radius_p + i);
           j++)
        area_out.insert(std::make_pair(center_in.first + j, current_y));
    area_out.insert(std::make_pair(center_in.first + *(circle_radius_p + i),
                                   current_y));
  } // end FOR
  i--;
  current_y++;
  if (!fillArea_in)
    for (j = *(circle_radius_p + i) - 1;
         j != std::numeric_limits<unsigned int>::max();
         j--)
      area_out.insert(std::make_pair(center_in.first + j, current_y));
  // second quadrant (CCW)
  if (!fillArea_in)
    for (j = 1;
         ((j < *(circle_radius_p + i)) &&
          ((center_in.first - j) != std::numeric_limits<unsigned int>::max()));
         j++)
      area_out.insert(std::make_pair(center_in.first - j, current_y));
  for (;
       i != std::numeric_limits<unsigned int>::max();
       i--, current_y++)
  {
    if (fillArea_in)
      for (j = 1;
           ((j < *(circle_radius_p + i)) &&
            ((center_in.first - j) !=
             std::numeric_limits<unsigned int>::max()));
           j++)
        area_out.insert(std::make_pair(center_in.first - j, current_y));
    // *NOTE*: edge-clamp (left) here
    area_out.insert(std::make_pair((*(circle_radius_p + i) <= center_in.first) ? (center_in.first - *(circle_radius_p + i))
                                                                               : 0,
                                   current_y));
  } // end FOR
  ACE_ASSERT((current_y == (center_in.second + 1)) &&
             (i == std::numeric_limits<unsigned int>::max()));
  // third quadrant (CCW)
  for (i = 1;
       i <= radius_in;
       i++, current_y++)
  {
    if (fillArea_in)
      for (j = 1;
           ((j < *(circle_radius_p + i)) &&
            ((center_in.first - j) !=
             std::numeric_limits<unsigned int>::max()));
           j++)
        area_out.insert(std::make_pair(center_in.first - j, current_y));
    // *NOTE*: edge-clamp (left) here
    area_out.insert(std::make_pair((*(circle_radius_p + i) <= center_in.first) ? (center_in.first - *(circle_radius_p + i))
                                                                               : 0,
                                   current_y));
  } // end FOR
  i--;
  current_y--;
  ACE_ASSERT(current_y == (center_in.second + radius_in));
  if (!fillArea_in)
    for (j = ((*(circle_radius_p + i) <= center_in.first) ? (*(circle_radius_p + i) - 1)
                                                          : center_in.first);
         j >= 1;
         j--)
      area_out.insert(std::make_pair(center_in.first - j, current_y));
  // fourth quadrant (CCW)
  if (!fillArea_in)
    for (j = 0;
         j < *(circle_radius_p + i);
         j++)
      area_out.insert(std::make_pair(center_in.first + j, current_y));
  for (;
       i > 0;
       i--, current_y--)
  {
    if (fillArea_in)
      for (j = 0;
           j < *(circle_radius_p + i);
           j++)
        area_out.insert(std::make_pair(center_in.first + j, current_y));
    area_out.insert(std::make_pair(center_in.first + *(circle_radius_p + i),
                                   current_y));
  } // end FOR
  ACE_ASSERT((i == 0) &&
             (current_y == center_in.second));
}

void
RPG_Map_Common_Tools::dump(const RPG_Map_Area_t& zone_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::dump"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("zone (%u position(s)):\n"),
             zone_in.size()));
  unsigned int index = 0;
  for (RPG_Map_AreaConstIterator_t zone_iterator = zone_in.begin();
       zone_iterator != zone_in.end();
       zone_iterator++, index++)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("position[%u] at (%u,%u)\n"),
               index,
               (*zone_iterator).first,
               (*zone_iterator).second));
  } // end FOR
}

unsigned int
RPG_Map_Common_Tools::distance(const RPG_Map_Position_t& position1_in,
                               const RPG_Map_Position_t& position2_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::distance"));

  return (::abs(static_cast<int>(position1_in.first)  -
                static_cast<int>(position2_in.first)) +
          ::abs(static_cast<int>(position1_in.second) -
                static_cast<int>(position2_in.second)));
}

unsigned int
RPG_Map_Common_Tools::distanceMax(const RPG_Map_Position_t& position1_in,
                                  const RPG_Map_Position_t& position2_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::distanceMax"));

  unsigned int distance_1 = ::abs(static_cast<int>(position1_in.first)  -
                                  static_cast<int>(position2_in.first));
  unsigned int distance_2 = ::abs(static_cast<int>(position1_in.second) -
                                  static_cast<int>(position2_in.second));

  return ((distance_1 > distance_2) ? distance_1 : distance_2);
}

bool
RPG_Map_Common_Tools::isAdjacent(const RPG_Map_Position_t& position1_in,
                                 const RPG_Map_Position_t& position2_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::isAdjacent"));

  return (distanceMax(position1_in, position2_in) == 1);
}

std::string
RPG_Map_Common_Tools::orientation2String(const RPG_Map_Orientation& orientation_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::orientation2String"));

  // init result value
  std::string result = ACE_TEXT_ALWAYS_CHAR("MAP_ORIENTATION_INVALID");

  switch (orientation_in)
  {
    case MAP_ORIENTATION_HORIZONTAL:
      result = ACE_TEXT_ALWAYS_CHAR("MAP_ORIENTATION_HORIZONTAL"); break;
    case MAP_ORIENTATION_VERTICAL:
      result = ACE_TEXT_ALWAYS_CHAR("MAP_ORIENTATION_VERTICAL"); break;
    case MAP_ORIENTATION_INVALID:
      break;
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid orientation (was %d), aborting\n"),
                 orientation_in));

      ACE_ASSERT(false);

      break;
    }
  } // end SWITCH

  return result;
}

std::string
RPG_Map_Common_Tools::map2String(const RPG_Map_t& map_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::map2String"));

  std::string result;

  RPG_Map_PositionsConstIterator_t iterator;
  RPG_Map_Position_t current_position;
  RPG_Map_Door_t door_dummy;
  for (unsigned int y = 0;
       y < map_in.plan.size_y;
       y++)
  {
    for (unsigned int x = 0;
         x < map_in.plan.size_x;
         x++)
    {
      current_position = std::make_pair(x, y);
      if (map_in.start == current_position)
        result += 'X';
      else if (map_in.seeds.find(current_position) != map_in.seeds.end())
        result += '@';
      else if (map_in.plan.unmapped.find(current_position) !=
               map_in.plan.unmapped.end())
        result += ' ';
      else if (map_in.plan.walls.find(current_position) !=
               map_in.plan.walls.end())
        result += '#';
      else
      {
        door_dummy.position = current_position;
        if (map_in.plan.doors.find(door_dummy) != map_in.plan.doors.end())
          result += '=';
        else
          result += '.';
      } // end ELSE
    } // end FOR
    result += '\n';
  } // end FOR

  return result;
}

std::string
RPG_Map_Common_Tools::mapElement2String(const RPG_Map_Element& element_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::map2String"));

  std::string result = ACE_TEXT_ALWAYS_CHAR("MAPELEMENT_INVALID");

  switch (element_in)
  {
    case MAPELEMENT_UNMAPPED:
      result = ACE_TEXT_ALWAYS_CHAR("MAPELEMENT_UNMAPPED"); break;
    case MAPELEMENT_FLOOR:
      result = ACE_TEXT_ALWAYS_CHAR("MAPELEMENT_FLOOR"); break;
    case MAPELEMENT_STAIRS:
      result = ACE_TEXT_ALWAYS_CHAR("MAPELEMENT_STAIRS"); break;
    case MAPELEMENT_WALL:
      result = ACE_TEXT_ALWAYS_CHAR("MAPELEMENT_WALL"); break;
    case MAPELEMENT_DOOR:
      result = ACE_TEXT_ALWAYS_CHAR("MAPELEMENT_DOOR"); break;
    case MAPELEMENT_INVALID:
      break;
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid element (was %d), aborting\n"),
                 element_in));

      ACE_ASSERT(false);

      break;
    }
  } // end SWITCH

  return result;
}

RPG_Map_Direction
RPG_Map_Common_Tools::door2exitDirection(const RPG_Map_Position_t& position_in, // door
                                         const RPG_Map_FloorPlan_t& floorPlan_in)
{
  // *NOTE*: algorithm is as follows:
  // - perform two "flood-fill"s just beyond the given position - one INSIDE
  //   and one OUTSIDE
  // - iff (!) this floor plan has SQUARE rooms:
  //   - if either (!) area is NON-SQUARE, it's OUTSIDE
  //   --> both areas are SQUARE/NON-SQUARE: the SMALLER area determines INSIDE
  // --> else, this floor plan MAY have NON-SQUARE rooms:
  //   - compare # of adjacent doors of both resulting areas (i.e. >= 1)
  //   - iff (!) either (!) area has only a SINGLE DOOR, it's INSIDE
  //   --> BOTH areas have more than a SINGLE DOOR:
  //   [--> *NOTE*: at this point, we start guessing !]
  //     - if one area is SQUARE and the other is NON-SQUARE, the NON-SQUARE one
  //       is OUTSIDE
  //     - else (BOTH areas are SQUARE-SHAPED or NON-SQUARE-SHAPED):
  //     --> the SMALLER area is INSIDE

  // sanity check(s)
  RPG_Map_Door_t position_door;
  position_door.position = position_in;
  RPG_Map_DoorsIterator_t iterator = floorPlan_in.doors.find(position_door);
  if (iterator == floorPlan_in.doors.end())
    return RPG_MAP_DIRECTION_INVALID;

  // determine orientation
  RPG_Map_Position_t east;
  RPG_Map_Orientation orientation = MAP_ORIENTATION_INVALID;
  east = position_in;
  east.first++;
  if (floorPlan_in.walls.find(east) != floorPlan_in.walls.end())
    orientation = MAP_ORIENTATION_HORIZONTAL;
  else
    orientation = MAP_ORIENTATION_VERTICAL;

  RPG_Map_Position_t position_1, position_2;
  position_1 = position_in; position_2 = position_in;
  switch (orientation)
  {
    case MAP_ORIENTATION_HORIZONTAL:
      position_1.second--; position_2.second++; break;
    case MAP_ORIENTATION_VERTICAL:
      position_1.first--; position_2.first++; break;
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid orientation (was: \"%s\"), aborting\n"),
                 ACE_TEXT(RPG_Map_Common_Tools::orientation2String(orientation).c_str())));
      ACE_ASSERT(false);
      
      return RPG_MAP_DIRECTION_INVALID;
    }
  } // end SWITCH

  RPG_Map_Positions_t area_1, area_2;
  RPG_Map_Common_Tools::floodFill(position_1,
                                  floorPlan_in,
                                  area_1);
  RPG_Map_Common_Tools::floodFill(position_2,
                                  floorPlan_in,
                                  area_2);
  bool area_1_is_square, area_2_is_square;
  area_1_is_square = RPG_Map_Common_Tools::isSquare(area_1);
  area_2_is_square = RPG_Map_Common_Tools::isSquare(area_2);
  if (floorPlan_in.rooms_are_square)
  {
    if ((!area_1_is_square && area_2_is_square) ||
        (area_1_is_square && !area_2_is_square))
    {
compare_shape:
      switch (orientation)
      {
        case MAP_ORIENTATION_HORIZONTAL:
          return (area_1_is_square ? DIRECTION_DOWN : DIRECTION_UP);
        case MAP_ORIENTATION_VERTICAL:
          return (area_1_is_square ? DIRECTION_RIGHT : DIRECTION_LEFT);
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid orientation (was: \"%s\"), aborting\n"),
                     ACE_TEXT(RPG_Map_Common_Tools::orientation2String(orientation).c_str())));
          ACE_ASSERT(false);

          return RPG_MAP_DIRECTION_INVALID;
        }
      } // end SWITCH
    } // end IF

compare_size:
    switch (orientation)
    {
      case MAP_ORIENTATION_HORIZONTAL:
        return (area_1.size() <= area_2.size() ? DIRECTION_DOWN : DIRECTION_UP);
      case MAP_ORIENTATION_VERTICAL:
        return (area_1.size() <= area_2.size() ? DIRECTION_RIGHT
                                               : DIRECTION_LEFT);
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid orientation (was: \"%s\"), aborting\n"),
                   ACE_TEXT(RPG_Map_Common_Tools::orientation2String(orientation).c_str())));
        ACE_ASSERT(false);

        return RPG_MAP_DIRECTION_INVALID;
      }
    } // end SWITCH
  } // end IF

  // --> this floor plan MAY have NON-SQUARE rooms: (see above)
  unsigned int area_1_num_doors, area_2_num_doors;
  area_1_num_doors = RPG_Map_Common_Tools::countAdjacentDoors(area_1,
                                                              floorPlan_in);
  area_2_num_doors = RPG_Map_Common_Tools::countAdjacentDoors(area_2,
                                                              floorPlan_in);
  ACE_ASSERT(area_1_num_doors && area_2_num_doors);
  if ((area_1_num_doors == 1) ||
      (area_2_num_doors == 1))
  {
    switch (orientation)
    {
      case MAP_ORIENTATION_HORIZONTAL:
        return ((area_1_num_doors == 1) ? DIRECTION_DOWN : DIRECTION_UP);
      case MAP_ORIENTATION_VERTICAL:
        return ((area_1_num_doors == 1) ? DIRECTION_RIGHT : DIRECTION_LEFT);
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid orientation (was: \"%s\"), aborting\n"),
                   ACE_TEXT(RPG_Map_Common_Tools::orientation2String(orientation).c_str())));
        ACE_ASSERT(false);

        return RPG_MAP_DIRECTION_INVALID;
      }
    } // end SWITCH
  } // end IF

  if ((!area_1_is_square && area_2_is_square) ||
      (area_1_is_square && !area_2_is_square))
    goto compare_shape;

  goto compare_size;

  ACE_ASSERT(false);
  ACE_NOTREACHED(return RPG_MAP_DIRECTION_INVALID;)
}

bool
RPG_Map_Common_Tools::isFloor(const RPG_Map_Position_t& position_in,
                              const RPG_Map_FloorPlan_t& floorPlan_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::isFloor"));

  RPG_Map_Door_t position_door;
  position_door.position = position_in;
  return ((floorPlan_in.doors.find(position_door) ==
           floorPlan_in.doors.end())                                         &&
          (floorPlan_in.walls.find(position_in) == floorPlan_in.walls.end()) &&
          (floorPlan_in.unmapped.find(position_in) ==
           floorPlan_in.unmapped.end()));
}

bool
RPG_Map_Common_Tools::isInsideRoom(const RPG_Map_Position_t& position_in,
                                   const RPG_Map_FloorPlan_t& floorPlan_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::isInsideRoom"));

  // *NOTE*: algorithm is as follows:
  // - walls are NOT considered to be part of rooms
  // - doors are considered to be part of a room
  // - perform a "flood-fill" of the given position
  // - walk the perimeter, find a door
  // - if there were no doors, it's an unmapped area --> false
  // - that doors' notion of "outside" is the result

  // step0: preliminary checking
  if (floorPlan_in.walls.find(position_in) != floorPlan_in.walls.end())
    return false;
  RPG_Map_Door_t position_door;
  position_door.position = position_in;
  if (floorPlan_in.doors.find(position_door) != floorPlan_in.doors.end())
    return true;

  // step1: "flood-fill" the area
  RPG_Map_Positions_t area;
  RPG_Map_Common_Tools::floodFill(position_in,
                                  floorPlan_in,
                                  area);

  // step2: find doors along the perimeter
  RPG_Map_Position_t door = std::make_pair(0, 0);
  RPG_Map_Position_t start_position = *area.begin();
  start_position.first--; start_position.second--;
  ACE_ASSERT(floorPlan_in.walls.find(start_position) !=
             floorPlan_in.walls.end());
  RPG_Map_Position_t current = start_position;
  RPG_Map_Direction next = DIRECTION_RIGHT;
  RPG_Map_Direction origin = DIRECTION_DOWN;
  RPG_Map_Directions_t directions;
  do
  {
    // compute 4 neighbours
    RPG_Map_Position_t u, d, l, r;
    u = current; u.second--;
    d = current; d.second++;
    l = current; l.first--;
    r = current; r.first++;

    // find possible destination(s)
    directions.clear();
    position_door.position = u;
    if ((floorPlan_in.walls.find(u) != floorPlan_in.walls.end()) ||
        (floorPlan_in.doors.find(position_door) != floorPlan_in.doors.end()))
      directions.insert(DIRECTION_UP);
    position_door.position = r;
    if ((floorPlan_in.walls.find(r) != floorPlan_in.walls.end()) ||
        (floorPlan_in.doors.find(position_door) != floorPlan_in.doors.end()))
      directions.insert(DIRECTION_RIGHT);
    position_door.position = d;
    if ((floorPlan_in.walls.find(d) != floorPlan_in.walls.end()) ||
        (floorPlan_in.doors.find(position_door) != floorPlan_in.doors.end()))
      directions.insert(DIRECTION_DOWN);
    position_door.position = l;
    if ((floorPlan_in.walls.find(l) != floorPlan_in.walls.end()) ||
        (floorPlan_in.doors.find(position_door) != floorPlan_in.doors.end()))
      directions.insert(DIRECTION_LEFT);

    // compute next direction (clockwise)

    // sanity check: reached a dead-end ?
    if (directions.size() <= 1)
    {
      // dead-end --> turn around
      next = origin;
    } // end IF
    else
      switch (origin)
      {
        case DIRECTION_UP:
        {
          if ((directions.find(DIRECTION_DOWN) == directions.end()) || // corner
              (directions.find(DIRECTION_RIGHT) != directions.end()))  // intersection ?
          {
            // determine next direction
            if (directions.find(DIRECTION_RIGHT) != directions.end())
              next = DIRECTION_RIGHT; // intersection, turn left
            else
            {
              ACE_ASSERT(directions.find(DIRECTION_LEFT) != directions.end());
              next = DIRECTION_LEFT; // corner, turn right
            } // end ELSE
          } // end IF

          break;
        }
        case DIRECTION_RIGHT:
        {
          if ((directions.find(DIRECTION_LEFT) == directions.end()) || // corner
              (directions.find(DIRECTION_DOWN) != directions.end()))   // intersection ?
          {
            // determine next direction
            if (directions.find(DIRECTION_DOWN) != directions.end())
              next = DIRECTION_DOWN; // intersection, turn left
            else
            {
              ACE_ASSERT(directions.find(DIRECTION_UP) != directions.end());
              next = DIRECTION_UP; // corner, turn right
            } // end ELSE
          } // end IF

          break;
        }
        case DIRECTION_DOWN:
        {
          if ((directions.find(DIRECTION_UP) == directions.end()) || // corner
              (directions.find(DIRECTION_LEFT) != directions.end())) // intersection ?
          {
            // determine next direction
            if (directions.find(DIRECTION_LEFT) != directions.end())
              next = DIRECTION_LEFT; // intersection, turn left
            else
            {
              ACE_ASSERT(directions.find(DIRECTION_RIGHT) != directions.end());
              next = DIRECTION_RIGHT; // corner, turn right
            } // end ELSE
          } // end IF

          break;
        }
        case DIRECTION_LEFT:
        {
          if ((directions.find(DIRECTION_RIGHT) == directions.end()) || // corner
              (directions.find(DIRECTION_UP) != directions.end()))      // intersection ?
          {
            // determine next direction
            if (directions.find(DIRECTION_UP) != directions.end())
              next = DIRECTION_UP; // intersection, turn left
            else
            {
              ACE_ASSERT(directions.find(DIRECTION_DOWN) != directions.end());
              next = DIRECTION_DOWN; // corner, turn right
            } // end ELSE
          } // end IF

          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid origin (was \"%s\"), continuing\n"),
                     ACE_TEXT(RPG_Map_DirectionHelper::RPG_Map_DirectionToString(origin).c_str())));

          ACE_ASSERT(false);

          break;
        }
      } // end SWITCH

    // move along the perimeter
    switch (next)
    {
      case DIRECTION_UP:
      {
        current.second--; // go up
        origin = DIRECTION_DOWN;
        break;
      }
      case DIRECTION_RIGHT:
      {
        current.first++; // go right
        origin = DIRECTION_LEFT;
        break;
      }
      case DIRECTION_DOWN:
      {
        current.second++; // go down
        origin = DIRECTION_UP;
        break;
      }
      case DIRECTION_LEFT:
      {
        current.first--; // go left
        origin = DIRECTION_RIGHT;
        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid direction (was \"%s\"), continuing\n"),
                   ACE_TEXT(RPG_Map_DirectionHelper::RPG_Map_DirectionToString(next).c_str())));

        ACE_ASSERT(false);

        break;
      }
    } // end SWITCH

    position_door.position = current;
    if (floorPlan_in.doors.find(position_door) != floorPlan_in.doors.end())
    {
      door = current;

      break; // done
    } // end IF
  } while (current != start_position);
  ACE_ASSERT(door.first || door.second); // there must be a door...

  // step3: test doors' notion of "outside"
  position_door.position = door;
  RPG_Map_DoorsConstIterator_t current_door_iterator =
      floorPlan_in.doors.find(position_door);
  ACE_ASSERT((current_door_iterator != floorPlan_in.doors.end()) &&
             ((*current_door_iterator).position == door));
  // compute position just INSIDE of the door
  current = door;
  switch ((*current_door_iterator).outside)
  {
    case DIRECTION_UP:
      current.second++; break;
    case DIRECTION_RIGHT:
      current.first--; break;
    case DIRECTION_DOWN:
      current.second--; break;
    case DIRECTION_LEFT:
      current.first++; break;
    case RPG_MAP_DIRECTION_INVALID:
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid direction (was \"%s\"), continuing\n"),
                 ACE_TEXT(RPG_Map_DirectionHelper::RPG_Map_DirectionToString((*current_door_iterator).outside).c_str())));

      ACE_ASSERT(false);

      break;
    }
  } // end SWITCH

  return (area.find(current) != area.end());
}

bool
RPG_Map_Common_Tools::roomsAreSquare(const RPG_Map_t& map_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::hasLineOfSight"));

  RPG_Map_Positions_t area;
  for (RPG_Map_PositionsConstIterator_t iterator = map_in.seeds.begin();
       iterator != map_in.seeds.end();
       iterator++)
  {
    floodFill(*iterator,
              map_in.plan,
              area);
    if (!isSquare(area))
      return false;
  } // end FOR

  return true;
}

bool
RPG_Map_Common_Tools::hasLineOfSight(const RPG_Map_Position_t& source_in,
                                     const RPG_Map_Position_t& target_in,
                                     const RPG_Map_Positions_t& obstacles_in,
                                     const bool& allowTargetIsObstacle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::hasLineOfSight"));

  // step1: compute shortest path (== line-of-sight)
  RPG_Map_PositionList_t path;
  RPG_Map_Pathfinding_Tools::findPath(source_in,
                                      target_in,
                                      path);

  // step2: path blocked at some point ?
  RPG_Map_Positions_t line_of_sight, blocked;
  line_of_sight.insert(path.begin(), path.end());
  std::set_intersection(obstacles_in.begin(), obstacles_in.end(),
                        line_of_sight.begin(), line_of_sight.end(),
                        std::inserter(blocked, blocked.begin()));

  // *NOTE*: if target_in is an obstacle itself, the view (!) is NOT blocked
  // as long as there are no other obstacles in the path...
  if (allowTargetIsObstacle_in &&
      (obstacles_in.find(target_in) != obstacles_in.end()))
    return (blocked.size() == 1);

  return blocked.empty();
}

unsigned int
RPG_Map_Common_Tools::area2Positions(const RPG_Map_Position_t& position1_in,
                                     const RPG_Map_Position_t& position2_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::area2Positions"));

  return ((::abs(static_cast<int>(position1_in.first)  -
                 static_cast<int>(position2_in.first))  + 1) *
          (::abs(static_cast<int>(position1_in.second) -
                 static_cast<int>(position2_in.second)) + 1));
}

bool
RPG_Map_Common_Tools::positionInSquare(const RPG_Map_Position_t& position_in,
                                       const RPG_Map_Square_t& square_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::positionInSquare"));

  return ((position_in.first  >= square_in.ul.first)  &&
          (position_in.first  <= square_in.lr.first)  &&
          (position_in.second >= square_in.ul.second) &&
          (position_in.second <= square_in.lr.second));
}

bool
RPG_Map_Common_Tools::intersect(const RPG_Map_Area_t& map_in,
                                const RPG_Map_Position_t& position_in,
                                const ORIGIN& origin_in,
                                RPG_Map_Directions_t& directions_out,
                                RPG_Map_Direction& next_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::intersect"));

  // init return value(s)
  directions_out.clear();
  next_out = RPG_MAP_DIRECTION_INVALID;

  RPG_Map_Position_t up, right, down, left;
  up = position_in; up.second--;
  right = position_in; right.first++;
  down = position_in; down.second++;
  left = position_in; left.first--;

  // find possible destination(s)
  if (map_in.find(up) != map_in.end())
    directions_out.insert(DIRECTION_UP);
  if (map_in.find(right) != map_in.end())
    directions_out.insert(DIRECTION_RIGHT);
  if (map_in.find(down) != map_in.end())
    directions_out.insert(DIRECTION_DOWN);
  if (map_in.find(left) != map_in.end())
    directions_out.insert(DIRECTION_LEFT);

  // if NOT an intersection, compute next direction
  switch (directions_out.size())
  {
    case 2:
    {
      directions_out.erase(origin_in);
      // fall through !
    }
    case 1:
    {
      next_out = *(directions_out.begin());
      directions_out.clear();
      // fall through !
    }
    case 0:
      return false;
    default:
      break;
  } // end IF

  return true;
}

void
RPG_Map_Common_Tools::crawlToPosition(const RPG_Map_Area_t& map_in,
                                      const RPG_Map_Position_t& origin_in,
                                      const RPG_Map_Position_t& target_in,
                                      const ORIGIN& startOrigin_in,
                                      RPG_Map_PositionList_t& trail_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::crawlToPosition"));

  // init return value(s)
  trail_out.clear();

  ORIGIN origin = startOrigin_in;
  RPG_Map_Direction next = RPG_MAP_DIRECTION_INVALID;
  RPG_Map_Directions_t directions;

  // start at origin
  RPG_Map_Position_t current = origin_in;
  do
  {
    if (intersect(map_in,
                  current,
                  origin,
                  directions,
                  next))
    {
      // compute next direction
      switch (origin)
      {
        case DIRECTION_UP:
        {
          // proceed right, down or left, in this order
          if (directions.find(DIRECTION_RIGHT) != directions.end())
            next = DIRECTION_RIGHT;
          else if (directions.find(DIRECTION_DOWN) != directions.end())
            next = DIRECTION_DOWN;
          else
            next = DIRECTION_LEFT;

          break;
        }
        case DIRECTION_RIGHT:
        {
          // proceed down, left or up, in this order
          if (directions.find(DIRECTION_DOWN) != directions.end())
            next = DIRECTION_DOWN;
          else if (directions.find(DIRECTION_LEFT) != directions.end())
            next = DIRECTION_LEFT;
          else
            next = DIRECTION_UP;

          break;
        }
        case DIRECTION_DOWN:
        {
          // proceed left, up or right, in this order
          if (directions.find(DIRECTION_LEFT) != directions.end())
            next = DIRECTION_LEFT;
          else if (directions.find(DIRECTION_UP) != directions.end())
            next = DIRECTION_UP;
          else
            next = DIRECTION_RIGHT;

          break;
        }
        case DIRECTION_LEFT:
        {
          // proceed up, right or down, in this order
          if (directions.find(DIRECTION_UP) != directions.end())
            next = DIRECTION_UP;
          else if (directions.find(DIRECTION_RIGHT) != directions.end())
            next = DIRECTION_RIGHT;
          else
            next = DIRECTION_DOWN;

          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid origin (was \"%s\"), continuing\n"),
                     ACE_TEXT(RPG_Map_DirectionHelper::RPG_Map_DirectionToString(origin).c_str())));
          ACE_ASSERT(false);

          break;
        }
      } // end SWITCH
    } // end IF

    trail_out.push_back(current);
    // finished ?
    if (current == target_in)
      return;

    // move
    switch (next)
    {
      case DIRECTION_UP:
      {
        current.second--; // go up
        origin = DIRECTION_DOWN;
        break;
      }
      case DIRECTION_RIGHT:
      {
        current.first++; // go right
        origin = DIRECTION_LEFT;
        break;
      }
      case DIRECTION_DOWN:
      {
        current.second++; // go down
        origin = DIRECTION_UP;
        break;
      }
      case DIRECTION_LEFT:
      {
        current.first--; // go left
        origin = DIRECTION_RIGHT;
        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid direction (was \"%s\"), continuing\n"),
                   ACE_TEXT(RPG_Map_DirectionHelper::RPG_Map_DirectionToString(next).c_str())));
        ACE_ASSERT(false);

        break;
      }
    } // end SWITCH
  } while (true);
}

void
RPG_Map_Common_Tools::crop(RPG_Map_Area_t& room_inout)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::crop"));

  // sanity check
  if (room_inout.empty())
    return; // nothing to do...

  // two-pass algorithm:
  // 1. iterate over rows
  // 2. iterate over columns
  // and remove any sequences of less than 3 consecutive cells
  RPG_Map_AreaConstIterator_t begin_sequence = room_inout.begin();
  RPG_Map_AreaConstIterator_t line_iterator;
  RPG_Map_AreaConstIterator_t next_iterator = room_inout.begin();
  unsigned int next_x = 0;
  unsigned int count = 0;

  // step1
  while (true)
  {
    if ((next_iterator != room_inout.end()) &&
        ((*next_iterator).second == (*begin_sequence).second))
    {
      // skip to next row/end
      next_iterator++;
      continue;
    } // end IF

    // iterate over row
    next_x = (*begin_sequence).first;
    count = 0;
    line_iterator = begin_sequence;
    while (line_iterator != next_iterator) // maybe end
    {
      while ((line_iterator != room_inout.end()) &&
             ((*line_iterator).first == next_x))
      {
        next_x++;
        count++;
        line_iterator++;
      } // end IF

      // there was a gap (maybe next row/end)
      // --> remove any previous sequence of less than 3 consecutive cells
      if (count < 3)
      {
        // *BUG* here
        // *NOTE*: std::set::erase removes [first, last) !
        room_inout.erase(begin_sequence, line_iterator);
      } // end IF
      if (line_iterator == room_inout.end()) // done ?
        continue;

      // same line --> next sequence ?
      begin_sequence = line_iterator;
      next_x = (*begin_sequence).first;
      count = 0;
    } // end WHILE

    // finished ?
    if (room_inout.empty() ||
        (next_iterator == room_inout.end()))
      break;
  } // end WHILE

  // sanity check
  if (room_inout.empty())
    return; // nothing more to do...

  // step2
  // create alternate sorting
  RPG_Map_AltPositions_t alt_room;
  for (RPG_Map_AreaConstIterator_t iterator = room_inout.begin();
       iterator != room_inout.end();
       iterator++)
    alt_room.insert(*iterator);

  RPG_Map_AltPositionsConstIterator_t begin_alt_room = alt_room.begin();
  RPG_Map_AltPositionsConstIterator_t line_iterator_alt;
  RPG_Map_AltPositionsConstIterator_t next_iterator_alt = alt_room.begin();
  unsigned int next_y = 0;
  count = 0;
  while (true)
  {
    if ((next_iterator_alt != alt_room.end()) &&
        ((*next_iterator_alt).first == (*begin_alt_room).first))
    {
      // skip to next column/end
      next_iterator_alt++;
      continue;
    } // end IF

    // iterate over column
    next_y = (*begin_alt_room).second;
    count = 0;
    line_iterator_alt = begin_alt_room;
    while (line_iterator_alt != next_iterator_alt) // maybe end
    {
      while ((line_iterator_alt != alt_room.end()) &&
            ((*line_iterator_alt).second == next_y))
      {
        next_y++;
        count++;
        line_iterator_alt++;
      } // end IF

      // there is a gap (maybe next column)
      // --> remove any previous sequence of less than 3 consecutive cells
      if (count < 3)
      {
        // *NOTE*: std::set::erase removes [first, last) !
        alt_room.erase(begin_alt_room, line_iterator_alt);
      } // end IF
      if (line_iterator_alt == alt_room.end()) // done ?
        continue;

      // same line --> next sequence ?
      begin_alt_room = line_iterator_alt;
      next_y = (*begin_alt_room).second;
      count = 0;
    } // end WHILE

    // finished ?
    if (alt_room.empty() ||
        (next_iterator_alt == alt_room.end()))
      break;
  } // end WHILE

  // return original sorting
  room_inout.clear();
  room_inout.insert(alt_room.begin(), alt_room.end());
}

// void
// RPG_Map_Common_Tools::cropSquareBoundary(RPG_Map_Area_t& room_inout)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::cropSquareBoundary"));
//
//   // sanity check
//   if (room_inout.empty())
//     return; // nothing to do...
//
//   // two-pass algorithm:
//   // 1. iterate over rows
//   // 2. iterate over columns
//   // and remove any sequences of less than 3 consecutive cells
//
//   // *NOTE*: as this is a (square !) boundary, just check the first rows/columns
//   RPG_Map_AreaConstIterator_t begin_sequence = room_inout.begin();
//   RPG_Map_AreaConstIterator_t line_iterator;
//   RPG_Map_AreaConstIterator_t zone_iterator = room_inout.begin();
//   unsigned long next_x = 0;
//   unsigned long count = 0;
//
//   bool is_first_line = true;
//
//   // step1
//   while (true)
//   {
//     if ((zone_iterator != room_inout.end()) &&
//         ((*zone_iterator).second == (*begin_sequence).second))
//     {
//       // skip to next row/end
//       zone_iterator++;
//       continue;
//     } // end IF
//
//     // first row ?
//     if (is_first_line)
//       is_first_line = false;
//     else
//     {
// //       // second row --> skip to last row
// //       RPG_Map_AreaConstIterator_t last = room_inout.end(); last--;
// //       while ((zone_iterator != room_inout.end()) &&
// //              ((*zone_iterator).second != (*last).second))
// //         zone_iterator++;
// //       begin_sequence = zone_iterator;
//       break;
//     } // end ELSE
//
//     // iterate over row
//     next_x = (*begin_sequence).first;
//     count = 0;
//     line_iterator = begin_sequence;
//     while (line_iterator != zone_iterator) // maybe end
//     {
//       while ((line_iterator != room_inout.end()) &&
//               ((*line_iterator).first == next_x))
//       {
//         next_x++;
//         count++;
//         line_iterator++;
//       } // end IF
//
//       // there was a gap (maybe next row/end)
//       // --> remove any previous sequence of less than 3 consecutive cells
//       if (count < 3)
//       {
//         // *NOTE*: if we crop anything --> crop EVERYTHING !
//         room_inout.clear();
//         break; // done !
//       } // end IF
//       begin_sequence = line_iterator;
//       next_x = (*begin_sequence).first;
//       count = 0;
//     } // end WHILE
//
//     // finished ?
//     if (room_inout.empty() ||
//         (zone_iterator == room_inout.end()))
//       break;
//   } // end WHILE
//
//   // sanity check
//   if (room_inout.empty())
//     return; // nothing more to do...
//
//   // step2
//   // create alternate sorting
//   RPG_Map_AltPositions_t alt_room;
//   for (zone_iterator = room_inout.begin();
//        zone_iterator != room_inout.end();
//        zone_iterator++)
//     alt_room.insert(*zone_iterator);
//
//   begin_sequence = alt_room.begin();
//   zone_iterator = alt_room.begin();
//   unsigned long next_y = 0;
//   count = 0;
//
//   is_first_line = true;
//
//   while (true)
//   {
//     if ((zone_iterator != alt_room.end()) &&
//         ((*zone_iterator).first == (*begin_sequence).first))
//     {
//       // skip to next column/end
//       zone_iterator++;
//       continue;
//     } // end IF
//
//     // first column ?
//     if (is_first_line)
//       is_first_line = false;
//     else
//     {
// //       // second column --> skip to last column
// //       RPG_Map_AreaConstIterator_t last = room_inout.end(); last--;
// //       while ((zone_iterator != room_inout.end()) &&
// //              ((*zone_iterator).first != (*last).first))
// //         zone_iterator++;
// //       begin_sequence = zone_iterator;
//       break;
//     } // end ELSE
//
//     // iterate over column
//     next_y = (*begin_sequence).second;
//     count = 0;
//     line_iterator = begin_sequence;
//     while (line_iterator != zone_iterator) // maybe end
//     {
//       while ((line_iterator != room_inout.end()) &&
//               ((*line_iterator).second == next_y))
//       {
//         next_y++;
//         count++;
//         line_iterator++;
//       } // end IF
//
//       // there is a gap (maybe next column)
//       // --> remove any previous sequence of less than 3 consecutive cells
//       if (count < 3)
//       {
//         // *NOTE*: if we crop anything --> crop EVERYTHING !
//         alt_room.clear();
//         break; // done !
//       } // end IF
//       begin_sequence = line_iterator;
//       next_y = (*begin_sequence).second;
//       count = 0;
//     } // end WHILE
//
//     // finished ?
//     if (alt_room.empty() ||
//         (zone_iterator == alt_room.end()))
//       break;
//   } // end WHILE
//
//   // return original sorting
//   if (alt_room.empty())
//     room_inout.clear();
// }

bool
RPG_Map_Common_Tools::turn(const RPG_Map_Area_t& map_in,
                           const RPG_Map_Position_t& position_in,
                           const ORIGIN& origin_in,
                           const bool& clockwise_in,
                           bool& isCorner_out,
                           RPG_Map_Direction& next_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::turn"));

  // init return value(s)
  isCorner_out = false;
  // *NOTE*: only change next_out if appropriate (i.e. when turning)
//   next_out = DIRECTION_INVALID;

  RPG_Map_Position_t up, right, down, left;
  up = position_in; up.second--;
  right = position_in; right.first++;
  down = position_in; down.second++;
  left = position_in; left.first--;

  // find possible destination(s)
  RPG_Map_Directions_t directions;
  if (map_in.find(up) != map_in.end())
    directions.insert(DIRECTION_UP);
  if (map_in.find(right) != map_in.end())
    directions.insert(DIRECTION_RIGHT);
  if (map_in.find(down) != map_in.end())
    directions.insert(DIRECTION_DOWN);
  if (map_in.find(left) != map_in.end())
    directions.insert(DIRECTION_LEFT);

  // *NOTE*: if position is a [dead-end/]corner/intersection
  // --> compute next direction

  // sanity check: reached a dead-end ?
  // *NOTE*: while walking a perimeter, "dead-ends" cannot happen - unless:
  // - the room itself is "flat" (i.e. a single row/cell)
  if (directions.size() <= 1)
  {
    // dead-end, turn around
    next_out = origin_in;

    return true;
  } // end IF

  // *NOTE*: while walking a perimeter, intersections cannot happen - unless:
  // - a section of the room is "flat" (i.e. consists of "solid walls")
  switch (origin_in)
  {
    case DIRECTION_UP:
    {
      if ((directions.find(DIRECTION_DOWN) == directions.end()) || // corner
          (directions.find((clockwise_in ? DIRECTION_RIGHT
                                         : DIRECTION_LEFT)) !=
           directions.end())) // intersection
      {
        // determine next direction
        if (directions.find((clockwise_in ? DIRECTION_RIGHT
                                          : DIRECTION_LEFT)) !=
            directions.end())
          next_out =
              (clockwise_in ? DIRECTION_RIGHT : DIRECTION_LEFT); // intersection, turn left/right
        else
        {
          next_out = (clockwise_in ? DIRECTION_LEFT : DIRECTION_RIGHT); // corner, turn right/left
          isCorner_out = true;
        } // end ELSE

        return true;
      } // end IF

      // --> can continue (and cannot turn left/right)
      break;
    }
    case DIRECTION_RIGHT:
    {
      if ((directions.find(DIRECTION_LEFT) == directions.end()) || // corner
          (directions.find((clockwise_in ? DIRECTION_DOWN
                                         : DIRECTION_UP)) != directions.end())) // intersection
      {
        // determine next direction
        if (directions.find((clockwise_in ? DIRECTION_DOWN : DIRECTION_UP)) !=
            directions.end())
          next_out = (clockwise_in ? DIRECTION_DOWN : DIRECTION_UP); // intersection, turn left/right
        else
        {
          next_out = (clockwise_in ? DIRECTION_UP : DIRECTION_DOWN); // corner, turn right/left
          isCorner_out = true;
        } // end ELSE

        return true;
      } // end IF

      // --> can continue (and cannot turn left/right)
      break;
    }
    case DIRECTION_DOWN:
    {
      if ((directions.find(DIRECTION_UP) == directions.end()) || // corner
          (directions.find((clockwise_in ? DIRECTION_LEFT
                                         : DIRECTION_RIGHT)) !=
           directions.end())) // intersection
      {
        // determine next direction
        if (directions.find((clockwise_in ? DIRECTION_LEFT
                                          : DIRECTION_RIGHT)) !=
            directions.end())
          next_out = (clockwise_in ? DIRECTION_LEFT : DIRECTION_RIGHT); // intersection, turn left/right
        else
        {
          next_out = (clockwise_in ? DIRECTION_RIGHT : DIRECTION_LEFT); // corner, turn right/left
          isCorner_out = true;
        } // end ELSE

        return true;
      } // end IF

      // --> can continue (and cannot turn left/right)
      break;
    }
    case DIRECTION_LEFT:
    {
      if ((directions.find(DIRECTION_RIGHT) == directions.end()) || // corner
          (directions.find((clockwise_in ? DIRECTION_UP
                                         : DIRECTION_DOWN)) !=
           directions.end())) // intersection
      {
        // determine next direction
        if (directions.find((clockwise_in ? DIRECTION_UP
                                          : DIRECTION_DOWN)) !=
            directions.end())
          next_out = (clockwise_in ? DIRECTION_UP : DIRECTION_DOWN); // intersection, turn left/right
        else
        {
          next_out = (clockwise_in ? DIRECTION_DOWN : DIRECTION_UP); // corner, turn right/left
          isCorner_out = true;
        } // end ELSE

        return true;
      } // end IF

      // --> can continue (and cannot turn left/right)
      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid origin (was \"%s\"), continuing\n"),
                 ACE_TEXT(RPG_Map_DirectionHelper::RPG_Map_DirectionToString(origin_in).c_str())));
      ACE_ASSERT(false);

      break;
    }
  } // end IF

  return false;
}

void
RPG_Map_Common_Tools::findDoorPositions(const RPG_Map_Area_t& room_in,
                                        const bool& doorFillsPosition_in,
                                        RPG_Map_PositionList_t& doorPositions_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::findDoorPositions"));

  // init return value(s)
  doorPositions_out.clear();

  // sanity check(s)
  if (room_in.empty())
    return; // nothing to do

  // *NOTE*: constraints depend on whether doors fill a whole position [] or
  // just "adorn" the side of a wall (implements the notion that walls are
  // infinitely thin - which is essentially a design *DECISION*)
  // - corners/intersections are unsuitable positions
  // - enforce a minimal distance [i.e. to allow corridors_bounds] between any 2
  //   doors
  RPG_Map_Position_t current = *(room_in.begin());
  RPG_Map_Direction next = DIRECTION_RIGHT;
  ORIGIN origin = DIRECTION_DOWN;
  bool is_corner = true; // (else intersection)
  if (doorFillsPosition_in)
  {
    // *NOTE*: in this case, the perimeter should already have been prepared
    // --> see references to crop() above

    // what remains is walking the perimeter and checking each position, while
    // omitting corners.
    // *NOTE*: do this both clock/counter-clockwise to find the maximum number
    // of suitable position(s)

    // step1: clockwise
    // *NOTE*: first position is ALWAYS a corner, thus unsuitable
    // --> next position to the right/down along the wall could be suitable
    bool next_suitable = true;
    do
    {
      // change direction ?
      if (!turn(room_in,
                current,
                origin,
                true, // turn clockwise
                is_corner,
                next)) // <-- will be changed appropriately
      {
        if (next_suitable)
        {
          // good position for a door !
          doorPositions_out.push_back(current);
          next_suitable = false; // flag adjacent position as unsuitable
        } // end IF
        else
          next_suitable = true; // adjacent position could be suitable
      } // end IF
      else
        next_suitable = is_corner; // flag next position

      // move along the perimeter
      switch (next)
      {
        case DIRECTION_UP:
        {
          current.second--; // go up
          origin = DIRECTION_DOWN;
          break;
        }
        case DIRECTION_RIGHT:
        {
          current.first++; // go right
          origin = DIRECTION_LEFT;
          break;
        }
        case DIRECTION_DOWN:
        {
          current.second++; // go down
          origin = DIRECTION_UP;
          break;
        }
        case DIRECTION_LEFT:
        {
          current.first--; // go left
          origin = DIRECTION_RIGHT;
          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid direction (was \"%s\"), continuing\n"),
                     ACE_TEXT(RPG_Map_DirectionHelper::RPG_Map_DirectionToString(next).c_str())));

          ACE_ASSERT(false);

          break;
        }
      } // end SWITCH
    } while (current != *(room_in.begin()));

    // step2: counter-clockwise
    RPG_Map_PositionList_t alt_doorPositions;
    origin = DIRECTION_RIGHT;
    is_corner = true; // (else intersection)
    // *NOTE*: first position is ALWAYS a corner, thus unsuitable
    // --> next position to the right/down along the wall could be suitable
    // if it's not an intersection...
    next_suitable = true;
    do
    {
      // change direction ?
      if (!turn(room_in,
                current,
                origin,
                false, // turn counter-clockwise
                is_corner,
                next)) // <-- will be changed appropriately
      {
        if (next_suitable)
        {
          // good position for a door !
          alt_doorPositions.push_back(current);
          next_suitable = false; // flag adjacent position as unsuitable
        } // end IF
        else
          next_suitable = true; // adjacent position could be suitable
      } // end IF
      else
        next_suitable = is_corner; // flag next position

      // move along the perimeter
      switch (next)
      {
        case DIRECTION_UP:
        {
          current.second--; // go up
          origin = DIRECTION_DOWN;
          break;
        }
        case DIRECTION_RIGHT:
        {
          current.first++; // go right
          origin = DIRECTION_LEFT;
          break;
        }
        case DIRECTION_DOWN:
        {
          current.second++; // go down
          origin = DIRECTION_UP;
          break;
        }
        case DIRECTION_LEFT:
        {
          current.first--; // go left
          origin = DIRECTION_RIGHT;
          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid direction (was \"%s\"), continuing\n"),
                     ACE_TEXT(RPG_Map_DirectionHelper::RPG_Map_DirectionToString(next).c_str())));
          ACE_ASSERT(false);

          break;
        }
      } // end SWITCH
    } while (current != *(room_in.begin()));

    // step3: return the "better" result (i.e. more options == better)
    if (alt_doorPositions.size() > doorPositions_out.size())
      doorPositions_out = alt_doorPositions;
  } // end IF
  else
  {
    // in this case, walls are infinitely thin, so doors can be placed pretty
    // much everywhere along the perimeter
    // --> walk the perimeter counting "faces" and divide the result by the
    // (arbitrary) separation we want to enforce

    // guard against special case
    if (room_in.size() == 1)
    {
      // four positions
      doorPositions_out.push_back(current);
      doorPositions_out.push_back(current);
      doorPositions_out.push_back(current);
      doorPositions_out.push_back(current);
    } // end IF
    else
    {
      is_corner = false;
      do
      {
        // move along the perimeter
        switch (next)
        {
          case DIRECTION_UP:
          {
            current.second--; // go up
            origin = DIRECTION_DOWN;
            break;
          }
          case DIRECTION_RIGHT:
          {
            current.first++; // go right
            origin = DIRECTION_LEFT;
            break;
          }
          case DIRECTION_DOWN:
          {
            current.second++; // go down
            origin = DIRECTION_UP;
            break;
          }
          case DIRECTION_LEFT:
          {
            current.first--; // go left
            origin = DIRECTION_RIGHT;
            break;
          }
          default:
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("invalid direction (was \"%s\"), continuing\n"),
                       ACE_TEXT(RPG_Map_DirectionHelper::RPG_Map_DirectionToString(next).c_str())));

            ACE_ASSERT(false);

            break;
          }
        } // end SWITCH

        // change direction ?
        if (!turn(room_in,
                  current,
                  origin,
                  true, // turn clockwise
                  is_corner,
                  next)) // <-- will be changed appropriately
        {
          // moving along the wall...
          doorPositions_out.push_back(current);
        } // end IF
        else
        {
          // reached a dead-end ?
          if (next == origin)
          {
            // three positions
            doorPositions_out.push_back(current);
            doorPositions_out.push_back(current);
            doorPositions_out.push_back(current);
          } // end IF
          else if (is_corner)
          {
            // two positions
            doorPositions_out.push_back(current);
            doorPositions_out.push_back(current);
          } // end IF
        } // end ELSE
      } while (current != *(room_in.begin()));
    } // end ELSE

    // step2: enforce separation, i.e. spare every (RPG_MAP_DOOR_SEPARATION + 1)
    // element
    unsigned int count = 0;
    for (RPG_Map_PositionListIterator_t list_iterator = doorPositions_out.begin();
         list_iterator != doorPositions_out.end();
         list_iterator++, count++)
      if (count % (RPG_MAP_DOOR_SEPARATION + 1))
        doorPositions_out.erase(list_iterator);
  } // end ELSE
}

RPG_Map_Direction
RPG_Map_Common_Tools::door2exitDirection(const RPG_Map_Area_t& room_in,
                                         const RPG_Map_Position_t& door_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::door2exitDirection"));

  // step1: compute 4 neighbours
  RPG_Map_Position_t up, right, down, left;
  up = door_in; up.second -= 1;
  right = door_in; right.first += 1;
  down = door_in; down.second += 1;
  left = door_in; left.first -= 1;

  // step2: two of the neighbours will be walls, the third will be inside
  // --> the one that is NOT part of the room indicates the exit direction
  if (room_in.find(up) == room_in.end())
    return DIRECTION_UP;
  else if (room_in.find(right) == room_in.end())
    return DIRECTION_RIGHT;
  else if (room_in.find(down) == room_in.end())
    return DIRECTION_DOWN;

  ACE_ASSERT(room_in.find(left) == room_in.end());
  return DIRECTION_LEFT;
}

void
RPG_Map_Common_Tools::displayCorridors(const unsigned int& dimensionX_in,
                                       const unsigned int& dimensionY_in,
                                       const RPG_Map_AreaList_t& rooms_in,
                                       const RPG_Map_AreaList_t& doors_in,
                                       const RPG_Map_AreaList_t& corridors_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::displayCorridors"));

  RPG_Map_Position_t current_position;
  unsigned int index = 0;
  bool shared_path = false;
  unsigned int first_index = 0;
  RPG_Map_AreaListConstIterator_t iterator;
  bool done = false;
  std::ostringstream converter;
  std::string corridor_glyphs("0123456789^°!\"§$%&/()ß?'`+*~-_:,;{}[]<>|");
  for (unsigned int y = 0;
       y < dimensionY_in;
       y++)
  {
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));
    converter.clear();

    for (unsigned int x = 0;
         x < dimensionX_in;
         x++)
    {
      current_position = std::make_pair(x, y);

      done = false;
      for (iterator = doors_in.begin();
           iterator != doors_in.end();
           iterator++)
      {
        if ((*iterator).find(current_position) != (*iterator).end())
        {
          converter << ACE_TEXT("=");

          done = true;

          break;
        } // end IF
      } // end FOR
      if (done)
        continue;
      for (iterator = rooms_in.begin();
           iterator != rooms_in.end();
           iterator++)
      {
        if ((*iterator).find(current_position) != (*iterator).end())
        {
          converter << ACE_TEXT("#");

          done = true;

          break;
        } // end IF
      } // end FOR
      if (done)
        continue;

      index = 0;
      shared_path = false;
      first_index = 0;
      for (iterator = corridors_in.begin();
           iterator != corridors_in.end();
           iterator++, index++)
      {
        if ((*iterator).find(current_position) != (*iterator).end())
        {
          if (!done)
          {
            first_index = index;
            done = true;
          } // end IF
          else
            shared_path = true;
        } // end IF
      } // end FOR
      if (done)
      {
        ACE_ASSERT(first_index < corridor_glyphs.size());
        converter << (shared_path ? 'x'
                                  : corridor_glyphs[first_index]);

        continue;
      } // end IF

      converter << ACE_TEXT(".");
    } // end FOR
    converter << std::endl;

    // print line
    std::clog << converter.str();
  } // end FOR
}

void
RPG_Map_Common_Tools::floodFill(const RPG_Map_Position_t& position_in,
                                const RPG_Map_FloorPlan_t& floorPlan_in,
                                RPG_Map_Positions_t& area_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::floodFill"));

  // init return value(s)
  area_out.clear();

  area_out.insert(position_in);
  std::stack<RPG_Map_Position_t> position_stack;
  position_stack.push(position_in);
  RPG_Map_Door_t position_door;
  while (!position_stack.empty())
  {
    // compute 8 neighbours
    RPG_Map_Position_t tl, t, tr, l, r, bl, b, br;
    tl = position_stack.top(); tl.first--; tl.second--;
    t = position_stack.top(); t.second--;
    tr = position_stack.top(); tr.first++; tr.second--;
    l = position_stack.top(); l.first--;
    r = position_stack.top(); r.first++;
    bl = position_stack.top(); bl.first--; bl.second++;
    b = position_stack.top(); b.second++;
    br = position_stack.top(); br.first++; br.second++;

    // remove element
    position_stack.pop();

    // neighbor != WALL/DOOR ? --> insert into area (& stack)
    position_door.position = tl;
    if ((floorPlan_in.walls.find(tl) == floorPlan_in.walls.end()) &&
        (floorPlan_in.doors.find(position_door) == floorPlan_in.doors.end()))
      if (area_out.insert(tl).second)
        position_stack.push(tl);
    position_door.position = t;
    if ((floorPlan_in.walls.find(t) == floorPlan_in.walls.end()) &&
        (floorPlan_in.doors.find(position_door) == floorPlan_in.doors.end()))
      if (area_out.insert(t).second)
        position_stack.push(t);
    position_door.position = tr;
    if ((floorPlan_in.walls.find(tr) == floorPlan_in.walls.end()) &&
        (floorPlan_in.doors.find(position_door) == floorPlan_in.doors.end()))
      if (area_out.insert(tr).second)
        position_stack.push(tr);
    position_door.position = l;
    if ((floorPlan_in.walls.find(l) == floorPlan_in.walls.end()) &&
        (floorPlan_in.doors.find(position_door) == floorPlan_in.doors.end()))
      if (area_out.insert(l).second)
        position_stack.push(l);
    position_door.position = r;
    if ((floorPlan_in.walls.find(r) == floorPlan_in.walls.end()) &&
        (floorPlan_in.doors.find(position_door) == floorPlan_in.doors.end()))
      if (area_out.insert(r).second)
        position_stack.push(r);
    position_door.position = bl;
    if ((floorPlan_in.walls.find(bl) == floorPlan_in.walls.end()) &&
        (floorPlan_in.doors.find(position_door) == floorPlan_in.doors.end()))
      if (area_out.insert(bl).second)
        position_stack.push(bl);
    position_door.position = b;
    if ((floorPlan_in.walls.find(b) == floorPlan_in.walls.end()) &&
        (floorPlan_in.doors.find(position_door) == floorPlan_in.doors.end()))
      if (area_out.insert(b).second)
        position_stack.push(b);
    position_door.position = br;
    if ((floorPlan_in.walls.find(br) == floorPlan_in.walls.end()) &&
        (floorPlan_in.doors.find(position_door) == floorPlan_in.doors.end()))
      if (area_out.insert(br).second)
        position_stack.push(br);
  } // end WHILE
}

bool
RPG_Map_Common_Tools::isSquare(const RPG_Map_Positions_t& area_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::isSquare"));

  // sanity checks
  if (area_in.empty())
    return false; // nothing to do...
  if (area_in.size() < 3)
    return true; // nothing to do...

  //RPG_Map_Position_t upper_left  = std::make_pair(0, 0);
  //RPG_Map_Position_t lower_right = std::make_pair(std::numeric_limits<unsigned int>::max(),
  //                                                std::numeric_limits<unsigned int>::max());
  RPG_Map_Position_t upper_left  = *area_in.begin();
  RPG_Map_PositionsConstIterator_t last = area_in.end(); last--;
  RPG_Map_Position_t lower_right = *last;

  for (unsigned int x = upper_left.first;
       x <= lower_right.first;
       x++)
    for (unsigned int y = upper_left.second;
         y <= lower_right.second;
         y++)
      if (area_in.find(std::make_pair(x, y)) == area_in.end())
        return false;

  return true;
}

void
RPG_Map_Common_Tools::perimeter(const RPG_Map_Area_t& room_in,
                                RPG_Map_Positions_t& perimeter_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::perimeter"));

  // init return value(s)
  perimeter_out.clear();

  if (room_in.empty())
    return; // nothing to do...

  RPG_Map_Position_t upper_left, upper_right, lower_left, lower_right;
  RPG_Map_AreaConstIterator_t current = room_in.begin();
  RPG_Map_AreaConstIterator_t last = room_in.end(); last--;
  // step1: compute the 4 corners
  // upper left == first element
  upper_left = *current;
  // upper right == last element of the first row
  while ((current != room_in.end()) &&
         ((*current).second == upper_left.second))
    current++;
  current--;
  upper_right = *current;
  // lower_right == last element
  lower_right = *last;
  // lower_left == first element of the last row
  if (upper_left.second == lower_right.second)
    lower_left = upper_left;
  else
  {
    // there's more than one row
    current = last;
    while ((*current).second == lower_right.second)
      current--;
    current++;
    lower_left = *current;
  } // end ELSE

  // step2: start at top left position and go around clockwise
  RPG_Map_PositionList_t trail;
  crawlToPosition(room_in,
                  upper_left,
                  upper_right,
                  DIRECTION_DOWN,
                  trail);
  perimeter_out.insert(trail.begin(), trail.end());
  trail.clear();
  crawlToPosition(room_in,
                  upper_right,
                  lower_right,
                  DIRECTION_LEFT,
                  trail);
  perimeter_out.insert(trail.begin(), trail.end());
  trail.clear();
  crawlToPosition(room_in,
                  lower_right,
                  lower_left,
                  DIRECTION_UP,
                  trail);
  perimeter_out.insert(trail.begin(), trail.end());
  trail.clear();
  crawlToPosition(room_in,
                  lower_left,
                  upper_left,
                  DIRECTION_RIGHT,
                  trail);
  perimeter_out.insert(trail.begin(), trail.end());
}

unsigned int
RPG_Map_Common_Tools::countAdjacentDoors(const RPG_Map_Positions_t& area_in,
                                         const RPG_Map_FloorPlan_t& floorPlan_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::countAdjacentDoors"));

  // init return value(s)
  unsigned int result = 0;

  // sanity check
  if (area_in.empty())
    return 0; // nothing to do...

  // step1: find a trail along the perimeter
  RPG_Map_PositionList_t perimeter_trail;
  RPG_Map_Area_t zone;
  zone.insert(area_in.begin(), area_in.end());
  RPG_Map_Position_t upper_left, upper_right, lower_left, lower_right;
  RPG_Map_AreaConstIterator_t current = zone.begin();
  RPG_Map_AreaConstIterator_t last = zone.end(); last--;
  // step1a: compute the 4 corners
  // upper left == first element
  upper_left = *current;
  // upper right == last element of the first row
  while ((current != zone.end()) &&
         ((*current).second == upper_left.second))
    current++;
  current--;
  upper_right = *current;
  // lower_right == last element
  lower_right = *last;
  // lower_left == first element of the last row
  if (upper_left.second == lower_right.second)
    lower_left = upper_left;
  else
  {
    // there's more than one row
    current = last;
    while ((*current).second == lower_right.second)
      current--;
    current++;
    lower_left = *current;
  } // end ELSE
  // step1b: start at top left position and go around clockwise
  RPG_Map_PositionList_t trail;
  crawlToPosition(zone,
                  upper_left,
                  upper_right,
                  DIRECTION_DOWN,
                  trail);
  perimeter_trail.insert(perimeter_trail.end(), trail.begin(), trail.end());
  trail.clear();
  crawlToPosition(zone,
                  upper_right,
                  lower_right,
                  DIRECTION_LEFT,
                  trail);
  perimeter_trail.insert(perimeter_trail.end(), trail.begin(), trail.end());
  trail.clear();
  crawlToPosition(zone,
                  lower_right,
                  lower_left,
                  DIRECTION_UP,
                  trail);
  perimeter_trail.insert(perimeter_trail.end(), trail.begin(), trail.end());
  trail.clear();
  crawlToPosition(zone,
                  lower_left,
                  upper_left,
                  DIRECTION_RIGHT,
                  trail);
  perimeter_trail.insert(perimeter_trail.end(), trail.begin(), trail.end());

  // step2: walk the perimeter and find any adjacent doors
  RPG_Map_Positions_t door_positions;
  for (RPG_Map_DoorsConstIterator_t iterator = floorPlan_in.doors.begin();
       iterator != floorPlan_in.doors.end();
       iterator++)
    door_positions.insert((*iterator).position);
  RPG_Map_PositionsConstIterator_t door_iterator;
  for (RPG_Map_PositionListConstIterator_t iterator = perimeter_trail.begin();
       iterator != perimeter_trail.end();
       iterator++)
    for (door_iterator = door_positions.begin();
         door_iterator != door_positions.end();
         door_iterator++)
      if (isAdjacent(*iterator, *door_iterator))
      {
        result++;
        continue;
      } // end IF

  return result;
}

std::string
RPG_Map_Common_Tools::getMapsDirectory()
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Common_Tools::getMapsDirectory"));

  std::string result;

  std::string data_path = RPG_Common_File_Tools::getWorkingDirectory();
#ifdef BASEDIR
  data_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           false);
#endif // #ifdef BASEDIR
	result = data_path;
	result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
	result += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_MAP_SUB);
	result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
	result += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DATA_SUB);
#else
  result += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_MAPS_SUB);
#endif

  if (!RPG_Common_File_Tools::isDirectory(result))
  {
    if (!RPG_Common_File_Tools::createDirectory(result))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_File_Tools::createDirectory(\"%s\"), falling back\n"),
                 ACE_TEXT(result.c_str())));

      // fallback
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
      result = ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DEF_DUMP_DIR);
#else
      result = ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DEF_DUMP_DIR));
#endif
    } // end IF
    else
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("created maps directory \"%s\"\n"),
                 ACE_TEXT(result.c_str())));
  } // end IF

  return result;
}
