#!/bin/sh
#//%%%FILE%%%////////////////////////////////////////////////////////////////////
#// File Name: bindings.sh
#//
#// History:
#//   Date   |Name | Description of modification
#// ---------|-----|-------------------------------------------------------------
#// 20/02/06 | soh | Creation.
#//%%%FILE%%%////////////////////////////////////////////////////////////////////

# XML Parser
# generate schema include file
/usr/bin/xsdcxx cxx-parser --type-map ../rpg_character.map --char-type char --output-dir ./.. --xml-parser xerces --generate-validation --generate-noop-impl --force-overwrite --generate-xml-schema --skel-file-suffix _schema_dictionary_types --hxx-suffix .h --show-anonymous --show-sloc ../rpg_character.xsd

# generate implementation
/usr/bin/xsdcxx cxx-parser --type-map ../rpg_character.map --char-type char --output-dir ./.. --xml-parser xerces --generate-validation --generate-noop-impl --force-overwrite --extern-xml-schema rpg_character_schema.h --skel-file-suffix _dictionary_types --impl-file-suffix _dictionary_parser_base --hxx-suffix .h --cxx-suffix .cpp --show-anonymous --show-sloc ../rpg_character.xsd

# # XML Tree
# # generate schema files
# /usr/bin/xsdcxx cxx-tree --char-type char --output-dir ./.. --namespace-map http://www.w3.org/2001/XMLSchema= --generate-serialization --generate-ostream --generate-doxygen --generate-comparison --generate-forward --suppress-parsing --type-naming knr --function-naming knr --root-element-last --hxx-suffix .h --cxx-suffix .cpp --fwd-suffix -fwd.h --show-anonymous --show-sloc ../rpg_character.xsd
