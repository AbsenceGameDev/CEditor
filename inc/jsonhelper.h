#ifndef JSONHELPER_H
#define JSONHELPER_H

#include <fstream>
#include "json.hpp"
#include "ruleset.h"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CharacterClass, Name, PrimeStat, HitDieShape, ID)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CharacterRace, Name, MinimumStats, Modifiers, AbilityName, AllowedClassIDs, ID)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StatRollReturn, RollValue, ModifiedValue)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DnDCharacter, Name, Race, Class, AbilityScores, Hitpoints, Gold, Alignment, ID)

namespace JHelper
{
   namespace Class
   {
      inline CharacterClass LoadConfig(const std::string& FileName)
      {
         nlohmann::json json;
         std::ifstream filestream(FileName, std::ifstream::binary);
         filestream >> json;
         return json.get<CharacterClass>();
      }

      inline void SaveConfig(const CharacterClass& ClassDescr)
      {
         const nlohmann::json json = ClassDescr;
         
         // This won't be portable but works for now
         std::string ConfigsDir = "../../rsc/savedata/classes/";
         ConfigsDir = ConfigsDir + ClassDescr.Name + ".json";
         
         std::ofstream outfilestream(ConfigsDir.c_str());

         outfilestream << std::setw(4) << json << '\n';
      }
   }

   namespace Race
   {
      inline CharacterRace LoadConfig(const std::string& FileName)
      {
         nlohmann::json json;
         std::ifstream filestream(FileName, std::ifstream::binary);
         filestream >> json;
         return json.get<CharacterRace>();
      }
       
      inline void SaveConfig(const CharacterRace& RaceDescr)
      {
         const nlohmann::json json = RaceDescr;

         // This won't be portable but works for now
         std::string ConfigsDir = "../../rsc/savedata/races/";
         ConfigsDir = ConfigsDir + RaceDescr.Name + ".json";
         
         std::ofstream outfilestream(ConfigsDir.c_str());
         outfilestream << std::setw(4) << json << '\n';
      }
   }

   namespace Character
   {
      inline DnDCharacter LoadConfig(const std::string& FileName)
      {
         nlohmann::json json;
         std::ifstream filestream(FileName, std::ifstream::binary);
         filestream >> json;
         return json.get<DnDCharacter>();
      }
       
      inline void SaveConfig(const DnDCharacter& CharDescr)
      {
         const nlohmann::json json = CharDescr;

         // This won't be portable but works for now
         std::string ConfigsDir = "../../rsc/savedata/characters/";
         ConfigsDir = ConfigsDir + CharDescr.Name + ".json";

         std::ofstream outfilestream(ConfigsDir.c_str());

         outfilestream << std::setw(4) << json << '\n';
      }
   }

}

#endif // JSONHELPER_H
