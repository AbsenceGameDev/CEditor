#ifndef EDITOR_CORE_H
#define EDITOR_CORE_H

#include <condition_variable>
#include <mutex>
#include <thread>

#include <vector>
#include <map>

#include "jsonhelper.h"
#include "ruleset.h"

using CharTreeIt = const std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const int, DnDCharacter>>>>;
using RaceTreeIt = const std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const int, CharacterRace>>>>;
using ClassTreeIt = const std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const int, CharacterClass>>>>;

struct ConfigData
{
   std::vector<DnDCharacter>   Characters{};
   std::vector<CharacterClass> Classes{};
   std::vector<CharacterRace>  Races{};
   bool FinishThread = false;
};

class EditorCore
{
   ~EditorCore();
public:
   static void CreateCharacter(const DnDCharacter& Character, bool Save = true);
   static void CreateClass(const CharacterClass& Class, bool Save = true);
   static void CreateRace(const CharacterRace& Race, bool Save = true);

   static void DeleteCharacter(int CharacterID);
   static void DeleteClass(int ClassID);
   static void DeleteRace(int RaceID);

   static void LoadConfigData();
   static void StoreData();
   static void StartReadConfigs();
   static void EndReadConfigs(const struct ConfigData& Data);

   static std::map<int, DnDCharacter>   MappedCharacters;
   static std::map<int, CharacterRace>  MappedRaces;
   static std::map<int, CharacterClass> MappedClasses;

   static std::condition_variable LoadConfigCondVar;
   static std::mutex LoadConfigMutex;
   static ConfigData SharedConfigData;

   static std::thread LoadDataThread;
   static std::thread CopyLoadedDataThread;
};



#endif // EDITOR_CORE_H
