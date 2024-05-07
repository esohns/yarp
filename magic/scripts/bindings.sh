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
XML2CppCodeEXE=./tools/XML2CppCode/build/gcc/Debug/XML2CppCode
command -v ${XML2CppCodeEXE} >/dev/null 2>&1 || { echo "XML2CppCode is not installed. Aborting." >&2; exit 1; }
command -v xsdcxx >/dev/null 2>&1 || { echo "xsdcxx is not installed. Aborting." >&2; exit 1; }

# generate exports file
PERL_SCRIPT=${ACE_ROOT}/bin/generate_export_file.pl
[ ! -f ${PERL_SCRIPT} ] && echo "ERROR: invalid script file \"${PERL_SCRIPT}\" (not a file), aborting" && exit 1
perl ${PERL_SCRIPT} -n RPG_Magic > ./magic/rpg_magic_exports.h
[ $? -ne 0 ] && echo "ERROR: failed to perl, aborting" && exit 1

# C++ "glue code"
${XML2CppCodeEXE} -e -f ./magic/etc/rpg_magic.xsd -i -o ./magic -s -u -x RPG_Magic
[ $? -ne 0 ] && echo "ERROR: failed to XML2CppCode, aborting" && exit 1

# XML Parser/Tree
# generate "XMLSchema" namespace include file (rpg_magic.xsd)
#xsdcxx cxx-parser --char-type char --output-dir ./magic --xml-parser xerces --force-overwrite --generate-xml-schema --skel-file-suffix "" --hxx-suffix .h --show-anonymous --show-sloc ./magic/rpg_XMLSchema_XML_types.xsd
#[ $? -ne 0 ] && echo "ERROR: failed to xsdcxx, aborting" && exit 1

# generate parser include/implementation (rpg_magic.xsd)
xsdcxx cxx-parser --type-map ./magic/etc/rpg_magic.map --char-type char --output-dir ./magic --namespace-map urn:rpg= --xml-parser xerces --force-overwrite --extern-xml-schema rpg_XMLSchema.h --skel-file-suffix _XML_types --hxx-suffix .h --cxx-suffix .cpp --show-anonymous --show-sloc ./magic/etc/rpg_magic.xsd
[ $? -ne 0 ] && echo "ERROR: failed to xsdcxx, aborting" && exit 1

## generate "XMLSchema" namespace include file (tree)
#xsdcxx cxx-tree --char-type char --output-dir ./magic --generate-serialization --generate-xml-schema --hxx-suffix .h --#show-anonymous --show-sloc ./magic/rpg_XMLSchema_XML_tree.xsd
#xsdcxx cxx-tree --char-type char --output-dir ./magic --generate-serialization --generate-insertion ACE_OutputCDR --generate-extraction ACE_InputCDR --generate-xml-schema --hxx-suffix .h --show-anonymous --show-sloc ./magic/rpg_XMLSchema_XML_tree.xsd
#[ $? -ne 0 ] && echo "ERROR: failed to xsdcxx, aborting" && exit 1

# generate tree include/implementation (rpg_magic.xsd)
xsdcxx cxx-tree --generate-serialization --generate-ostream --generate-comparison --type-regex '/(.+) RPG_(.+)_Type/RPG_\u$2_XMLTree_Type/' --char-type char --output-dir ./magic --namespace-map urn:rpg= --extern-xml-schema rpg_XMLSchema.h --hxx-suffix _XML_tree.h --cxx-suffix _XML_tree.cpp --show-anonymous --show-sloc ./magic/etc/rpg_magic.xsd
#xsdcxx cxx-tree --generate-serialization --generate-ostream --generate-comparison --generate-insertion ACE_OutputCDR --generate-extraction ACE_InputCDR --type-regex '/(.+) RPG_(.+)_Type/RPG_\u$2_XMLTree_Type/' --char-type char --output-dir ./magic --namespace-map urn:rpg= --extern-xml-schema rpg_XMLSchema.h --hxx-suffix _XML_tree.h --cxx-suffix _XML_tree.cpp --show-anonymous --show-sloc --export-symbol "RPG_Magic_Export" --hxx-prologue "#include \"rpg_magic_exports.h\"" ./magic/rpg_magic.xsd
if [ $? -ne 0 ]; then
 echo "ERROR: failed to xsdcxx, aborting"
 exit 1
fi
