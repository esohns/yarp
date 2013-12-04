@rem //%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem // File Name: bindings.bat
@rem //
@rem #// History:
@rem #//   Date   |Name | Description of modification
@rem #// ---------|-----|-------------------------------------------------------------
@rem #// 20/02/06 | soh | Creation.
@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////

@rem generate exports file
C:\Perl\bin\perl.exe C:\Temp\ACE_wrappers\bin\generate_export_file.pl -n RPG_Dice > .\..\rpg_dice_exports.h

@rem C++ "glue code"
C:\Tools\XML2CppCode.exe -d RPG_Dice_Export -e -f .\..\rpg_dice.xsd -i -o .\.. -s -u -x RPG_Dice

@rem generate "XMLSchema" namespace include file (rpg_dice.xsd)
"C:\Program Files\CodeSynthesis XSD 3.3\bin\xsd.exe" cxx-parser --char-type char --output-dir .\.. --xml-parser xerces --force-overwrite --generate-xml-schema --skel-file-suffix "" --hxx-suffix .h --show-anonymous --show-sloc ..\rpg_XMLSchema_XML_types.xsd

@rem generate parser include/implementation (rpg_dice.xsd)
"C:\Program Files\CodeSynthesis XSD 3.3\bin\xsd.exe" cxx-parser --type-map .\..\rpg_dice.map --char-type char --output-dir .\.. --namespace-map urn:rpg= --xml-parser xerces --force-overwrite --extern-xml-schema rpg_XMLSchema.h --skel-file-suffix _XML_types --hxx-suffix .h --cxx-suffix .cpp --show-anonymous --show-sloc --export-symbol "RPG_Dice_Export" --hxx-prologue "#include \"rpg_dice_exports.h\"" --cxx-prologue-file .\..\stdafx.cpp .\..\rpg_dice.xsd

@rem generate "XMLSchema" namespace include file (tree)
@rem "C:\Program Files\CodeSynthesis XSD 3.3\bin\xsd.exe" cxx-tree --char-type char --output-dir .\.. --generate-serialization --generate-insertion ACE_OutputCDR --generate-extraction ACE_InputCDR --generate-xml-schema --hxx-suffix .h --show-anonymous --show-sloc ..\rpg_XMLSchema_XML_tree.xsd
"C:\Program Files\CodeSynthesis XSD 3.3\bin\xsd.exe" cxx-tree --char-type char --output-dir .\.. --generate-serialization --generate-xml-schema --hxx-suffix .h --show-anonymous --show-sloc ..\rpg_XMLSchema_XML_tree.xsd

@rem generate tree include/implementation
@rem "C:\Program Files\CodeSynthesis XSD 3.3\bin\xsd.exe" cxx-tree --generate-polymorphic --generate-serialization --generate-ostream --generate-comparison --generate-insertion ACE_OutputCDR --generate-extraction ACE_InputCDR --type-regex "/(.+) RPG_(.+)_Type/RPG_\u$2_XMLTree_Type/" --char-type char --output-dir .\.. --namespace-map urn:rpg= --export-xml-schema --extern-xml-schema rpg_XMLSchema.h --hxx-suffix _XML_tree.h --cxx-suffix _XML_tree.cpp --show-anonymous --show-sloc --export-symbol "RPG_Dice_Export" --hxx-prologue "#include \"rpg_dice_exports.h\"" --cxx-prologue-file .\..\stdafx.cpp .\..\rpg_dice.xsd
"C:\Program Files\CodeSynthesis XSD 3.3\bin\xsd.exe" cxx-tree --generate-polymorphic --generate-serialization --generate-ostream --generate-comparison --type-regex "/(.+) RPG_(.+)_Type/RPG_\u$2_XMLTree_Type/" --char-type char --output-dir .\.. --namespace-map urn:rpg= --export-xml-schema --extern-xml-schema rpg_XMLSchema.h --hxx-suffix _XML_tree.h --cxx-suffix _XML_tree.cpp --show-anonymous --show-sloc --export-symbol "RPG_Dice_Export" --hxx-prologue "#include \"rpg_dice_exports.h\"" --cxx-prologue-file .\..\stdafx.cpp .\..\rpg_dice.xsd
