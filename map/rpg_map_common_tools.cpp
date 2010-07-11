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
#include "rpg_map_pathfinding_tools.h"

#include <rpg_common_file_tools.h>

#include <rpg_dice.h>

#include <ace/FILE_Addr.h>
#include <ace/FILE_Connector.h>
#include <ace/FILE.h>
#include <ace/FILE_IO.h>
#include <ace/Log_Msg.h>

#include <sstream>
#include <iostream>

void
RPG_Map_Common_Tools::load(const std::string& filename_in,
                           RPG_Map_Positions_t& seedPoints_out,
                           RPG_Map_FloorPlan_t& level_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::load"));

  // sanity check(s)
  if (!RPG_Common_File_Tools::isReadable(filename_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_File_Tools::isReadable(\"%s\"), aborting\n"),
               filename_in.c_str()));

    return;
  } // end IF

  ACE_FILE_IO fileStream;
  ACE_FILE_Addr file_addr(ACE_TEXT_CHAR_TO_TCHAR(filename_in.c_str()));
  // don't want to block on file creation (see ACE doc)...
  ACE_Time_Value zero_timeout(0, 0);
  ACE_FILE_Connector file_connector;
  if (file_connector.connect(fileStream,
                             file_addr,
                             &zero_timeout,
                             ACE_Addr::sap_any,
                             0,
                             (O_RDONLY | O_TEXT), // read-only textmode
                             ACE_DEFAULT_FILE_PERMS) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_Connector::connect(\"%s\"): %m, aborting\n"),
               filename_in.c_str()));

    return;
  } // end IF

  // retrieve status info...
  ACE_FILE_Info info;
  if (fileStream.get_info(info) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_IO::get_info(): %m, aborting\n")));

    // clean up
    if (fileStream.close() == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_FILE_IO::close(): %m, aborting\n")));
    } // end IF

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("openend file (%u bytes): \"%s\"...\n"),
             filename_in.c_str(),
             ACE_static_cast(unsigned long, info.size_)));

  // *TODO*: parse file
  ACE_ASSERT(false);

  // clean up
  if (fileStream.close() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_IO::close(): %m, aborting\n")));

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("closed target file: \"%s\"...\n"),
             filename_in.c_str()));
}

void
RPG_Map_Common_Tools::save(const std::string& filename_in,
                           const RPG_Map_Positions_t& seedPoints_in,
                           const RPG_Map_FloorPlan_t& level_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::save"));

  // sanity check(s)
  if (RPG_Common_File_Tools::isReadable(filename_in))
    if (!RPG_Common_File_Tools::deleteFile(filename_in))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_File_Tools::deleteFile(\"%s\"), aborting\n"),
                 filename_in.c_str()));

      return;
    } // end IF

  ACE_FILE_IO fileStream;
  ACE_FILE_Addr file_addr(ACE_TEXT_CHAR_TO_TCHAR(filename_in.c_str()));
  // don't block on file creation (see ACE doc)...
  ACE_Time_Value zero_timeout(0, 0);
  ACE_FILE_Connector file_connector;
  if (file_connector.connect(fileStream,
                             file_addr,
                             &zero_timeout,
                             ACE_Addr::sap_any,
                             0,
                             (O_WRONLY | O_APPEND | O_CREAT | O_TRUNC | O_TEXT), // clobber existing files...
                             ACE_DEFAULT_FILE_PERMS) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_Connector::connect(\"%s\"): %m, aborting\n"),
               filename_in.c_str()));

    return;
  } // end IF

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("created/openend TXT file: \"%s\"...\n"),
//              filename_in.c_str()));

  ssize_t sent_bytes = 0;
  std::string row;
  RPG_Map_Position_t current_position;
  for (unsigned long y = 0;
       y < level_in.size_y;
       y++)
  {
    for (unsigned long x = 0;
         x < level_in.size_x;
         x++)
    {
      current_position = std::make_pair(x, y);

      // seed point ?
      if (seedPoints_in.find(current_position) != seedPoints_in.end())
      {
        row += '@'; // seed

        continue;
      } // end IF

      // unmapped, floor, wall, or door ?
      if (level_in.unmapped.find(current_position) != level_in.unmapped.end())
        row += ' '; // unmapped
      else if (level_in.walls.find(current_position) != level_in.walls.end())
        row += '#'; // wall
      else if (level_in.doors.find(current_position) != level_in.doors.end())
        row += '='; // door
      else
        row += '.'; // floor
    } // end FOR
    row += ACE_TEXT_ALWAYS_CHAR("\n");

    // write the row to the file
    sent_bytes = fileStream.send(row.c_str(),
                                 row.size());
    switch (sent_bytes)
    {
      case -1:
      case 0:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_FILE_IO::send(%u): %m, aborting\n"),
                   row.size()));

        // clean up
        if (fileStream.close() == -1)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to ACE_FILE_IO::close(): %m, aborting\n")));
        } // end IF

        return;
      }
      default:
      {
        if (ACE_static_cast(size_t, sent_bytes) != row.size())
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to ACE_FILE_IO::send(%u) (result was: %d), aborting\n"),
                     row.size(),
                     sent_bytes));

          // clean up
          if (fileStream.close() == -1)
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("failed to ACE_FILE_IO::close(): %m, aborting\n")));
          } // end IF

          return;
        } // end IF

        break;
      }
    } // end SWITCH

    row.clear();
  } // end FOR

  // retrieve status info...
  ACE_FILE_Info info;
  if (fileStream.get_info(info) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_IO::get_info(): %m, aborting\n")));

    // clean up
    if (fileStream.close() == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_FILE_IO::close(): %m, aborting\n")));
    } // end IF

    return;
  } // end IF

  // unlink empty files
  if (info.size_ == 0)
  {
    // --> remove() instead of close()...
    if (fileStream.remove() == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_FILE_IO::remove(): %m, aborting\n")));

      return;
    } // end IF

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("removed empty TXT file \"%s\"...\n"),
               filename_in.c_str()));

    return;
  } // end IF

  // OK (not empty)
  if (fileStream.close() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_IO::close(): %m, aborting\n")));

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("wrote TXT file \"%s\" (%u byte(s))...\n"),
             filename_in.c_str(),
             ACE_static_cast(unsigned long, info.size_)));
}

void
RPG_Map_Common_Tools::createFloorPlan(const unsigned long& dimensionX_in,
                                      const unsigned long& dimensionY_in,
                                      const unsigned long& numAreas_in,
                                      const bool& wantSquareRooms_in,
                                      const bool& maximizeRooms_in,
                                      const unsigned long& minRoomArea_in,
                                      const bool& wantDoors_in,
                                      const bool& wantCorridors_in,
                                      const bool& doorFillsPosition_in,
                                      const unsigned long& maxDoorsPerRoom_in,
                                      RPG_Map_Positions_t& seedPoints_out,
                                      RPG_Map_FloorPlan_t& level_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::createFloorPlan"));

  // init return value(s)
  seedPoints_out.clear();
  level_out.size_x = dimensionX_in;
  level_out.size_y = dimensionY_in;
  level_out.unmapped.clear();
  level_out.walls.clear();
  level_out.doors.clear();

  RPG_Map_Partition_t partition;
  RPG_Map_Positions_t conflicts;
  RPG_Map_ZoneList_t boundaries, rooms, doors;
  bool some_rooms_empty = true;
  bool no_rooms_empty = false;
  unsigned long index = 1;
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
    makePartition(dimensionX_in,
                  dimensionY_in,
                  numAreas_in,
                  (wantCorridors_in ? true : false), // resolve conflicts !
                  conflicts,
                  seedPoints_out,
                  partition);
    if (wantDoors_in)
    {
      // step2: form rooms within partition(s)
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
      // check: all rooms are not empty ?
      no_rooms_empty = true;
      for (RPG_Map_ZoneListConstIterator_t zonelist_iter = rooms.begin();
          zonelist_iter != rooms.end();
          zonelist_iter++)
        if ((*zonelist_iter).empty())
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
        makeDoors(dimensionX_in,
                  dimensionY_in,
                  boundaries,
                  doorFillsPosition_in,
                  maxDoorsPerRoom_in,
                  doors);
      } // end ELSE
    } // end IF
    else
      break; // there are no rooms...

    index++;
  } while (some_rooms_empty);

  if (wantCorridors_in)
  {
//     // debug info
//     displayRooms(dimensionX_in,
//                  dimensionY_in,
//                  rooms);

    // step3: create corridors_bounds
    connectRooms(dimensionX_in,
                 dimensionY_in,
                 boundaries,
                 doors,
                 rooms,
                 level_out);
  } // end IF
  else
  {
    // *NOTE*: in this case, conflict areas represent walls...
    // *TODO*: what if some areas are not reachable ?
    // --> create some doors ?
    level_out.walls = conflicts;
  } // end ELSE
}

void
RPG_Map_Common_Tools::displayFloorPlan(const RPG_Map_FloorPlan_t& level_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::displayFloorPlan"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("level ([%ux%u] - %u unmapped, %u walls, %u doors)...\n"),
             level_in.size_x,
             level_in.size_y,
             level_in.unmapped.size(),
             level_in.walls.size(),
             level_in.doors.size()));

  RPG_Map_Position_t current_position;
  for (unsigned long y = 0;
       y < level_in.size_y;
       y++)
  {
    for (unsigned long x = 0;
         x < level_in.size_x;
         x++)
    {
      current_position = std::make_pair(x, y);

      // unmapped, floor, wall, or door ?
      if (level_in.unmapped.find(current_position) != level_in.unmapped.end())
        std::cout << ACE_TEXT(" "); // unmapped
      else if (level_in.walls.find(current_position) != level_in.walls.end())
        std::cout << ACE_TEXT("#"); // wall
      else if (level_in.doors.find(current_position) != level_in.doors.end())
        std::cout << ACE_TEXT("="); // door
      else
        std::cout << ACE_TEXT("."); // floor
    } // end FOR
    std::cout << std::endl;
  } // end FOR
}

void
RPG_Map_Common_Tools::makePartition(const unsigned long& dimensionX_in,
                                    const unsigned long& dimensionY_in,
                                    const unsigned long& numRooms_in,
                                    const bool& resolveConflicts_in,
                                    RPG_Map_Positions_t& conflicts_out,
                                    RPG_Map_Positions_t& seedPoints_out,
                                    RPG_Map_Partition_t& partition_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::makePartition"));

  // init return value(s)
  seedPoints_out.clear();
  conflicts_out.clear();
  partition_out.clear();

  // step1: distribute numRooms_in "seed" points randomly across the plane
  RPG_Dice_RollResult_t result_x, result_y;
  do
  {
    seedPoints_out.clear();
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
      seedPoints_out.insert(std::make_pair(result_x[i] - 1,
                                           result_y[i] - 1));
      // enough data ?
      if (seedPoints_out.size() == numRooms_in)
        break;
    } // end FOR
    // enough data ?
    if (seedPoints_out.size() == numRooms_in)
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
  for (seed_iter = seedPoints_out.begin();
       seed_iter != seedPoints_out.end();
       seed_iter++, index++)
  {
    zone.clear();
    zone.insert(*seed_iter);
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
  unsigned long min = 0;
  unsigned long distance = 0;
  RPG_Map_Positions_t neighbours;
  RPG_Map_Position_t current;
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
      for (seed_iter = seedPoints_out.begin();
           seed_iter != seedPoints_out.end();
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
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("conflict for (%u,%u): %u nearest neighbours\n"),
//                    x, y,
//                    neighbours.size()));

        // conflict
        conflicts_out.insert(current);

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
  if (resolveConflicts_in)
  {
    while (!conflicts_out.empty())
    {
  //     // debug info
  //     displayPartition(dimensionX_in,
  //                      dimensionY_in,
  //                      conflicts_out,
  //                      seed_points,
  //                      partition_out);

      for (conflict_iter = conflicts_out.begin();
           conflict_iter != conflicts_out.end();
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
        for (seed_iter = seedPoints_out.begin();
             seed_iter != seedPoints_out.end();
             seed_iter++)
        {
          if ((*member_partition).find(*seed_iter) != (*member_partition).end())
          {
            current_seed = *seed_iter;
            break;
          } // end IF
        } // end FOR
        // sanity check
        ACE_ASSERT(seed_iter != seedPoints_out.end());
        ACE_ASSERT(seedPoints_out.find(current_seed) != seedPoints_out.end());

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
          conflicts_out.erase(current);
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
        min = std::numeric_limits<unsigned long>::max();
        neighbours.clear();
        for (seed_iter = seedPoints_out.begin();
            seed_iter != seedPoints_out.end();
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
  } // end IF

//   // debug info
//   displayPartition(dimensionX_in,
//                    dimensionY_in,
//                    conflicts_out,
//                    seedPoints_out,
//                    partition_out);
}

void
RPG_Map_Common_Tools::displayPartition(const unsigned long& dimensionX_in,
                                       const unsigned long& dimensionY_in,
                                       const RPG_Map_Positions_t& conflicts_in,
                                       const RPG_Map_Positions_t& seedPositions_in,
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
RPG_Map_Common_Tools::findMaxSquare(const RPG_Map_Zone_t& room_in,
                                    RPG_Map_Square_t& maxSquare_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::findMaxSquare"));

  // init return value(s)
  maxSquare_out.ul = std::make_pair(0, 0);
  maxSquare_out.lr = std::make_pair(0, 0);

  RPG_Map_ZoneConstIterator_t pointer;
  unsigned long maxArea = 0;
  unsigned long current_area = 0;
  unsigned long max_breadth = 0;
  unsigned long current_row = 0;
  unsigned long running_x = 0;
  for (RPG_Map_ZoneConstIterator_t zone_iterator = room_in.begin();
       zone_iterator != room_in.end();
       zone_iterator++)
  {
    max_breadth = 0;
    current_row = (*zone_iterator).second;
    running_x = (*zone_iterator).first;
    for (RPG_Map_ZoneConstIterator_t zone_iterator2 = zone_iterator;
         zone_iterator2 != room_in.end();
         zone_iterator2++)
    {
      // still on the top row ?
      if ((*zone_iterator2).second == ((*zone_iterator).second))
      {
        // check: for consecutive positions WITHIN the same row:
        //        if (previous_cell.x + 1) is NOT EQUAL to the current.x
        //        i.e. the current cell is not adjacent to the previous one
        //        --> skip forward to the next row (and resume checking !)
        if (running_x != (*zone_iterator2).first)
        {
          while ((zone_iterator2 != room_in.end()) &&
                 ((*zone_iterator2).second == current_row))
            zone_iterator2++;

          // finished with this position ?
          if (zone_iterator2 == room_in.end())
            break; // start next position

          // check next cell
          continue;
        } // end IF
        else
          max_breadth++; // current cell is adjacent to the last one
        running_x++; // adjacent cell must have this x-coordinate

        // --> compute enclosed area and continue
        current_area = area2Positions(*zone_iterator, *zone_iterator2);
        if (maxArea < current_area)
        {
          maxArea = current_area;
          maxSquare_out.ul = *zone_iterator;
          maxSquare_out.lr = *zone_iterator2;
        } // end IF

        // check next cell
        continue;
      } // end IF

      // we're NOT on the top row anymore...
      ACE_ASSERT((*zone_iterator2).second != (*zone_iterator).second);

      // step1: compute max. breadth for current position
      // --> has been done implicitely while processing the top row (see above)
      ACE_ASSERT(max_breadth);

      // check: IMMEDIATELY on breaking to a new row:
      //        if our position.x is LARGER than our current_square.ul.x
      //        --> already found the largest square for this position
      if ((*zone_iterator2).second > current_row)
      {
        running_x = (*zone_iterator2).first;
        current_row++;
        if ((*zone_iterator2).second != current_row)
        {
          // this room is NOT compact (reason: cropped ?)
          // --> already found the largest square for this position
          break; // start next position
        } // end IF

        if ((*zone_iterator2).first > (*zone_iterator).first)
          break; // start next position

        // retrieve the last cell of the previous row
        pointer = zone_iterator2;
        pointer--;

        // optimization: if last_cell.x of the previous row was SMALLER than
        //               current_square.ul.x
        //               --> already found the largest square for this position
        if ((*pointer).first < (*zone_iterator).first)
          break; // start next position

        // otherwise, perhaps max_breadth needs a reduction...
        if ((((*pointer).first - (*zone_iterator).first) + 1) < max_breadth)
          max_breadth = (((*pointer).first - (*zone_iterator).first) + 1);
        ACE_ASSERT(max_breadth);
      } // end IF
      else
      {
        // as long as the current_cell.x is EQUAL OR SMALLER than
        // current_square.ul.x (i.e. we're left/at the reference position),
        // ignore any skips between consecutive positions (see below)
        if ((*zone_iterator2).first <= (*zone_iterator).first)
          running_x = (*zone_iterator2).first;
        else
          running_x++;
      } // end ELSE

      // check: check if we're within the square spanned by
      //        [current_square.ul,(current_square.ul.x + (max_breadth - 1),y)]
      if (((*zone_iterator2).first > ((*zone_iterator).first + max_breadth - 1)) ||
          ((*zone_iterator2).first < (*zone_iterator).first))
        continue; // no, we're not --> check next cell

      // we're within the current square

      // check: for consecutive positions WITHIN the same row:
      //        if (previous_cell.x + 1) is NOT EQUAL to the current.x
      //        i.e. the current cell is not adjacent to the previous one
      //        --> already found the largest square for this position
      if (running_x != (*zone_iterator2).first)
        break; // start next position

      // --> compute enclosed area
      current_area = area2Positions(*zone_iterator, *zone_iterator2);
      if (maxArea < current_area)
      {
        maxArea = current_area;
        maxSquare_out.ul = *zone_iterator;
        maxSquare_out.lr = *zone_iterator2;
      } // end IF
    } // end FOR
  } // end FOR
}

void
RPG_Map_Common_Tools::makeRooms(const unsigned long& dimensionX_in,
                                const unsigned long& dimensionY_in,
                                const RPG_Map_Partition_t& partition_in,
                                const bool& wantSquareRooms_in,
                                const bool& wantRoomSeparation_in,
                                const bool& cropAreas_in,
                                const bool& maximizeRooms_in,
                                const unsigned long& minRoomArea_in,
                                RPG_Map_ZoneList_t& rooms_out,
                                RPG_Map_ZoneList_t& boundaries_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::makeRooms"));

  // init return value(s)
  rooms_out.clear();
  boundaries_out.clear();

  // place a room into every partition...
  unsigned long index = 0;
  RPG_Map_Zone_t current_zone;

  // step0: make some preparations...
  if (cropAreas_in)
  {
    // --> remove any cruft from the perimeter
    index = 0;
    unsigned long last_size = 0;
    for (RPG_Map_PartitionConstIterator_t partition_iter = partition_in.begin();
         partition_iter != partition_in.end();
         partition_iter++, index++)
    {
      current_zone.clear();

      // make a copy...
      current_zone = *partition_iter;

      // *NOTE*: crop until the size is stable
      // i.e. if (min. breadth/width is <3/3), the whole room will be cropped
      // --> too slim/flat...
      do
      {
        last_size = current_zone.size();
        crop(current_zone);
      } while (last_size > current_zone.size());

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

      rooms_out.push_back(current_zone);
    } // end FOR
  } // end IF

  RPG_Map_SquareList_t maxSquares;
  RPG_Map_ZoneListConstIterator_t zonelist_iter;
  if (wantSquareRooms_in)
  {
    // make square room(s)
    RPG_Map_Square_t maxSquare;
    index = 0;
    for (zonelist_iter = (cropAreas_in ? rooms_out.begin()
                                       : partition_in.begin());
         zonelist_iter != (cropAreas_in ? rooms_out.end()
                                        : partition_in.end());
         zonelist_iter++, index++)
    {
      maxSquare.ul = std::make_pair(0, 0);
      maxSquare.lr = std::make_pair(0, 0);

      // sanity check
      if ((*zonelist_iter).empty())
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("max. square[%u]: doesn't exist\n"),
                   index));
      } // end IF
      else
      {
        // find coordinates of the maximum-size square room(s)
        // --> can fit in smaller rooms anytime...
        findMaxSquare(*zonelist_iter, maxSquare);

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

  // step1: create rooms
  RPG_Map_ZoneListIterator_t zones_iter;
  RPG_Map_ZoneConstIterator_t zone_iter;
  if (maximizeRooms_in)
  {
    if (wantSquareRooms_in)
    {
      index = 0;
      RPG_Map_SquareListConstIterator_t squares_iter = maxSquares.begin();
      for (zones_iter = (cropAreas_in ? rooms_out.begin()
                                      : ACE_const_cast(RPG_Map_Partition_t&, partition_in).begin());
           zones_iter != (cropAreas_in ? rooms_out.end()
                                       : ACE_const_cast(RPG_Map_Partition_t&, partition_in).end());
           zones_iter++, squares_iter++, index++)
      {
        current_zone.clear();
        for (zone_iter = (*zones_iter).begin();
             zone_iter != (*zones_iter).end();
             zone_iter++)
        {
          if (positionInSquare(*zone_iter, *squares_iter))
            current_zone.insert(*zone_iter);
        } // end FOR

        // sanity check
        ACE_ASSERT((current_zone.empty() ? true // that's OK (but area2Positions yields 1)...
                                         : current_zone.size() == area2Positions((*squares_iter).ul,
                                                                                 (*squares_iter).lr)));

        if (cropAreas_in)
          *zones_iter = current_zone; // do in-place editing...
        else
          rooms_out.push_back(current_zone);

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
      rooms_out = partition_in;
    } // end ELSE
  } // end IF
  else
  {
    // *NOTE*: rooms may have been cropped...
    // --> nothing to do...
  } // end ELSE

  // step2: enforce any other constraint(s)

  // minimal area
  if (minRoomArea_in)
  {
    index = 0;
    for (zones_iter = rooms_out.begin();
         zones_iter != rooms_out.end();
         zones_iter++, index++)
    {
      if ((*zones_iter).size() < minRoomArea_in)
      {
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("zone[%u] is too small (%u < %u)...\n"),
//                    index,
//                    (*zones_iter).size(),
//                    minArea_in));

        // crop room (what else can we do ?)
        (*zones_iter).clear();
      } // end IF
    } // end FOR
  } // end IF

  if (wantRoomSeparation_in)
  {
    // enforce a minimal separation to fit in corridors_bounds
    RPG_Map_Position_t up, right, down, left;
    RPG_Map_Directions_t crop_walls;
    index = 0;
    for (zones_iter = rooms_out.begin();
         zones_iter != rooms_out.end();
         zones_iter++, index++)
    {
      crop_walls.clear();
      for (zone_iter = (*zones_iter).begin();
           zone_iter != (*zones_iter).end();
           zone_iter++)
      {
        // compute neighbours
        up = *zone_iter; up.second -= ((up.second == 0) ? 0 : 1);
        right = *zone_iter; right.first += ((right.first == (dimensionX_in - 1)) ? 0 : 1);
        down = *zone_iter; down.second += ((down.second == (dimensionY_in - 1)) ? 0 : 1);
        left = *zone_iter; left.first -= ((left.first == 0) ? 0 : 1);

        // (direct) contact with ANOTHER room ?
        for (zonelist_iter = rooms_out.begin();
             zonelist_iter != rooms_out.end();
             zonelist_iter++)
        {
          // skip the CURRENT room
          if (*zonelist_iter == *zones_iter)
            continue;

          // *NOTE*: if there is contact, resize the larger one...
          // *TODO*: if this would crop the entire room, we should try to crop
          // the smaller one instead...
          if ((*zones_iter).size() < (*zonelist_iter).size())
            continue;

          if ((*zonelist_iter).find(up) != (*zonelist_iter).end())
            crop_walls.insert(UP);
          if ((*zonelist_iter).find(right) != (*zonelist_iter).end())
            crop_walls.insert(RIGHT);
          if ((*zonelist_iter).find(down) != (*zonelist_iter).end())
            crop_walls.insert(DOWN);
          if ((*zonelist_iter).find(left) != (*zonelist_iter).end())
            crop_walls.insert(LEFT);
        } // end FOR
      } // end FOR

      // if necessary, crop the room
      if (!crop_walls.empty())
      {
        RPG_Map_Position_t upper_left, upper_right, lower_left, lower_right;
        RPG_Map_PositionsConstIterator_t first, last;
        for (RPG_Map_DirectionsConstIterator_t crop_iter = crop_walls.begin();
             crop_iter != crop_walls.end();
             crop_iter++)
        {
          switch (*crop_iter)
          {
            case UP:
            {
              // erase top row
              first = (*zones_iter).begin();
              last = (*zones_iter).begin();
              while ((*last).second == (*first).second)
                last++;
              // *NOTE*: std::set::erase removes [first, last) !
              (*zones_iter).erase(first, last);
//               // create new top row
//               first = last;
//               last++;
//               for (unsigned long i = 1;
//                    i <= ((*last).first - (*first).first);
//                    i++)
//                 (*zones_iter).insert(std::make_pair(((*first).first + i), (*first).second));

//               ACE_DEBUG((LM_DEBUG,
//                          ACE_TEXT("zone[%u]: cropped top row...\n"),
//                          index));

              break;
            }
            case RIGHT:
            {
              // erase rightmost column
              last = (*zones_iter).end(); last--;
              unsigned long column_x = (*last).first;
              for (RPG_Map_ZoneIterator_t zone_iter2 = (*zones_iter).begin();
                   zone_iter2 != (*zones_iter).end();
                   zone_iter2++)
              {
                if ((*zone_iter2).first == column_x)
                  (*zones_iter).erase(zone_iter2);
              } // end FOR
//               // create new rightmost column
//               first = (*zones_iter).begin();
//               last = (*zones_iter).end(); last--;
//               for (unsigned long i = 1;
//                    i <= ((*last).second - (*first).second);
//                    i++)
//                 (*zones_iter).insert(std::make_pair((*last).first, ((*last).second - i)));

//               ACE_DEBUG((LM_DEBUG,
//                          ACE_TEXT("zone[%u]: cropped rightmost column...\n"),
//                          index));

              break;
            }
            case DOWN:
            {
              // erase bottom row
              first = (*zones_iter).begin();
              last = (*zones_iter).end(); last--;
              while ((*first).second != (*last).second)
                first++;
              // *NOTE*: std::set::erase removes [first, last) !
              (*zones_iter).erase(first, (*zones_iter).end());
//               // create new bottom row
//               last = (*zones_iter).end(); last--;
//               first = last;
//               first--;
//               for (unsigned long i = 1;
//                    i <= ((*last).first - (*first).first);
//                    i++)
//                 (*zones_iter).insert(std::make_pair(((*first).first + i), (*first).second));

//               ACE_DEBUG((LM_DEBUG,
//                          ACE_TEXT("zone[%u]: cropped bottom row...\n"),
//                          index));

              break;
            }
            case LEFT:
            {
              // erase leftmost column
              first = (*zones_iter).begin();
              unsigned long column_x = (*first).first;
              for (RPG_Map_ZoneIterator_t zone_iter2 = (*zones_iter).begin();
                   zone_iter2 != (*zones_iter).end();
                   zone_iter2++)
              {
                if ((*zone_iter2).first == column_x)
                  (*zones_iter).erase(zone_iter2);
              } // end FOR
//               // create new leftmost column
//               first = (*zones_iter).begin();
//               last = (*zones_iter).end(); last--;
//               for (unsigned long i = 1;
//                    i <= ((*last).second - (*first).second);
//                    i++)
//                 (*zones_iter).insert(std::make_pair((*first).first, ((*last).second - i)));

//               ACE_DEBUG((LM_DEBUG,
//                          ACE_TEXT("zone[%u]: cropped leftmost column...\n"),
//                          index));

              break;
            }
            default:
            {
              // debug info
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("invalid direction (was %u), continuing\n"),
                         *crop_iter));

              // *TODO*: what else can we do ?
              ACE_ASSERT(false);

              break;
            }
          } // end SWITCH
        } // end FOR

        // (re-)crop the entire room...
//         cropSquareBoundary(*zones_iter);
        crop(*zones_iter);

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

  // step3: compute the boundary
  RPG_Map_ZoneConstIterator_t last;
  RPG_Map_Position_t upper_left;
  RPG_Map_Position_t upper_right;
  RPG_Map_Position_t lower_right;
  RPG_Map_Position_t lower_left;
  RPG_Map_PositionList_t trail;
  RPG_Map_PositionListConstIterator_t trail_iter;
  index = 0;
  for (zones_iter = rooms_out.begin();
       zones_iter != rooms_out.end();
       zones_iter++, index++)
  {
    current_zone.clear();

    // step0: sanity check
    if ((*zones_iter).empty())
      continue; // nothing to do...

    zone_iter = (*zones_iter).begin();
    last = (*zones_iter).end(); last--;
    // step1: compute the 4 corners
    // upper left == first element
    upper_left = *zone_iter;
    // upper right == last element of the first row
    while ((zone_iter != (*zones_iter).end()) &&
           ((*zone_iter).second == upper_left.second))
      zone_iter++;
    zone_iter--;
    upper_right = *zone_iter;
    // lower_right == last element
    lower_right = *last;
    // lower_left == first element of the last row
    if (upper_left.second == lower_right.second)
      lower_left = upper_left;
    else
    {
      // there's more than one row
      zone_iter = last;
      while ((*zone_iter).second == lower_right.second)
        zone_iter--;
      zone_iter++;
      lower_left = *zone_iter;
    } // end ELSE

    // step2: start at top left position and go around clockwise
    trail.clear();
    crawlToPosition(*zones_iter,
                    upper_left,
                    upper_right,
                    DOWN,
                    trail);
    for (trail_iter = trail.begin();
         trail_iter != trail.end();
         trail_iter++)
      current_zone.insert(*trail_iter);
    trail.clear();
    crawlToPosition(*zones_iter,
                    upper_right,
                    lower_right,
                    LEFT,
                    trail);
    for (trail_iter = trail.begin();
         trail_iter != trail.end();
         trail_iter++)
      current_zone.insert(*trail_iter);
    trail.clear();
    crawlToPosition(*zones_iter,
                    lower_right,
                    lower_left,
                    UP,
                    trail);
    for (trail_iter = trail.begin();
         trail_iter != trail.end();
         trail_iter++)
      current_zone.insert(*trail_iter);
    trail.clear();
    crawlToPosition(*zones_iter,
                    lower_left,
                    upper_left,
                    RIGHT,
                    trail);
    for (trail_iter = trail.begin();
         trail_iter != trail.end();
         trail_iter++)
      current_zone.insert(*trail_iter);

//     *zones_iter = current_zone; // do in-place editing...
    boundaries_out.push_back(current_zone);
  } // end FOR
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
RPG_Map_Common_Tools::makeDoors(const unsigned long& dimensionX_in,
                                const unsigned long& dimensionY_in,
                                const RPG_Map_ZoneList_t& boundaries_in,
                                const bool& doorFillsPosition_in,
                                const unsigned long& maxDoorsPerRoom_in,
                                RPG_Map_ZoneList_t& doors_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::makeDoors"));

  // init return value(s)
  doors_out.clear();

  RPG_Map_ZoneListConstIterator_t zonelist_iter;
  // *NOTE*: every room needs at least one (possibly secret) door
  // *NOTE*: doors cannot be situated on the boundary of the level
  // *NOTE*: doors can connect rooms directly (i.e. without a corridor)
  // *NOTE*: to ensure connectivity for n rooms, we need at least (n-1)*2 doors...
  unsigned long total_doors = 0;
  unsigned long index = 0;

  RPG_Map_Zone_t current_doors;
  unsigned long num_doors = 0;
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
      if (maxDoorsPerRoom_in == std::numeric_limits<unsigned long>::max())
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
RPG_Map_Common_Tools::connectRooms(const unsigned long& dimensionX_in,
                                   const unsigned long& dimensionY_in,
                                   const RPG_Map_ZoneList_t& boundaries_in,
                                   const RPG_Map_ZoneList_t& doors_in,
                                   const RPG_Map_ZoneList_t& rooms_in,
                                   RPG_Map_FloorPlan_t& level_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::connectRooms"));

  // init return value(s)
//   level_out.size_x = dimensionX_in;
//   level_out.size_y = dimensionY_in;
  level_out.unmapped.clear();
  level_out.walls.clear();
  level_out.doors.clear();

  RPG_Map_ZoneListConstIterator_t zonelist_iter;
  RPG_Map_ZoneConstIterator_t zone_iter;
  // step0: add room walls
  for (zonelist_iter = boundaries_in.begin();
       zonelist_iter != boundaries_in.end();
       zonelist_iter++)
    for (zone_iter = (*zonelist_iter).begin();
         zone_iter != (*zonelist_iter).end();
         zone_iter++)
      level_out.walls.insert(*zone_iter);
  // step1: add the doors
  unsigned long index = 0;
  unsigned long index2 = 0;
  RPG_Map_ZoneConstIterator_t doors_iterator;
  for (zonelist_iter = doors_in.begin();
       zonelist_iter != doors_in.end();
       zonelist_iter++, index++)
  {
    index2 = 0;
    for (doors_iterator = (*zonelist_iter).begin();
         doors_iterator != (*zonelist_iter).end();
         doors_iterator++, index2++)
    {
      level_out.doors.insert(*doors_iterator);

//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("door[%u,%u] at (%u,%u)\n"),
//                  index, index2,
//                  (*doors_iterator).first,
//                  (*doors_iterator).second));
    } // end FOR
  } // end FOR

  // step2: make corridors
  RPG_Map_ZoneList_t corridors_bounds;
  RPG_Map_ZoneList_t corridors;
  RPG_Map_ZoneListConstIterator_t rooms_iter, zonelist_iter2;
  RPG_Map_ZoneConstIterator_t doors_iter, doors_iter2;
  RPG_Map_Zone_t current_corridor;
  RPG_Map_Positions_t possible_positions, used_positions;
  RPG_Dice_RollResult_t result;
  RPG_Map_PositionsConstIterator_t target_door;
  RPG_Map_Path_t current_path;
  RPG_Map_Position_t wall_position1, wall_position2, last_position;
  RPG_Map_Direction last_direction = INVALID;
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
      if (!RPG_Map_Pathfinding_Tools::findPath(dimensionX_in,
                                               dimensionY_in,
                                               level_out.walls,
                                               *doors_iter,
                                               door2exitDirection(*rooms_iter,
                                                                  *doors_iter),
                                               *target_door,
                                               current_path))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("cannot find path from (%u,%u) to (%u,%u), continuing\n"),
                   (*doors_iter).first,
                   (*doors_iter).second,
                   (*target_door).first,
                   (*target_door).second));
      } // end IF
//         else
//         {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("found path from (%u,%u) to (%u,%u)\n"),
//                      (*doors_iter).first,
//                      (*doors_iter).second,
//                      (*target_door).first,
//                      (*target_door).second));
//         } // end ELSE

      used_positions.insert(*doors_iter);
      used_positions.insert(*target_door);
      if ((used_positions == level_out.doors) &&
          (!connectivity_established))
      {
        // check: when minimal connectivity has been reached (i.e. every door has
        //        been attached to some corridor), relax this constraint
        used_positions.clear();

//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("connectivity has been established (%u path(s))...\n"),
//                      (paths.size() + 1)));

        connectivity_established = true;
      } // end IF

      // step3: create a corridor along this path
      last_position = (*doors_iter);
      last_direction = (*current_path.begin()).second;
      path_iter = current_path.begin(); path_iter++;
      last = current_path.end(); last--;
      for (;
           path_iter != last;
           path_iter++)
      {
        // when turning, add additional walls at the last position:
        // - in the direction of last travel
        // - from there, a corner in the OPPOSITE direction of CURRENT travel
        if ((*path_iter).second != last_direction)
        {
          wall_position1 = last_position;
          switch (last_direction)
          {
            case UP:
              wall_position1.second--; break;
            case DOWN:
              wall_position1.second++; break;
            case RIGHT:
              wall_position1.first++; break;
            case LEFT:
              wall_position1.first--; break;
            default:
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("invalid direction (was \"%s\", %u), continuing\n"),
                         direction2String(last_direction).c_str(),
                         last_direction));

              break;
            }
          } // end SWITCH
          wall_position2 = wall_position1;
          switch ((*path_iter).second)
          {
            case UP:
              wall_position2.second++; break;
            case DOWN:
              wall_position2.second--; break;
            case RIGHT:
              wall_position2.first--; break;
            case LEFT:
              wall_position2.first++; break;
            case INVALID:
            {
              // reached the endpoint ?
              if ((*path_iter).first == *target_door)
                break; // done
            }
            default:
            {
              ACE_DEBUG((LM_ERROR,
                          ACE_TEXT("invalid direction (was \"%s\", %u), continuing\n"),
                          direction2String((*path_iter).second).c_str(),
                          (*path_iter).second));

              break;
            }
          } // end SWITCH
          current_corridor.insert(wall_position1);
          current_corridor.insert(wall_position2);
        } // end IF
        wall_position1 = last_position;
        wall_position2 = last_position;
        switch ((*path_iter).second)
        {
          case UP:
          {
            wall_position1.first--;
            wall_position1.second--;
            wall_position2.first++;
            wall_position2.second--;

            last_position.second--;

            break;
          }
          case DOWN:
          {
            wall_position1.first--;
            wall_position1.second++;
            wall_position2.first++;
            wall_position2.second++;

            last_position.second++;

            break;
          }
          case RIGHT:
          {
            wall_position1.first++;
            wall_position1.second--;
            wall_position2.first++;
            wall_position2.second++;

            last_position.first++;

            break;
          }
          case LEFT:
          {
            wall_position1.first--;
            wall_position1.second--;
            wall_position2.first--;
            wall_position2.second++;

            last_position.first--;

            break;
          }
          case INVALID:
          {
            // reached the endpoint ?
            if ((*path_iter).first == *target_door)
              break; // done
          }
          default:
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("invalid direction (was \"%s\", %u), continuing\n"),
                       direction2String((*path_iter).second).c_str(),
                       (*path_iter).second));

            break;
          }
        } // end SWITCH
        last_direction = (*path_iter).second;
        current_corridor.insert(wall_position1);
        current_corridor.insert(wall_position2);
      } // end FOR
      corridors_bounds.push_back(current_corridor);
      paths.push_back(current_path);
    } // end FOR

  // step3: throw everything together
  for (zonelist_iter = corridors_bounds.begin();
       zonelist_iter != corridors_bounds.end();
       zonelist_iter++)
    for (RPG_Map_ZoneConstIterator_t zone_iter = (*zonelist_iter).begin();
         zone_iter != (*zonelist_iter).end();
         zone_iter++)
      level_out.walls.insert(*zone_iter);

  // step4
  // - clear any rubble that may now block some corridors
  // - compute unmapped areas
  corridors = corridors_bounds;
  RPG_Map_ZoneListIterator_t corridors_iter;
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

//   // debug info
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
  for (unsigned long y = 0;
       y < dimensionY_in;
       y++)
    for (unsigned long x = 0;
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
      for (zonelist_iter = corridors.begin();
           zonelist_iter != corridors.end();
           zonelist_iter++)
        if ((*zonelist_iter).find(current_position) != (*zonelist_iter).end())
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
RPG_Map_Common_Tools::dump(const RPG_Map_Zone_t& zone_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::dump"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("zone (%u position(s)):\n"),
             zone_in.size()));
  unsigned long index = 0;
  for (RPG_Map_ZoneConstIterator_t zone_iterator = zone_in.begin();
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

const unsigned long
RPG_Map_Common_Tools::dist2Positions(const RPG_Map_Position_t& position1_in,
                                     const RPG_Map_Position_t& position2_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::dist2Positions"));

  return (::abs(position1_in.first - position2_in.first) +
          ::abs(position1_in.second - position2_in.second));
}

const std::string
RPG_Map_Common_Tools::direction2String(const RPG_Map_Direction& direction_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::direction2String"));

  switch (direction_in)
  {
    case UP:
      return std::string("UP");
    case RIGHT:
      return std::string("RIGHT");
    case DOWN:
      return std::string("DOWN");
    case LEFT:
      return std::string("LEFT");
    case INVALID:
      return std::string("INVALID");
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid direction (was %u), continuing\n"),
                 direction_in));

      ACE_ASSERT(false);

      break;
    }
  } // end SWITCH

  return std::string("INVALID");
}

const bool
RPG_Map_Common_Tools::isFloor(const RPG_Map_Position_t& position_in,
                              const RPG_Map_FloorPlan_t& levelMap_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::isFloor"));

  return ((levelMap_in.doors.find(position_in) == levelMap_in.doors.end()) &&
          (levelMap_in.walls.find(position_in) == levelMap_in.walls.end()) &&
          (levelMap_in.unmapped.find(position_in) == levelMap_in.unmapped.end()));
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

const bool
RPG_Map_Common_Tools::intersect(const RPG_Map_Zone_t& map_in,
                                const RPG_Map_Position_t& position_in,
                                const ORIGIN& origin_in,
                                RPG_Map_Directions_t& directions_out,
                                RPG_Map_Direction& next_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::intersect"));

  // init return value(s)
  directions_out.clear();
  next_out = INVALID;

  RPG_Map_Position_t up, right, down, left;
  up = position_in; up.second--;
  right = position_in; right.first++;
  down = position_in; down.second++;
  left = position_in; left.first--;

  // find possible destination(s)
  if (map_in.find(up) != map_in.end())
    directions_out.insert(UP);
  if (map_in.find(right) != map_in.end())
    directions_out.insert(RIGHT);
  if (map_in.find(down) != map_in.end())
    directions_out.insert(DOWN);
  if (map_in.find(left) != map_in.end())
    directions_out.insert(LEFT);

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
    {
      return false;
    }
    default:
      break;
  } // end IF

  return true;
}

void
RPG_Map_Common_Tools::crawlToPosition(const RPG_Map_Zone_t& map_in,
                                      const RPG_Map_Position_t& origin_in,
                                      const RPG_Map_Position_t& target_in,
                                      const ORIGIN& startOrigin_in,
                                      RPG_Map_PositionList_t& trail_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::crawlToPosition"));

  // init return value(s)
  trail_out.clear();

  ORIGIN origin = startOrigin_in;
  RPG_Map_Direction next = INVALID;
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
        case UP:
        {
          // proceed right, down or left, in this order
          if (directions.find(RIGHT) != directions.end())
            next = RIGHT;
          else if (directions.find(DOWN) != directions.end())
            next = DOWN;
          else
            next = LEFT;

          break;
        }
        case RIGHT:
        {
          // proceed down, left or up, in this order
          if (directions.find(DOWN) != directions.end())
            next = DOWN;
          else if (directions.find(LEFT) != directions.end())
            next = LEFT;
          else
            next = UP;

          break;
        }
        case DOWN:
        {
          // proceed left, up or right, in this order
          if (directions.find(LEFT) != directions.end())
            next = LEFT;
          else if (directions.find(UP) != directions.end())
            next = UP;
          else
            next = RIGHT;

          break;
        }
        case LEFT:
        {
          // proceed up, right or down, in this order
          if (directions.find(UP) != directions.end())
            next = UP;
          else if (directions.find(RIGHT) != directions.end())
            next = RIGHT;
          else
            next = DOWN;

          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid origin (was %u), continuing\n"),
                     origin));

          // *TODO*: what else can we do ?
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
      case UP:
      {
        current.second--; // go up
        origin = DOWN;
        break;
      }
      case RIGHT:
      {
        current.first++; // go right
        origin = LEFT;
        break;
      }
      case DOWN:
      {
        current.second++; // go down
        origin = UP;
        break;
      }
      case LEFT:
      {
        current.first--; // go left
        origin = RIGHT;
        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid direction (was %u), continuing\n"),
                   next));

        // *TODO*: what else can we do ?
        ACE_ASSERT(false);

        break;
      }
    } // end SWITCH
  } while (true);
}

void
RPG_Map_Common_Tools::crop(RPG_Map_Zone_t& room_inout)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::crop"));

  // sanity check
  if (room_inout.empty())
    return; // nothing to do...

  // two-pass algorithm:
  // 1. iterate over rows
  // 2. iterate over columns
  // and remove any sequences of less than 3 consecutive cells
  RPG_Map_ZoneConstIterator_t begin_sequence = room_inout.begin();
  RPG_Map_ZoneConstIterator_t line_iterator;
  RPG_Map_ZoneConstIterator_t zone_iterator = room_inout.begin();
  unsigned long next_x = 0;
  unsigned long count = 0;

  // step1
  while (true)
  {
    if ((zone_iterator != room_inout.end()) &&
        ((*zone_iterator).second == (*begin_sequence).second))
    {
      // skip to next row/end
      zone_iterator++;
      continue;
    } // end IF

    // iterate over row
    next_x = (*begin_sequence).first;
    count = 0;
    line_iterator = begin_sequence;
    while (line_iterator != zone_iterator) // maybe end
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
        // *NOTE*: std::set::erase removes [first, last) !
//         end_sequence = line_iterator;
//         end_sequence--;
//         room_inout.erase(begin_sequence, end_sequence);
        room_inout.erase(begin_sequence, line_iterator);
      } // end IF
      begin_sequence = line_iterator;
      next_x = (*begin_sequence).first;
      count = 0;
    } // end WHILE

    // finished ?
    if (room_inout.empty() ||
        (zone_iterator == room_inout.end()))
      break;
  } // end WHILE

  // sanity check
  if (room_inout.empty())
    return; // nothing more to do...

  // step2
  // create alternate sorting
  RPG_Map_AltPositions_t alt_room;
  for (zone_iterator = room_inout.begin();
       zone_iterator != room_inout.end();
       zone_iterator++)
    alt_room.insert(*zone_iterator);

  begin_sequence = alt_room.begin();
  unsigned long next_y = 0;
  count = 0;
  zone_iterator = alt_room.begin();
  while (true)
  {
    if ((zone_iterator != alt_room.end()) &&
        ((*zone_iterator).first == (*begin_sequence).first))
    {
      // skip to next column/end
      zone_iterator++;
      continue;
    } // end IF

    // iterate over column
    next_y = (*begin_sequence).second;
    count = 0;
    line_iterator = begin_sequence;
    while (line_iterator != zone_iterator) // maybe end
    {
      while ((line_iterator != room_inout.end()) &&
            ((*line_iterator).second == next_y))
      {
        next_y++;
        count++;
        line_iterator++;
      } // end IF

      // there is a gap (maybe next column)
      // --> remove any previous sequence of less than 3 consecutive cells
      if (count < 3)
      {
        // *NOTE*: std::set::erase removes [first, last) !
//         end_sequence = line_iterator;
//         end_sequence--;
//         alt_room.erase(begin_sequence, end_sequence);
        alt_room.erase(begin_sequence, line_iterator);
      } // end IF
      begin_sequence = line_iterator;
      next_y = (*begin_sequence).second;
      count = 0;
    } // end WHILE

    // finished ?
    if (alt_room.empty() ||
        (zone_iterator == alt_room.end()))
      break;
  } // end WHILE

  // return original sorting
  room_inout.clear();
  for (zone_iterator = alt_room.begin();
       zone_iterator != alt_room.end();
       zone_iterator++)
    room_inout.insert(*zone_iterator);
}

// void
// RPG_Map_Common_Tools::cropSquareBoundary(RPG_Map_Zone_t& room_inout)
// {
//   ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::cropSquareBoundary"));
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
//   RPG_Map_ZoneConstIterator_t begin_sequence = room_inout.begin();
//   RPG_Map_ZoneConstIterator_t line_iterator;
//   RPG_Map_ZoneConstIterator_t zone_iterator = room_inout.begin();
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
// //       RPG_Map_ZoneConstIterator_t last = room_inout.end(); last--;
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
// //       RPG_Map_ZoneConstIterator_t last = room_inout.end(); last--;
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

const bool
RPG_Map_Common_Tools::turn(const RPG_Map_Zone_t& map_in,
                           const RPG_Map_Position_t& position_in,
                           const ORIGIN& origin_in,
                           const bool& clockwise_in,
                           bool& isCorner_out,
                           RPG_Map_Direction& next_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::turn"));

  // init return value(s)
  isCorner_out = false;
  // *NOTE*: only change next_out if appropriate (i.e. when turning)
//   next_out = INVALID;

  RPG_Map_Position_t up, right, down, left;
  up = position_in; up.second--;
  right = position_in; right.first++;
  down = position_in; down.second++;
  left = position_in; left.first--;

  // find possible destination(s)
  RPG_Map_Directions_t directions;
  if (map_in.find(up) != map_in.end())
    directions.insert(UP);
  if (map_in.find(right) != map_in.end())
    directions.insert(RIGHT);
  if (map_in.find(down) != map_in.end())
    directions.insert(DOWN);
  if (map_in.find(left) != map_in.end())
    directions.insert(LEFT);

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
    case UP:
    {
      if ((directions.find(DOWN) == directions.end()) || // corner
          (directions.find((clockwise_in ? RIGHT : LEFT)) != directions.end())) // intersection
      {
        // determine next direction
        if (directions.find((clockwise_in ? RIGHT : LEFT)) != directions.end())
          next_out = (clockwise_in ? RIGHT : LEFT); // intersection, turn left/right
        else
        {
          next_out = (clockwise_in ? LEFT : RIGHT); // corner, turn right/left
          isCorner_out = true;
        } // end ELSE

        return true;
      } // end IF

      // --> can continue (and cannot turn left/right)
      break;
    }
    case RIGHT:
    {
      if ((directions.find(LEFT) == directions.end()) || // corner
          (directions.find((clockwise_in ? DOWN : UP)) != directions.end())) // intersection
      {
        // determine next direction
        if (directions.find((clockwise_in ? DOWN : UP)) != directions.end())
          next_out = (clockwise_in ? DOWN : UP); // intersection, turn left/right
        else
        {
          next_out = (clockwise_in ? UP : DOWN); // corner, turn right/left
          isCorner_out = true;
        } // end ELSE

        return true;
      } // end IF

      // --> can continue (and cannot turn left/right)
      break;
    }
    case DOWN:
    {
      if ((directions.find(UP) == directions.end()) || // corner
          (directions.find((clockwise_in ? LEFT : RIGHT)) != directions.end())) // intersection
      {
        // determine next direction
        if (directions.find((clockwise_in ? LEFT : RIGHT)) != directions.end())
          next_out = (clockwise_in ? LEFT : RIGHT); // intersection, turn left/right
        else
        {
          next_out = (clockwise_in ? RIGHT : LEFT); // corner, turn right/left
          isCorner_out = true;
        } // end ELSE

        return true;
      } // end IF

      // --> can continue (and cannot turn left/right)
      break;
    }
    case LEFT:
    {
      if ((directions.find(RIGHT) == directions.end()) || // corner
          (directions.find((clockwise_in ? UP : DOWN)) != directions.end())) // intersection
      {
        // determine next direction
        if (directions.find((clockwise_in ? UP : DOWN)) != directions.end())
          next_out = (clockwise_in ? UP : DOWN); // intersection, turn left/right
        else
        {
          next_out = (clockwise_in ? DOWN : UP); // corner, turn right/left
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
                 ACE_TEXT("invalid origin (was %u), continuing\n"),
                 origin_in));

      // *TODO*: what else can we do ?
      ACE_ASSERT(false);

      break;
    }
  } // end IF

  return false;
}

void
RPG_Map_Common_Tools::findDoorPositions(const RPG_Map_Zone_t& room_in,
                                        const bool& doorFillsPosition_in,
                                        RPG_Map_PositionList_t& doorPositions_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::findDoorPositions"));

  // init return value(s)
  doorPositions_out.clear();

  // sanity check(s)
  if (room_in.empty())
    return; // nothing to do

  // *NOTE*: constraints depend on whether doors fill a whole position [] or just
  // "adorn" the side of a wall (implements the notion that walls are
  // infinitely thin - which is essentially a design *DECISION*)
  // - corners/intersections are unsuitable positions
  // - enforce a minimal distance [i.e. to allow corridors_bounds] between any 2 doors
  RPG_Map_Position_t current = *(room_in.begin());
  RPG_Map_Direction next = RIGHT;
  ORIGIN origin = DOWN;
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
        case UP:
        {
          current.second--; // go up
          origin = DOWN;
          break;
        }
        case RIGHT:
        {
          current.first++; // go right
          origin = LEFT;
          break;
        }
        case DOWN:
        {
          current.second++; // go down
          origin = UP;
          break;
        }
        case LEFT:
        {
          current.first--; // go left
          origin = RIGHT;
          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid direction (was %u), continuing\n"),
                     next));

          // *TODO*: what else can we do ?
          ACE_ASSERT(false);

          break;
        }
      } // end SWITCH
    } while (current != *(room_in.begin()));

    // step2: counter-clockwise
    RPG_Map_PositionList_t alt_doorPositions;
    origin = RIGHT;
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
        case UP:
        {
          current.second--; // go up
          origin = DOWN;
          break;
        }
        case RIGHT:
        {
          current.first++; // go right
          origin = LEFT;
          break;
        }
        case DOWN:
        {
          current.second++; // go down
          origin = UP;
          break;
        }
        case LEFT:
        {
          current.first--; // go left
          origin = RIGHT;
          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid direction (was %u), continuing\n"),
                     next));

          // *TODO*: what else can we do ?
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
          case UP:
          {
            current.second--; // go up
            origin = DOWN;
            break;
          }
          case RIGHT:
          {
            current.first++; // go right
            origin = LEFT;
            break;
          }
          case DOWN:
          {
            current.second++; // go down
            origin = UP;
            break;
          }
          case LEFT:
          {
            current.first--; // go left
            origin = RIGHT;
            break;
          }
          default:
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("invalid direction (was %u), continuing\n"),
                       next));

            // *TODO*: what else can we do ?
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
    unsigned long count = 0;
    for (RPG_Map_PositionListIterator_t list_iterator = doorPositions_out.begin();
         list_iterator != doorPositions_out.end();
         list_iterator++, count++)
    {
      if (count % (RPG_MAP_DOOR_SEPARATION + 1))
        doorPositions_out.erase(list_iterator);
    } // end FOR
  } // end ELSE
}

const RPG_Map_Direction
RPG_Map_Common_Tools::door2exitDirection(const RPG_Map_Zone_t& room_in,
                                         const RPG_Map_Position_t& door_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::door2exitDirection"));

  // step1: compute 4 neighbours
  RPG_Map_Position_t up, right, down, left;
  up = door_in; up.second -= 1;
  right = door_in; right.first += 1;
  down = door_in; down.second += 1;
  left = door_in; left.first -= 1;

  // step2: two of the neighbours will be walls, the third will be inside
  // --> the one that is NOT part of the room indicates the exit direction
  if (room_in.find(up) == room_in.end())
    return UP;
  else if (room_in.find(right) == room_in.end())
    return RIGHT;
  else if (room_in.find(down) == room_in.end())
    return DOWN;

  ACE_ASSERT(room_in.find(left) == room_in.end());
  return LEFT;
}

void
RPG_Map_Common_Tools::displayCorridors(const unsigned long& dimensionX_in,
                                       const unsigned long& dimensionY_in,
                                       const RPG_Map_ZoneList_t& rooms_in,
                                       const RPG_Map_ZoneList_t& doors_in,
                                       const RPG_Map_ZoneList_t& corridors_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Common_Tools::displayCorridors"));

  RPG_Map_Position_t current_position;
  unsigned long index = 0;
  RPG_Map_ZoneListConstIterator_t iterator;
  bool done = false;
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

      done = false;
      for (iterator = doors_in.begin();
           iterator != doors_in.end();
           iterator++)
      {
        if ((*iterator).find(current_position) != (*iterator).end())
        {
          std::cout << ACE_TEXT("=");

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
          std::cout << ACE_TEXT("#");

          done = true;

          break;
        } // end IF
      } // end FOR
      if (done)
        continue;

      index = 0;
      for (iterator = corridors_in.begin();
           iterator != corridors_in.end();
           iterator++, index++)
      {
        if ((*iterator).find(current_position) != (*iterator).end())
        {
          converter.str(ACE_TEXT_ALWAYS_CHAR(""));
          converter.clear();
          converter << index;
          std::cout << converter.str();

          done = true;

          break;
        } // end IF
      } // end FOR
      if (done)
        continue;

      std::cout << ACE_TEXT(".");
    } // end FOR
    std::cout << std::endl;
  } // end FOR
}
