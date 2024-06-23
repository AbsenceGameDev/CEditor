#ifndef JSONHELPER_H
#define JSONHELPER_H

#ifndef _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif

#include <experimental/filesystem>

#include "ruleset.h"

void StartReadConfigs();
void EndReadConfigs(const struct ConfigData& Data);

namespace JHelper
{
   namespace Class
   {
      CharacterClass LoadConfig(const std::string& FileName);
      void SaveConfig(const CharacterClass& ClassDescr);
   }

   namespace Race
   {
      CharacterRace LoadConfig(const std::string& FileName); 
      void SaveConfig(const CharacterRace& RaceDescr);
   }

   namespace Character
   {
      DnDCharacter LoadConfig(const std::string& FileName);
      void SaveConfig(const DnDCharacter& CharDescr);
   }
}

template<typename TStructType>
struct LoaderUtilityBase
{
   virtual ~LoaderUtilityBase() {};
   static void Init()  { return; }
   static std::vector<TStructType> ScanDirectory();
};
   
struct ClassLoaderUtility final : LoaderUtilityBase<CharacterClass>
{
   static void Init() { CachedStructs = ScanDirectory();}
   static std::vector<CharacterClass> CachedStructs;
};

struct RaceLoaderUtility final : LoaderUtilityBase<CharacterRace>
{
   static void Init() { CachedStructs = ScanDirectory();}
   static std::vector<CharacterRace> CachedStructs;
};

struct CharacterLoaderUtility final : LoaderUtilityBase<DnDCharacter>
{
   static void Init() { CachedStructs = ScanDirectory();}
   static std::vector<DnDCharacter> CachedStructs;
};   

template<>
inline std::vector<CharacterClass> LoaderUtilityBase<CharacterClass>::ScanDirectory()
{
   std::vector<CharacterClass> LoadedConfigs{};
   std::experimental::filesystem::path FinalPath = std::experimental::filesystem::current_path()  / ".."/ ".." / "rsc" / "savedata" / "classes" / "";      
   for (const std::experimental::filesystem::directory_entry& DirEntry : std::experimental::filesystem::directory_iterator(FinalPath))
   {
      LoadedConfigs.emplace_back(JHelper::Class::LoadConfig(DirEntry.path().string()));
   }

   return LoadedConfigs;
}

template<>
inline std::vector<CharacterRace> LoaderUtilityBase<CharacterRace>::ScanDirectory()
{
   std::experimental::filesystem::path FinalPath = std::experimental::filesystem::current_path()  / ".."/ ".." / "rsc" / "savedata" / "races" / "\0";      
   std::vector<CharacterRace> LoadedConfigs{};
   for (const std::experimental::filesystem::directory_entry& DirEntry : std::experimental::filesystem::directory_iterator(FinalPath))
   {
      LoadedConfigs.emplace_back(JHelper::Race::LoadConfig(DirEntry.path().string()));
   }
   return LoadedConfigs;
}

template<>
inline std::vector<DnDCharacter> LoaderUtilityBase<DnDCharacter>::ScanDirectory()
{
   std::experimental::filesystem::path FinalPath = std::experimental::filesystem::current_path()  / ".."/ ".." / "rsc" / "savedata" / "characters" / "\0";      
   std::vector<DnDCharacter> LoadedConfigs{};
   
   for (const std::experimental::filesystem::directory_entry& DirEntry : std::experimental::filesystem::directory_iterator(FinalPath))
   {
      LoadedConfigs.emplace_back(JHelper::Character::LoadConfig(DirEntry.path().string()));
   }
   return LoadedConfigs;
}   


#endif // JSONHELPER_H
