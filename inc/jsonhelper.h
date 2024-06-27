#ifndef JSONHELPER_H
#define JSONHELPER_H

#ifndef _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif

#include <experimental/filesystem>
#include <string>

#include "ruleset.h"

#define PRM(payload)  (#payload), (payload)

void StartReadConfigs();
void EndReadConfigs(const struct ConfigData& Data);

namespace JHelper
{
   // @note Blame nlohmanns buggy library for me resorting to deserializing the jsons by hand 
   struct json_object_s* FromFileToRoot(const std::string& FileName, struct json_value_s* root);
   bool IsExpectedElement(const struct json_object_element_s* Element, const char* EntryName, enum json_type_e CompareType);
   namespace PoorMansDeserializer
   {
      void DeserializeArray(int* TargetArray, struct json_object_element_s* JsonEntry);
      void DeserializeStatRollArray(StatRollReturn* TargetArray, struct json_object_element_s* JsonEntry);
      void DeserializeName(GenericName& Destination, struct json_object_element_s* NameEntry);

      template<typename TEnumType = Spec::Character::StatType>
      void DeserializeEnum(TEnumType& TargetEnum, struct json_object_element_s* EnumEntry)
      {
         TargetEnum = static_cast<TEnumType>(std::atoi(json_value_as_number(EnumEntry->value)->number));      
      }
      void DeserializeInteger(int& TargetValue, struct json_object_element_s* NumberEntry);
   }
   
   namespace PoorMansSerializer
   {
      inline std::string StartSerialize(const std::string& Append = std::string("")) { return Append + R"({)"; }
      inline std::string EndSerialize() { return R"(})"; }

      template<bool IsFirstEntry = false, bool IsWithoutStartingBlock = true>
      std::string SerializeGenericName(const char* NameLabel, const GenericName& Name)
      {
         return
            IsFirstEntry
            ? IsWithoutStartingBlock == false
               ? std::string(  R"({")" + std::string(NameLabel) + R"(" : ")" + std::string(Name)  + R"(")" )  
               : std::string(  R"(    ")" + std::string(NameLabel) + R"(" : ")" + std::string(Name)  + R"(")" )
            : std::string(     R"(,    ")" + std::string(NameLabel) + R"(" : ")" + std::string(Name)  + R"(")" );
      }

      template<bool IsFirstEntry = false, bool IsWithoutStartingBlock = true, typename TNumber = int>
      std::string SerializeGenericNumber(const char* NumberLabel, TNumber Value)
      {
         return
            IsFirstEntry
            ? IsWithoutStartingBlock == false
               ? std::string(  R"({")" + std::string(NumberLabel) + R"(" : )" + std::to_string(Value))  
               : std::string(  R"(    ")" + std::string(NumberLabel) + R"(" : )" + std::to_string(Value))
            : std::string(     R"(,    ")" + std::string(NumberLabel) + R"(" : )" + std::to_string(Value));      
      }

      template<bool IsFirstEntry = false, bool IsWithoutStartingBlock = true>
      std::string SerializeVec2(const char* NumberLabel, struct ImVec2& Value)
      {
         const std::string Tail = R"([)" + std::to_string(Value.x) + R"( , )" + std::to_string(Value.y) + R"(])" ;
         return
            IsFirstEntry
            ? IsWithoutStartingBlock == false
               ? std::string(  R"({")" + std::string(NumberLabel) + R"(" : )" + Tail) 
               : std::string(  R"(    ")" + std::string(NumberLabel) + R"(" : )" + Tail)
            : std::string(     R"(,    ")" + std::string(NumberLabel) + R"(" : )" + Tail);      
      }      


      template<bool IsFirstEntry = false, bool IsWithoutStartingBlock = true>
      std::string SerializeIntArray(const char* ArrayLabel, const int* Values, int ArraySize)
      {
         std::string Inner{"[ "};
         for (int Step = 0; Step < ArraySize; )
         {
            int NextStep = Step + 1;
            Inner += std::to_string(Values[Step]) + (NextStep >= ArraySize ? std::string{} : std::string{","});
            ++Step;
         }
         Inner += std::string{"]"};
         
         return
            IsFirstEntry
            ? IsWithoutStartingBlock == false
               ? std::string(  R"({")" + std::string(ArrayLabel) + R"(" : )"     + Inner)  
               : std::string(  R"(    ")" + std::string(ArrayLabel) + R"(" : )"  + Inner)
            : std::string(     R"(,    ")" + std::string(ArrayLabel) + R"(" : )" + Inner);      
      }


      template<bool IsFirstEntry = false, bool IsWithoutStartingBlock = true>
      std::string SerializeAbilityScores(const char* ArrayLabel, const StatRollReturn* Values, int ArraySize)
      {
         std::string Inner{"[ "};
         for (int Step = 0; Step < ArraySize; )
         {
            const StatRollReturn& CurrentRollStep = Values[Step++];
            Inner += "{";
            Inner += R"("RollValue" : )" + std::to_string(CurrentRollStep.RollValue);
            Inner += R"(, "ModifiedValue" : )" + std::to_string(CurrentRollStep.ModifiedValue);
            Inner += "}";
            Inner += (Step >= ArraySize ? std::string{} : std::string{","});
         }
         Inner += std::string{"]"};
         
         return
            IsFirstEntry
            ? IsWithoutStartingBlock == false
               ? std::string(  R"({")" + std::string(ArrayLabel) + R"(" : )"     + Inner)  
               : std::string(  R"(    ")" + std::string(ArrayLabel) + R"(" : )"  + Inner)
            : std::string(     R"(,    ")" + std::string(ArrayLabel) + R"(" : )" + Inner);      
      }     

   }
}

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
