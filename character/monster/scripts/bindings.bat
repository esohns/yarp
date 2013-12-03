@rem //%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem // File Name: bindings.bat
@rem //
@rem #// History:
@rem #//   Date   |Name | Description of modification
@rem #// ---------|-----|-------------------------------------------------------------
@rem #// 20/02/06 | soh | Creation.
@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////

@rem generate exports file
"E:\ACE_wrappers\bin\generate_export_file.pl" -n RPG_Monster > .\..\rpg_monster_exports.h

@rem C++ "glue code"
"E:\XML2CppCode\Debug\XML2CppCode.exe" -d RPG_Monster_Export -e -f .\..\rpg_monster.xsd -i -o .\.. -s -u -x RPG_Monster

@rem XML Parser
@rem generate "XMLSchema" namespace include file (rpg_monster.xsd)
"E:\Program Files\CodeSynthesis XSD 3.3\bin\xsd.exe" cxx-parser --char-type char --output-dir .\.. --xml-parser xerces --force-overwrite --generate-xml-schema --skel-file-suffix "" --hxx-suffix .h --show-anonymous --show-sloc ..\rpg_XMLSchema_XML_types.xsd

@rem generate include/implementation (rpg_monster.xsd)
"E:\Program Files\CodeSynthesis XSD 3.3\bin\xsd.exe" cxx-parser --type-map .\..\rpg_monster.map --char-type char --output-dir .\.. --namespace-map urn:rpg= --xml-parser xerces --force-overwrite --extern-xml-schema rpg_XMLSchema.h --skel-file-suffix _XML_types --hxx-suffix .h --cxx-suffix .cpp --show-anonymous --show-sloc --export-symbol "RPG_Monster_Export" --hxx-prologue "#include \"rpg_monster_exports.h\"" --cxx-prologue-file .\..\stdafx.cpp .\..\rpg_monster.xsd
