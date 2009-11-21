#!/bin/sh
#//%%%FILE%%%////////////////////////////////////////////////////////////////////
#// File Name: bindings.sh
#//
#// History:
#//   Date   |Name | Description of modification
#// ---------|-----|-------------------------------------------------------------
#// 20/02/06 | soh | Creation.
#//%%%FILE%%%////////////////////////////////////////////////////////////////////

# generate "XMLSchema" namespace include file
/usr/bin/xsdcxx cxx-parser --char-type char --output-dir ./.. --xml-parser xerces --force-overwrite --generate-xml-schema --skel-file-suffix "" --hxx-suffix .h --show-anonymous --show-sloc ../rpg_XMLSchema.xsd

# generate include/implementation
xsdcxx cxx-parser --type-map ../rpg_item.map --char-type char --output-dir ./.. --namespace-map urn:rpg= --include-with-brackets --xml-parser xerces --generate-noop-impl --force-overwrite --extern-xml-schema rpg_XMLSchema.h --skel-file-suffix _XML_types --impl-file-suffix _XML_parser_base --hxx-suffix .h --cxx-suffix .cpp --show-anonymous --show-sloc ../rpg_item.xsd
