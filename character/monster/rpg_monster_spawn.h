
// -------------------------------- * * * ----------------------------------- //
// PLEASE NOTE: this file was/is generated by XML2CppCode 0.1
// -------------------------------- * * * ----------------------------------- //

#pragma once
#ifndef RPG_MONSTER_SPAWN_H
#define RPG_MONSTER_SPAWN_H

#include "rpg_monster_exports.h"

struct RPG_Monster_Export RPG_Monster_Spawn
{
  std::string type;
  RPG_Common_FixedPeriod interval;
  float probability;
  unsigned int max_num_spawned;
  float amble_probability;
};

#endif