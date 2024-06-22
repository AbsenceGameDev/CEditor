#ifndef EDITOR_CORE_H
#define EDITOR_CORE_H

#include <fstream>
#include <map>

#include "ruleset.h"

using CharTreeIt = const std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const int, DnDCharacter>>>>;
using RaceTreeIt = const std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const int, CharacterRace>>>>;
using ClassTreeIt = const std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const int, CharacterClass>>>>;

class EditorCore
{
public:
   static void CreateCharacter(const DnDCharacter& Character);
   static void CreateClass(const CharacterClass& Class);
   static void CreateRace(const CharacterRace& Race);

   static void DeleteCharacter(int CharacterID);
   static void DeleteClass(int ClassID);
   static void DeleteRace(int RaceID);   

   static std::map<int, DnDCharacter> MappedCharacters;
   static std::map<int, CharacterRace> MappedRaces;
   static std::map<int, CharacterClass> MappedClasses;
};

#endif // EDITOR_CORE_H
