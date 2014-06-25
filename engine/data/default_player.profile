<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<player_t xmlns="urn:rpg" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="urn:rpg rpg_player.xsd">

  <name>aicJvgv</name>

  <abilities>
    <ability>ABILITY_NATURAL_WEAPONS</ability>
    <ability>ABILITY_SPELLS</ability>
    <ability>ABILITY_ANIMAL_COMPANION</ability>
  </abilities>

  <alignment>
    <civic>ALIGNMENTCIVIC_NEUTRAL</civic>
    <ethic>ALIGNMENTETHIC_GOOD</ethic>
  </alignment>

  <attributes>
    <strength>13</strength>
    <dexterity>16</dexterity>
    <constitution>12</constitution>
    <intelligence>11</intelligence>
    <wisdom>17</wisdom>
    <charisma>14</charisma>
  </attributes>

  <defaultSize>SIZE_MEDIUM</defaultSize>

  <feats>
    <feat>FEAT_PROFICIENCY_ARMOR_LIGHT</feat>
    <feat>FEAT_PROFICIENCY_ARMOR_MEDIUM</feat>
    <feat>FEAT_PROFICIENCY_SHIELD</feat>
    <feat>FEAT_PROFICIENCY_SIMPLE_WEAPONS</feat>
    <feat>FEAT_MULTI_ATTACK</feat>
  </feats>

  <knownSpells/>

  <maxHP>8</maxHP>

  <skills>
    <skill>
      <skill>SKILL_BLUFF</skill>
      <rank>3</rank>
    </skill>
    <skill>
      <skill>SKILL_CRAFT_ALCHEMY</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_CRAFT_SMITH_ARMOR</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_DECIPHER_SCRIPT</skill>
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
      <skill>SKILL_INTIMIDATE</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_KNOWLEDGE_ALL</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_KNOWLEDGE_ARCANA</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_KNOWLEDGE_DUNGEONS</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_SPELLCRAFT</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_SPOT</skill>
      <rank>1</rank>
    </skill>
    <skill>
      <skill>SKILL_TUMBLE</skill>
      <rank>1</rank>
    </skill>
  </skills>

  <classXML>
    <metaClass>METACLASS_PRIEST</metaClass>
    <subClass>SUBCLASS_DRUID</subClass>
  </classXML>

  <gender>GENDER_FEMALE</gender>

  <offhand>OFFHAND_LEFT</offhand>

  <race>RACE_DWARF</race>

  <race>RACE_HALFLING</race>

  <gold>0</gold>

  <inventory>
    <item>
      <type>ITEM_WEAPON</type>
      <weapon isDoubleWeapon="false" isNonLethal="false" isReachWeapon="false">
        <baseWeight>2</baseWeight>
        <baseStorePrice>
          <numGoldPieces>6</numGoldPieces>
        </baseStorePrice>
        <type>LIGHT_MELEE_WEAPON_SICKLE</type>
        <category>WEAPONCATEGORY_SIMPLE</category>
        <weaponClass>WEAPONCLASS_LIGHT_MELEE</weaponClass>
        <baseDamage>
          <numDice>1</numDice>
          <typeDice>D_6</typeDice>
        </baseDamage>
        <criticalHit>
          <minToHitRoll>20</minToHitRoll>
          <damageModifier>2</damageModifier>
        </criticalHit>
        <typeOfDamage>PHYSICALDAMAGE_SLASHING</typeOfDamage>
      </weapon>
    </item>
    <item>
      <type>ITEM_ARMOR</type>
      <armor>
        <baseWeight>25</baseWeight>
        <baseStorePrice>
          <numGoldPieces>15</numGoldPieces>
        </baseStorePrice>
        <type>ARMOR_HIDE</type>
        <category>ARMORCATEGORY_MEDIUM</category>
        <baseBonus>3</baseBonus>
        <maxDexterityBonus>4</maxDexterityBonus>
        <checkPenalty>-3</checkPenalty>
        <arcaneSpellFailure>20</arcaneSpellFailure>
        <baseSpeed>20</baseSpeed>
      </armor>
    </item>
    <item>
      <type>ITEM_ARMOR</type>
      <armor>
        <baseWeight>5</baseWeight>
        <baseStorePrice>
          <numGoldPieces>3</numGoldPieces>
        </baseStorePrice>
        <type>ARMOR_SHIELD_LIGHT_WOODEN</type>
        <category>ARMORCATEGORY_SHIELD</category>
        <baseBonus>1</baseBonus>
        <maxDexterityBonus>0</maxDexterityBonus>
        <checkPenalty>-1</checkPenalty>
        <arcaneSpellFailure>5</arcaneSpellFailure>
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
