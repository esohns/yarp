/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
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
#ifndef RPG_COMBAT_COMMON_H
#define RPG_COMBAT_COMMON_H

#include <rpg_character_base.h>

#include <ace/Log_Msg.h>

#include <set>

struct RPG_Combat_CombatantSequenceElement
{
  // needed for proper sorting...
  bool operator<(const RPG_Combat_CombatantSequenceElement& rhs_in) const
  {
    // *IMPORTANT NOTE*: we want to sort it DESCENDING, so < is actually > !
    if (initiative > rhs_in.initiative)
      return true;

    return (initiative < rhs_in.initiative ? false
                                           : (DEXModifier > rhs_in.DEXModifier));
  };
  bool operator==(const RPG_Combat_CombatantSequenceElement& rhs_in) const
  {
    if (initiative == rhs_in.initiative)
    {
      if (DEXModifier == rhs_in.DEXModifier)
      {
        // *IMPORTANT NOTE*: a conflict between monsters doesn't matter !
        // --> if either one is a PC, we have a conflict
        ACE_ASSERT(handle && rhs_in.handle);
        try
        {
          if (handle->isPlayerCharacter() ||
              rhs_in.handle->isPlayerCharacter())
            return true;
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in RPG_Combat_CombatantSequenceElement::operator==, aborting\n")));
        }
      } // end IF
    } // end IF

    return false;
  };
  bool operator!=(const RPG_Combat_CombatantSequenceElement& rhs_in) const
  {
    return !operator==(rhs_in);
  };

  char                      initiative;
  short int                 DEXModifier;
  const RPG_Character_Base* handle;
};

typedef std::set<RPG_Combat_CombatantSequenceElement> RPG_Combat_CombatantSequence_t;
typedef RPG_Combat_CombatantSequence_t::const_iterator RPG_Combat_CombatantSequenceIterator_t;

#endif
