
// -------------------------------- * * * ----------------------------------- //
// PLEASE NOTE: this file was/is generated by XML2CppCode 0.0.1-devel
// -------------------------------- * * * ----------------------------------- //

#ifndef RPG_MAGIC_SPELL_DAMAGETYPEUNION_H
#define RPG_MAGIC_SPELL_DAMAGETYPEUNION_H

struct RPG_Magic_Spell_DamageTypeUnion
{
  union
  {
    RPG_Common_PhysicalDamageType physicaldamagetype;
    RPG_Magic_Descriptor descriptor;
  };

  enum Discriminator_t
  {
    PHYSICALDAMAGETYPE,
    DESCRIPTOR,
    INVALID
  };
  Discriminator_t discriminator;
};

#endif
