#!/bin/sh
#//%%%FILE%%%////////////////////////////////////////////////////////////////////
#// File Name: bindings.sh
#//
#// History:
#//   Date   |Name | Description of modification
#// ---------|-----|-------------------------------------------------------------
#// 20/02/06 | soh | Creation.
#//%%%FILE%%%////////////////////////////////////////////////////////////////////

# C++ "glue code"
/usr/local/bin/XML2CppCode -e -f ./../rpg_character.xsd -i -o ./.. -p ./../templates/h -s -u

# XML Parser/Tree
# generate "XMLSchema" namespace include file (rpg_character.xsd)
/usr/bin/xsdcxx cxx-parser --char-type char --output-dir ./.. --xml-parser xerces --force-overwrite --generate-xml-schema --skel-file-suffix "" --hxx-suffix .h --show-anonymous --show-sloc ../rpg_XMLSchema_XML_types.xsd

# generate parser include/implementation (rpg_character.xsd)
xsdcxx cxx-parser --type-map ../rpg_character.map --char-type char --output-dir ./.. --namespace-map urn:rpg= --xml-parser xerces --force-overwrite --extern-xml-schema rpg_XMLSchema.h --skel-file-suffix _XML_types --hxx-suffix .h --cxx-suffix .cpp --show-anonymous --show-sloc ../rpg_character.xsd

# generate "XMLSchema" namespace include file (tree)
/usr/bin/xsdcxx cxx-tree --char-type char --output-dir ./.. --generate-serialization --generate-insertion ACE_OutputCDR --generate-extraction ACE_InputCDR --generate-xml-schema --hxx-suffix .h --show-anonymous --show-sloc ../rpg_XMLSchema_XML_tree.xsd

# generate tree include/implementation (rpg_character.xsd / rpg_character_player.xsd)
xsdcxx cxx-tree --generate-serialization --generate-ostream --generate-comparison --generate-insertion ACE_OutputCDR --generate-extraction ACE_InputCDR --type-regex '/(.+) RPG_Common_(.+)_Type/RPG_Common_\u$2_XMLTree_Type/' --type-regex '/(.+) RPG_Character_(.+)_Type/RPG_Character_\u$2_XMLTree_Type/' --char-type char --output-dir ./.. --namespace-map urn:rpg= --extern-xml-schema rpg_XMLSchema.h --hxx-suffix _XML_tree.h --cxx-suffix _XML_tree.cpp --show-anonymous --show-sloc ../rpg_character.xsd

xsdcxx cxx-tree --generate-serialization --generate-ostream --generate-comparison --generate-insertion ACE_OutputCDR --generate-extraction ACE_InputCDR --type-regex '/(.+) RPG_Common_(.+)_Type/RPG_Common_\u$2_XMLTree_Type/' --type-regex '/(.+) RPG_Magic_(.+)_Type/RPG_Magic_\u$2_XMLTree_Type/' --type-regex '/(.+) RPG_Item_(.+)_Type/RPG_Item_\u$2_XMLTree_Type/' --type-regex '/(.+) RPG_Character_(.+)_Type/RPG_Character_\u$2_XMLTree_Type/' --char-type char --output-dir ./.. --namespace-map urn:rpg= --extern-xml-schema rpg_XMLSchema.h --hxx-suffix _XML_tree.h --cxx-suffix _XML_tree.cpp --show-anonymous --show-sloc ../rpg_character_player.xsd
