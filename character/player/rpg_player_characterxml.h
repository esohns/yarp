
// -------------------------------- * * * ----------------------------------- //
// PLEASE NOTE: this file was/is generated by XML2CppCode 0.0.1-devel
// -------------------------------- * * * ----------------------------------- //

#ifndef RPG_PLAYER_CHARACTERXML_H
#define RPG_PLAYER_CHARACTERXML_H

struct RPG_Player_CharacterXML
 : public RPG_Player_CharacterBaseXML
{
  RPG_Character_ClassXML classXML;
  RPG_Character_Gender gender;
  RPG_Character_OffHand offhand;
  std::vector<RPG_Character_Race> races;
};

#endif
