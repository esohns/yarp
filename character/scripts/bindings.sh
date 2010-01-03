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

# XML Parser
# generate "XMLSchema" namespace include file
/usr/bin/xsdcxx cxx-parser --char-type char --output-dir ./.. --xml-parser xerces --force-overwrite --generate-xml-schema --skel-file-suffix "" --hxx-suffix .h --show-anonymous --show-sloc ../rpg_XMLSchema.xsd

# generate include/implementation
xsdcxx cxx-parser --type-map ../rpg_character.map --char-type char --output-dir ./.. --namespace-map urn:rpg= --xml-parser xerces --generate-noop-impl --force-overwrite --extern-xml-schema rpg_XMLSchema.h --skel-file-suffix _XML_types --impl-file-suffix _XML_parser_base --hxx-suffix .h --cxx-suffix .cpp --show-anonymous --show-sloc ../rpg_character.xsd

# # XML Tree
# # generate schema files
# /usr/bin/xsdcxx cxx-tree --char-type char --output-dir ./.. --namespace-map http://www.w3.org/2001/XMLSchema= --generate-serialization --generate-ostream --generate-doxygen --generate-comparison --generate-forward --suppress-parsing --type-naming knr --function-naming knr --root-element-last --hxx-suffix .h --cxx-suffix .cpp --fwd-suffix -fwd.h --show-anonymous --show-sloc ../rpg_character.xsd
