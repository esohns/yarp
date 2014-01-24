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
perl ${PERL_SCRIPT} -n RPG_Item > ./item/rpg_item_exports.h
[ $? -ne 0 ] && echo "ERROR: failed to perl, aborting" && exit 1

# C++ "glue code"
XML2CppCode -e -f ./item/rpg_item.xsd -i -o ./item -s -u
[ $? -ne 0 ] && echo "ERROR: failed to XML2CppCode, aborting" && exit 1

# generate "XMLSchema" namespace include file (rpg_item.xsd)
xsdcxx cxx-parser --char-type char --output-dir ./item --xml-parser xerces --force-overwrite --generate-xml-schema --skel-file-suffix "" --hxx-suffix .h --show-anonymous --show-sloc ./item/rpg_XMLSchema_XML_types.xsd
[ $? -ne 0 ] && echo "ERROR: failed to xsdcxx, aborting" && exit 1

# generate parser include/implementation (rpg_item.xsd)
xsdcxx cxx-parser --type-map ./item/rpg_item.map --char-type char --output-dir ./item --namespace-map urn:rpg= --xml-parser xerces --force-overwrite --extern-xml-schema rpg_XMLSchema.h --skel-file-suffix _XML_types --hxx-suffix .h --cxx-suffix .cpp --show-anonymous --show-sloc --export-symbol "RPG_Item_Export" --hxx-prologue "#include \"rpg_item_exports.h\"" ./item/rpg_item.xsd
[ $? -ne 0 ] && echo "ERROR: failed to xsdcxx, aborting" && exit 1
# *NOTE*: xsdcxx improperly rearranges the included headers from the map file
# --> move a repaired version back into the project directory
# *IMPORTANT NOTE*: needs to be updated after every change
cp -f ./item/scripts/rpg_item_XML_types.h ./item
[ $? -ne 0 ] && echo "ERROR: failed to cp, aborting" && exit 1

## generate "XMLSchema" namespace include file (tree)
#xsdcxx cxx-tree --char-type char --output-dir ./item --generate-serialization --generate-xml-schema --hxx-suffix .h --#show-anonymous --show-sloc ./item/rpg_XMLSchema_XML_tree.xsd
#xsdcxx cxx-tree --char-type char --output-dir ./item --generate-serialization --generate-insertion ACE_OutputCDR --generate-extraction ACE_InputCDR --generate-xml-schema --hxx-suffix .h --show-anonymous --show-sloc ./item/rpg_XMLSchema_XML_tree.xsd
#[ $? -ne 0 ] && echo "ERROR: failed to xsdcxx, aborting" && exit 1

# generate tree include/implementation (rpg_item.xsd)
xsdcxx cxx-tree --generate-serialization --generate-ostream --generate-comparison --type-regex '/(.+) RPG_(.+)_Type/RPG_\u$2_XMLTree_Type/' --root-element-none --char-type char --output-dir ./item --namespace-map urn:rpg= --extern-xml-schema rpg_XMLSchema.h --hxx-suffix _XML_tree.h --cxx-suffix _XML_tree.cpp --show-anonymous --show-sloc --export-symbol "RPG_Item_Export" --hxx-prologue "#include \"rpg_item_exports.h\"" ./item/rpg_item.xsd
#xsdcxx cxx-tree --generate-serialization --generate-ostream --generate-comparison --generate-insertion ACE_OutputCDR --generate-extraction ACE_InputCDR --type-regex '/(.+) RPG_(.+)_Type/RPG_\u$2_XMLTree_Type/' --root-element-none --char-type char --output-dir ./item --namespace-map urn:rpg= --extern-xml-schema rpg_XMLSchema.h --hxx-suffix _XML_tree.h --cxx-suffix _XML_tree.cpp --show-anonymous --show-sloc --export-symbol "RPG_Item_Export" --hxx-prologue "#include \"rpg_item_exports.h\"" ./item/rpg_item.xsd
if [ $? -ne 0 ]; then
 echo "ERROR: failed to xsdcxx, aborting"
 exit 1
fi
