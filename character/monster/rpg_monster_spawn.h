
// -------------------------------- * * * ----------------------------------- //
// PLEASE NOTE: this file was/is generated by XML2CppCode 0.0.1-devel
// -------------------------------- * * * ----------------------------------- //

#ifndef RPG_MONSTER_SPAWN_H
#define RPG_MONSTER_SPAWN_H

struct RPG_Monster_Spawn
{
  std::string type;
  RPG_Common_FixedPeriod interval;
  float probability;
  ACE_UINT32 max_num_spawned;
  float amble_probability;
};

#endif
