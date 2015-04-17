<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<player_t xmlns="urn:rpg" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="urn:rpg rpg_player.xsd">

  <name>pOvWSw</name>

  <abilities>
    <ability>ABILITY_SPELLS</ability>
  </abilities>

  <alignment>
    <civic>ALIGNMENTCIVIC_NEUTRAL</civic>
    <ethic>ALIGNMENTETHIC_NEUTRAL</ethic>
  </alignment>

  <attributes>
    <strength>13</strength>
    <dexterity>12</dexterity>
    <constitution>14</constitution>
    <intelligence>12</intelligence>
    <wisdom>14</wisdom>
    <charisma>17</charisma>
  </attributes>

  <defaultSize>SIZE_MEDIUM</defaultSize>

  <feats>
    <feat>FEAT_COMBAT_REFLEXES</feat>
    <feat>FEAT_PROFICIENCY_ARMOR_LIGHT</feat>
    <feat>FEAT_PROFICIENCY_ARMOR_MEDIUM</feat>
    <feat>FEAT_PROFICIENCY_ARMOR_HEAVY</feat>
    <feat>FEAT_PROFICIENCY_MARTIAL_WEAPONS</feat>
    <feat>FEAT_PROFICIENCY_SHIELD</feat>
    <feat>FEAT_PROFICIENCY_SIMPLE_WEAPONS</feat>
  </feats>

  <knownSpells/>

  <maxHP>10</maxHP>

  <skills>
    <skill>
      <skill>SKILL_APPRAISE</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_CRAFT_SMITH_BLACK</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_DIPLOMACY</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_DISGUISE</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_ESCAPE_ARTIST</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_HEAL</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_KNOWLEDGE_LOCAL</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_KNOWLEDGE_RELIGION</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_KNOWLEDGE_PLANES</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_SURVIVAL</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_SWIM</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_USE_ROPE</skill>
      <rank>1</rank>
    </skill>
  </skills>

  <classXML>
    <metaClass>METACLASS_WARRIOR</metaClass>
    <subClass>SUBCLASS_PALADIN</subClass>
  </classXML>

  <gender>GENDER_MALE</gender>

  <offhand>OFFHAND_LEFT</offhand>

  <race>RACE_ORC</race>

  <gold>0</gold>

  <inventory>
    <item>
      <type>ITEM_WEAPON</type>
      <weapon isDoubleWeapon="false" isNonLethal="false" isReachWeapon="false">
        <baseWeight>4</baseWeight>
        <baseStorePrice>
          <numGoldPieces>15</numGoldPieces>
        </baseStorePrice>
        <type>ONE_HANDED_MELEE_WEAPON_SWORD_LONG</type>
        <category>WEAPONCATEGORY_MARTIAL</category>
        <weaponClass>WEAPONCLASS_ONE_HANDED_MELEE</weaponClass>
        <baseDamage>
          <numDice>1</numDice>
          <typeDice>D_8</typeDice>
        </baseDamage>
        <criticalHit>
          <minToHitRoll>19</minToHitRoll>
          <damageModifier>2</damageModifier>
        </criticalHit>
        <typeOfDamage>PHYSICALDAMAGE_SLASHING</typeOfDamage>
      </weapon>
    </item>
    <item>
      <type>ITEM_ARMOR</type>
      <armor>
        <baseWeight>50</baseWeight>
        <baseStorePrice>
          <numGoldPieces>1500</numGoldPieces>
        </baseStorePrice>
        <type>ARMOR_PLATE_FULL</type>
        <category>ARMORCATEGORY_HEAVY</category>
        <baseBonus>8</baseBonus>
        <maxDexterityBonus>1</maxDexterityBonus>
        <checkPenalty>-6</checkPenalty>
        <arcaneSpellFailure>35</arcaneSpellFailure>
        <baseSpeed>20</baseSpeed>
      </armor>
    </item>
    <item>
      <type>ITEM_ARMOR</type>
      <armor>
        <baseWeight>15</baseWeight>
        <baseStorePrice>
          <numGoldPieces>20</numGoldPieces>
        </baseStorePrice>
        <type>ARMOR_SHIELD_HEAVY_STEEL</type>
        <category>ARMORCATEGORY_SHIELD</category>
        <baseBonus>2</baseBonus>
        <maxDexterityBonus>0</maxDexterityBonus>
        <checkPenalty>-2</checkPenalty>
        <arcaneSpellFailure>15</arcaneSpellFailure>
        <baseSpeed>0</baseSpeed>
      </armor>
    </item>
    <item>
      <type>ITEM_COMMODITY</type>
      <commodity>
        <baseWeight>1</baseWeight>
        <baseStorePrice>
          <numSilverPieces>1</numSilverPieces>
        </baseStorePrice>
        <type>COMMODITY_LIGHT</type>
        <subType>COMMODITY_LIGHT_TORCH</subType>
      </commodity>
    </item>
  </inventory>

  <XP>0</XP>

</player_t>
