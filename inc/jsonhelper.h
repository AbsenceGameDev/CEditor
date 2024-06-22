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
         std::string ConfigsDir = "./../rsc/savedata/classes";
         std::ofstream outfilestream(ConfigsDir.append("/").append(ClassDescr.Name)); // This won't be portable but works for now
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
         std::string ConfigsDir = "./../rsc/savedata/races";
         std::ofstream outfilestream(ConfigsDir.append("/").append(RaceDescr.Name)); // This won't be portable but works for now
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
         std::string ConfigsDir = "./../rsc/savedata/characters";
         std::ofstream outfilestream(ConfigsDir.append("/").append(CharDescr.Name)); // This won't be portable but works for now
         outfilestream << std::setw(4) << json << '\n';
      }
   }

}

#endif // JSONHELPER_H
