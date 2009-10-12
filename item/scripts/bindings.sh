#!/bin/sh
#//%%%FILE%%%////////////////////////////////////////////////////////////////////
#// File Name: bindings.sh
#//
#// History:
#//   Date   |Name | Description of modification
#// ---------|-----|-------------------------------------------------------------
#// 20/02/06 | soh | Creation.
#//%%%FILE%%%////////////////////////////////////////////////////////////////////

# generate schema include file
/usr/bin/xsdcxx cxx-parser --type-map ../rpg_item.map --char-type char --output-dir ./.. --xml-parser xerces --generate-validation --generate-noop-impl --force-overwrite --generate-xml-schema --skel-file-suffix _schema_dictionary_types --hxx-suffix .h --show-anonymous --show-sloc ../rpg_item.xsd

# generate implementation
/usr/bin/xsdcxx cxx-parser --type-map ../rpg_item.map --char-type char --output-dir ./.. --xml-parser xerces --generate-validation --generate-noop-impl --force-overwrite --extern-xml-schema rpg_item_schema.h --skel-file-suffix _dictionary_types --impl-file-suffix _dictionary_parser_base --hxx-suffix .h --cxx-suffix .cpp --show-anonymous --show-sloc ../rpg_item.xsd
