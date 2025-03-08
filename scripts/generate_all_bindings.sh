#!/bin/sh
# //%%%FILE%%%////////////////////////////////////////////////////////////////////
# // File Name: generate_all_bindings.sh
# //
# // History:
# //   Date   |Name | Description of modification
# // ---------|-----|-------------------------------------------------------------
# // 20/02/06 | soh | Creation.
# //%%%FILE%%%////////////////////////////////////////////////////////////////////

#DEFAULT_PROJECT_DIR="${HOME}/Projects/yarp"
DEFAULT_PROJECT_DIR=.
PROJECT_DIR=${DEFAULT_PROJECT_DIR}
## sanity check(s)
#[ ! -d ${PROJECT_DIR} ] && echo "ERROR: invalid project directory (was: \"${PROJECT_DIR}\"), aborting" && exit 1

## remember current dir...
#pushd . >/dev/null 2>&1

#cd ${PROJECT_DIR}
#[ $? -ne 0 ] && echo "ERROR: failed to switch to project directory (was: \"${PROJECT_DIR}\"), aborting" && exit 1

# ..........................................

echo "generating .so exports headers..."
${PROJECT_DIR}/chance/scripts/exports.sh
${PROJECT_DIR}/net/scripts/exports.sh
${PROJECT_DIR}/net/protocol/scripts/exports.sh
[ $? -ne 0 ] && echo "ERROR: failed generating .so exports headers, aborting" && exit 1
echo "generating .so exports headers...DONE"

# ..........................................

echo "generating XSD schema links..."
pushd . >/dev/null
cd ${PROJECT_DIR}/character/etc
rm -f rpg_common.xsd
ln -s ../../common/etc/rpg_common.xsd
rm -f rpg_dice.xsd
ln -s ../../chance/dice/etc/rpg_dice.xsd
popd >/dev/null

pushd . >/dev/null
cd ${PROJECT_DIR}/character/player/etc
rm -f rpg_character.xsd
ln -s ../../etc/rpg_character.xsd
rm -f rpg_common_environment.xsd
ln -s ../../../common/etc/rpg_common_environment.xsd
rm -f rpg_common.xsd
ln -s ../../../common/etc/rpg_common.xsd
rm -f rpg_dice.xsd
ln -s ../../../chance/dice/etc/rpg_dice.xsd
rm -f rpg_item.xsd
ln -s ../../../item/etc/rpg_item.xsd
rm -f rpg_magic.xsd
ln -s ../../../magic/etc/rpg_magic.xsd
popd >/dev/null

pushd . >/dev/null
cd ${PROJECT_DIR}/character/monster/etc
rm -f rpg_character.xsd
ln -s ../../etc/rpg_character.xsd
rm -f rpg_combat.xsd
ln -s ../../../combat/etc/rpg_combat.xsd
rm -f rpg_common_environment.xsd
ln -s ../../../common/etc/rpg_common_environment.xsd
rm -f rpg_common.xsd
ln -s ../../../common/etc/rpg_common.xsd
rm -f rpg_dice.xsd
ln -s ../../../chance/dice/etc/rpg_dice.xsd
rm -f rpg_item.xsd
ln -s ../../../item/etc/rpg_item.xsd
rm -f rpg_magic.xsd
ln -s ../../../magic/etc/rpg_magic.xsd
rm -f rpg_player.xsd
ln -s ../../player/etc/rpg_player.xsd
popd >/dev/null

pushd . >/dev/null
cd ${PROJECT_DIR}/combat/etc
rm -f rpg_character.xsd
ln -s ../../character/etc/rpg_character.xsd
rm -f rpg_common_environment.xsd
ln -s ../../common/etc/rpg_common_environment.xsd
rm -f rpg_common.xsd
ln -s ../../common/etc/rpg_common.xsd
rm -f rpg_dice.xsd
ln -s ../../chance/dice/etc/rpg_dice.xsd
rm -f rpg_magic.xsd
ln -s ../../magic/etc/rpg_magic.xsd
popd >/dev/null

pushd . >/dev/null
cd ${PROJECT_DIR}/common/etc
rm -f rpg_dice.xsd
ln -s ../../chance/dice/etc/rpg_dice.xsd
popd >/dev/null

pushd . >/dev/null
cd ${PROJECT_DIR}/engine/etc
rm -f rpg_character.xsd
ln -s ../../character/etc/rpg_character.xsd
rm -f rpg_combat.xsd
ln -s ../../combat/etc/rpg_combat.xsd
rm -f rpg_common_environment.xsd
ln -s ../../common/etc/rpg_common_environment.xsd
rm -f rpg_common.xsd
ln -s ../../common/etc/rpg_common.xsd
rm -f rpg_dice.xsd
ln -s ../../chance/dice/etc/rpg_dice.xsd
rm -f rpg_item.xsd
ln -s ../../item/etc/rpg_item.xsd
rm -f rpg_magic.xsd
ln -s ../../magic/etc/rpg_magic.xsd
rm -f rpg_map.xsd
ln -s ../../map/etc/rpg_map.xsd
rm -f rpg_monster.xsd
ln -s ../../character/monster/etc/rpg_monster.xsd
rm -f rpg_player.xsd
ln -s ../../character/player/etc/rpg_player.xsd
popd >/dev/null

pushd . >/dev/null
cd ${PROJECT_DIR}/item/etc
rm -f rpg_character.xsd
ln -s ../../character/etc/rpg_character.xsd
rm -f rpg_common_environment.xsd
ln -s ../../common/etc/rpg_common_environment.xsd
rm -f rpg_common.xsd
ln -s ../../common/etc/rpg_common.xsd
rm -f rpg_dice.xsd
ln -s ../../chance/dice/etc/rpg_dice.xsd
rm -f rpg_magic.xsd
ln -s ../../magic/etc/rpg_magic.xsd
popd >/dev/null

pushd . >/dev/null
cd ${PROJECT_DIR}/magic/etc
rm -f rpg_character.xsd
ln -s ../../character/etc/rpg_character.xsd
rm -f rpg_common_environment.xsd
ln -s ../../common/etc/rpg_common_environment.xsd
rm -f rpg_common.xsd
ln -s ../../common/etc/rpg_common.xsd
rm -f rpg_dice.xsd
ln -s ../../chance/dice/etc/rpg_dice.xsd
popd >/dev/null

pushd . >/dev/null
cd ${PROJECT_DIR}/map/etc
rm -f rpg_common_environment.xsd
ln -s ../../common/etc/rpg_common_environment.xsd
popd >/dev/null

pushd . >/dev/null
cd ${PROJECT_DIR}/client/etc
rm -f rpg_character.xsd
ln -s ../../character/etc/rpg_character.xsd
rm -f rpg_combat.xsd
ln -s ../../combat/etc/rpg_combat.xsd
rm -f rpg_common_environment.xsd
ln -s ../../common/etc/rpg_common_environment.xsd
rm -f rpg_common.xsd
ln -s ../../common/etc/rpg_common.xsd
rm -f rpg_dice.xsd
ln -s ../../chance/dice/etc/rpg_dice.xsd
rm -f rpg_engine.xsd
ln -s ../../engine/etc/rpg_engine.xsd
rm -f rpg_graphics.xsd
ln -s ../../graphics/etc/rpg_graphics.xsd
rm -f rpg_item.xsd
ln -s ../../item/etc/rpg_item.xsd
rm -f rpg_magic.xsd
ln -s ../../magic/etc/rpg_magic.xsd
rm -f rpg_map.xsd
ln -s ../../map/etc/rpg_map.xsd
rm -f rpg_monster.xsd
ln -s ../../character/monster/etc/rpg_monster.xsd
rm -f rpg_player.xsd
ln -s ../../character/player/etc/rpg_player.xsd
popd >/dev/null

echo "generating XML data bindings..."
${PROJECT_DIR}/chance/dice/scripts/bindings.sh
${PROJECT_DIR}/character/scripts/bindings.sh
${PROJECT_DIR}/character/player/scripts/bindings.sh
${PROJECT_DIR}/character/monster/scripts/bindings.sh
${PROJECT_DIR}/combat/scripts/bindings.sh
${PROJECT_DIR}/common/scripts/bindings.sh
${PROJECT_DIR}/engine/scripts/bindings.sh
${PROJECT_DIR}/graphics/scripts/bindings.sh
${PROJECT_DIR}/item/scripts/bindings.sh
${PROJECT_DIR}/magic/scripts/bindings.sh
${PROJECT_DIR}/map/scripts/bindings.sh
${PROJECT_DIR}/sound/scripts/bindings.sh
${PROJECT_DIR}/client/scripts/bindings.sh
[ $? -ne 0 ] && echo "ERROR: failed generating XML data bindings, aborting" && exit 1
echo "generating XML data bindings...DONE"

# ..........................................

echo "generating lex/yacc parsers..."
${PROJECT_DIR}/map/scripts/scanner.sh
${PROJECT_DIR}/map/scripts/parser.sh
${PROJECT_DIR}/net/protocol/scripts/scanner.sh
${PROJECT_DIR}/net/protocol/scripts/parser.sh
[ $? -ne 0 ] && echo "ERROR: failed generating lex/yacc parsers, aborting" && exit 1
echo "generating lex/yacc parsers...DONE"

#popd >/dev/null 2>&1
