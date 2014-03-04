#!/bin/sh
#//%%%FILE%%%////////////////////////////////////////////////////////////////////
#// File Name: bindings.sh
#//
#// History:
#//   Date   |Name | Description of modification
#// ---------|-----|-------------------------------------------------------------
#// 20/02/06 | soh | Creation.
#//%%%FILE%%%////////////////////////////////////////////////////////////////////

# sanity check(s)
command -v perl >/dev/null 2>&1 || { echo "Perl is not installed. Aborting." >&2; exit 1; }
command -v XML2CppCode >/dev/null 2>&1 || { echo "XML2CppCode is not installed. Aborting." >&2; exit 1; }
command -v xsdcxx >/dev/null 2>&1 || { echo "xsdcxx is not installed. Aborting." >&2; exit 1; }

# generate exports file
#PERL_SCRIPT=/usr/lib/ace/bin/generate_export_file.pl
PERL_SCRIPT=/usr/local/src/ACE_wrappers/bin/generate_export_file.pl
[ ! -f ${PERL_SCRIPT} ] && echo "ERROR: invalid script file \"${PERL_SCRIPT}\" (not a file), aborting" && exit 1
perl ${PERL_SCRIPT} -n RPG_Monster > ./character/monster/rpg_monster_exports.h
[ $? -ne 0 ] && echo "ERROR: failed to perl, aborting" && exit 1

# C++ "glue code"
XML2CppCode -d RPG_Monster_Export -e -f ./character/monster/rpg_monster.xsd -i -o ./character/monster -s -u
[ $? -ne 0 ] && echo "ERROR: failed to XML2CppCode, aborting" && exit 1

# XML Parser
# generate "XMLSchema" namespace include file
xsdcxx cxx-parser --char-type char --output-dir ./character/monster --xml-parser xerces --force-overwrite --generate-xml-schema --skel-file-suffix "" --hxx-suffix .h --show-anonymous --show-sloc ./character/monster/rpg_XMLSchema_XML_types.xsd
[ $? -ne 0 ] && echo "ERROR: failed to xsdcxx, aborting" && exit 1

# generate include/implementation
xsdcxx cxx-parser --type-map ./character/monster/rpg_monster.map --char-type char --output-dir ./character/monster --namespace-map urn:rpg= --xml-parser xerces --force-overwrite --extern-xml-schema rpg_XMLSchema.h --skel-file-suffix _XML_types --hxx-suffix .h --cxx-suffix .cpp --show-anonymous --show-sloc ./character/monster/rpg_monster.xsd
[ $? -ne 0 ] && echo "ERROR: failed to xsdcxx, aborting" && exit 1
# *NOTE*: xsdcxx improperly rearranges the included headers from the map file
# --> move a repaired version back into the project directory
# *IMPORTANT NOTE*: needs to be updated after every change
cp -f ./character/monster/scripts/rpg_monster_XML_types.h ./character/monster
if [ $? -ne 0 ]; then
 echo "ERROR: failed to cp, aborting"
 exit 1
fi
