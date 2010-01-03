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
/usr/local/bin/XML2CppCode -e -f ./../dice/rpg_dice.xsd -i -o ./../dice -p ./../templates/h -s -u

# generate "XMLSchema" namespace include file
/usr/bin/xsdcxx cxx-parser --char-type char --output-dir ./../dice --xml-parser xerces --force-overwrite --generate-xml-schema --skel-file-suffix "" --hxx-suffix .h --show-anonymous --show-sloc ../rpg_XMLSchema_XML_types.xsd

# generate include/implementation
xsdcxx cxx-parser --type-map ./../dice/rpg_dice.map --char-type char --output-dir ./../dice --namespace-map urn:rpg= --include-with-brackets --xml-parser xerces --generate-noop-impl --force-overwrite --extern-xml-schema rpg_XMLSchema.h --skel-file-suffix _XML_types --impl-file-suffix _XML_parser_base --hxx-suffix .h --cxx-suffix .cpp --show-anonymous --show-sloc ./../dice/rpg_dice.xsd
