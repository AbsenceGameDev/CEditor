#ifndef DATAFORMAT_H
#define DATAFORMAT_H

#include <fstream>

#include "ruleset.h"

#define MASK_BYTE(Value, ByteIdx) (unsigned char)(((Value) >> ((ByteIdx)*8)) & 0xffff)
#define RESTORE_BYTE(Bytes, ByteIdx) ((((int)((Bytes)[ByteIdx])) << ((ByteIdx)*8)))

namespace Data
{
   namespace Format
   {
      constexpr int FormatSize = 52;
      constexpr int NameSize = 52;
      constexpr int IDPadding = 4;

      constexpr int ClassSize = NameSize + IDPadding;
      constexpr int RaceSize = FormatSize + NameSize;
      constexpr int CharSize = FormatSize + NameSize + IDPadding;

      /** @brief Packet Format : [ID:2Byte, Name:52Byte, PrimeStat:1Byte, HitDieShape:1Byte] */
      using Class = char[ClassSize];
      /** @brief Packet Format : [ID:2Byte, Name:52Byte, AllowedClassIDs:20Byte, Modifiers:6Byte, MinimumStat:6Byte] */
      using Race = char[RaceSize];
      /** @brief Packet Format : [ID:4Byte, Name:52Byte, ClassID:2Byte, RaceID:2Byte, AbilityScores:12Bytes, Alignment:1Byte, Gold:2Bytes, Hitpoints:2Bytes] */
      using Char = char[CharSize];

      using GenericName = char[NameSize];
   }
}

namespace Data
{
   typedef struct Handler
   {
      static void SaveCharacterFormat(const DnDCharacter& Character)
      {
         Format::Char ChrFormat;
         MakeCharacterFormat(Character, ChrFormat);

         char buffer[Format::CharSize];
         std::ofstream File;
         File.rdbuf()->pubsetbuf(buffer, sizeof(buffer));

         File.open("..\\rsc\\CharacterData.CEdat");
         for (std::size_t i = 0; i < Format::CharSize; i++)
         {
            File << ChrFormat[i] << '\n';
         }
      }

      static void SaveCharacterFormat(const CharacterRace& Race)
      {
         Format::Race RFormat{};
         MakeRaceFormat(Race, RFormat);

         char buffer[Format::RaceSize];
         std::ofstream File;
         File.rdbuf()->pubsetbuf(buffer, sizeof(buffer));

         File.open("..\\rsc\\RaceData.CEdat");
         for (std::size_t i = 0; i < Format::RaceSize; i++)
         {
            File << RFormat[i] << '\n';
         }
      }

      static void SaveClassFormat(const CharacterClass& Class)
      {
         Format::Class CFormat{};
         MakeClassFormat(Class, CFormat);

         char buffer[Format::ClassSize];
         std::ofstream File;
         File.rdbuf()->pubsetbuf(buffer, sizeof(buffer));

         File.open("..\\rsc\\ClassData.CEdat");
         for (std::size_t i = 0; i < Format::ClassSize; i++)
         {
            File << CFormat[i] << '\n';
         }
      }

      static void LoadClassFormat()
      {
      }

   private:
      // Each race format needs a minimum of 32 bits, if we have 100 different classes stored to file it will take up about 3.2kb
      static void MakeClassFormat(const CharacterClass& Class, Format::Class& Format)
      {
         int StepFormat = 0;

         //
         // ID
         const int ClassID = Class.GetID(); // we can get away with only using the first 16 bits
         Format[StepFormat++] = MASK_BYTE(ClassID, 0);
         Format[StepFormat++] = MASK_BYTE(ClassID, 1);

         //
         // Name
         for (int StepArray = 0; StepArray < Format::NameSize;)
         {
            Format[StepFormat++] = Class.Name[StepArray++];
         }

         //
         // Appendix
         Format[StepFormat++] = Class.PrimeStat; // 8 bits : 6 possible enum values, reserved two values
         Format[StepFormat++] = Class.HitDieShape; // 8 bits : 5 possible values enum values, reserved three values
      }

      // Each race format needs a minimum of 444 bits, if we have 100 different races stored to file it will take up about 90kb
      static void MakeRaceFormat(const CharacterRace& Race, Format::Race& Format)
      {
         int StepFormat = 0;

         //
         // ID         
         const int RaceID = Race.GetID(); // we can get away with only using the first 16 bits
         Format[StepFormat++] = MASK_BYTE(RaceID, 0); // NOLINT
         Format[StepFormat++] = MASK_BYTE(RaceID, 1);

         //
         // Name         
         for (int StepArray = 0; StepArray < Format::NameSize;)
         {
            Format[StepFormat++] = Race.AbilityName[StepArray++];
         }

         //
         // Appendix         
         for (int StepArray = 0; StepArray < 20;)
         // Character.Race.AllowedClassIDs; // 16  * 20 bits : 320 bits, only store the first two bytes, we won't have 16k races  so we can get away with is
         {
            const int StepID = Race.AllowedClassIDs[StepArray++];
            Format[StepFormat++] = MASK_BYTE(StepID, 0);
            Format[StepFormat++] = MASK_BYTE(StepID, 1);
         }
         for (int StepArray = 0; StepArray < 6;)
         // Character.Race.Modifiers; // 8 * 6  bits : 48 bits // we need about 5 bits per modifier and we have 6 stats we can affect  
         {
            const int ModValue = Race.Modifiers[StepArray++];
            Format[StepFormat++] = MASK_BYTE(ModValue, 0);
         }
         for (int StepArray = 0; StepArray < 6;)
         // Character.Race.MinimumStats; // 8 * 6  bits : 48 bits // we need about 5 bits per min_stat and we have 6 stats we can affect
         {
            const int ModValue = Race.MinimumStats[StepArray++];
            Format[StepFormat++] = MASK_BYTE(ModValue, 0);
         }
      }

      static void MakeCharacterFormat(const DnDCharacter& Character, Format::Char& ChrFormat)
      {
         int StepFormat = 0;

         //
         // ID           
         const int CharID = Character.GetID(); // use all 32 bits
         ChrFormat[StepFormat++] = MASK_BYTE(CharID, 0);
         ChrFormat[StepFormat++] = MASK_BYTE(CharID, 1);
         ChrFormat[StepFormat++] = MASK_BYTE(CharID, 2);
         ChrFormat[StepFormat++] = MASK_BYTE(CharID, 3);

         //
         // Name    
         for (int StepArray = 0; StepArray < Format::NameSize;)
         {
            ChrFormat[StepFormat++] = Character.Name[StepArray++];
         }

         const int ClassID = Character.Class.GetID(); // we can get away with only using the first 16 bits
         ChrFormat[StepFormat++] = MASK_BYTE(ClassID, 0);
         ChrFormat[StepFormat++] = MASK_BYTE(ClassID, 1);

         const int RaceID = Character.Race.GetID(); // we can get away with only using the first 16 bits
         ChrFormat[StepFormat++] = MASK_BYTE(RaceID, 0);
         ChrFormat[StepFormat++] = MASK_BYTE(RaceID, 1);

         // Character.AbilityScores; // 16 * 6 bits
         for (int StepArray = 0; StepArray < 6;)
         {
            const StatRollReturn StatRollStep = Character.AbilityScores[StepArray++];
            ChrFormat[StepFormat++] = MASK_BYTE(StatRollStep.RollValue, 0);
            ChrFormat[StepFormat++] = MASK_BYTE(StatRollStep.ModifiedValue, 0);
         }

         // State based
         ChrFormat[StepFormat++] = MASK_BYTE(Character.Alignment, 0); // 9 possible values, store in a single byte
         ChrFormat[StepFormat++] = MASK_BYTE(Character.Gold, 0); // Store only the first 16 bits of gold, anything above is excessive 
         ChrFormat[StepFormat++] = MASK_BYTE(Character.Gold, 1);
         ChrFormat[StepFormat++] = MASK_BYTE(Character.Hitpoints, 0);
         // Store only the first 16 bits of hitpoints, anything above is excessive
         ChrFormat[StepFormat++] = MASK_BYTE(Character.Hitpoints, 1);
      }
   } dataformathandler_t;
}

#endif // DATAFORMAT_H
