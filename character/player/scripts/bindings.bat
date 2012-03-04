@rem //%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem // File Name: bindings.bat
@rem //
@rem #// History:
@rem #//   Date   |Name | Description of modification
@rem #// ---------|-----|-------------------------------------------------------------
@rem #// 20/02/06 | soh | Creation.
@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////

@rem generate exports file
"E:\ACE_wrappers\bin\generate_export_file.pl" -n RPG_Player > .\..\rpg_player_exports.h

@rem C++ "glue code"
"E:\XML2CppCode\Debug\XML2CppCode.exe" -d RPG_Player_Export -e -f .\..\rpg_player.xsd -i -o .\.. -s -u -x RPG_Player

@rem generate "XMLSchema" namespace include file (tree)
"E:\Program Files\CodeSynthesis XSD 3.3\bin\xsd.exe" cxx-tree --char-type char --output-dir .\.. --generate-serialization --generate-insertion ACE_OutputCDR --generate-extraction ACE_InputCDR --generate-xml-schema --hxx-suffix .h --show-anonymous --show-sloc ..\rpg_XMLSchema_XML_tree.xsd

@rem generate tree include/implementation
"E:\Program Files\CodeSynthesis XSD 3.3\bin\xsd.exe" cxx-tree --generate-polymorphic --generate-serialization --generate-ostream --generate-comparison --generate-insertion ACE_OutputCDR --generate-extraction ACE_InputCDR --type-regex "/(.+) RPG_(.+)_Type/RPG_\u$2_XMLTree_Type/" --char-type char --output-dir .\.. --namespace-map urn:rpg= --export-xml-schema --extern-xml-schema rpg_XMLSchema.h --hxx-suffix _XML_tree.h --cxx-suffix _XML_tree.cpp --show-anonymous --show-sloc --export-symbol "RPG_Player_Export" --hxx-prologue "#include \"rpg_player_exports.h\"" .\..\rpg_player.xsd
