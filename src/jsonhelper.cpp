#include <fstream>
#include <string>

#include "imgui.h"
#include "json.h"

#include "../inc/ruleset.h"
#include "../inc/jsonhelper.h"

namespace JHelper
{
   // @note Blame nlohmanns buggy library for me resorting to deserializing the jsons by hand 
   struct json_object_s* FromFileToRoot(const std::string& FileName, struct json_value_s* root)
   {
      std::ifstream Rewind(FileName, std::ifstream::binary);
      if (Rewind.is_open() == false) { return nullptr; /* @todo log error */ }

      std::string Data(std::istreambuf_iterator<char>(Rewind), {});
      root = json_parse_ex(Data.c_str(), Data.size(), json_parse_flags_allow_simplified_json, json_null, json_null, json_null);
      if (root == nullptr) { return nullptr; /* @todo log error */ }

      const bool IsNotObject = root->type != json_type_object;
      const bool IsNotString = root->type != json_type_object;
      if (IsNotObject && IsNotString)
      {
         free(root);
         return nullptr; /* @todo log error */
      }
      
      return static_cast<struct json_object_s*>(root->payload);         
   }
      
   bool IsExpectedElement(const struct json_object_element_s* Element, const char* EntryName, json_type_e CompareType)
   {
      const json_string_s* NameEntryName = Element->name;
      if (0 != strcmp(NameEntryName->string, EntryName)
         || NameEntryName->string_size != strlen(EntryName)
         || Element->value->type != CompareType)
      {
         return false; /* @todo log error */
      }         
      return true; /* @todo log error */
   }

   namespace PoorMansDeserializer
   {
      void DeserializeArray(int* TargetArray, struct json_object_element_s* JsonEntry)
      {
         struct json_array_s* JArray = json_value_as_array(JsonEntry->value);
         json_array_element_s* ArrayEntry = JArray->start;
         for (size_t Step = 0; Step < JArray->length;)
         {
            TargetArray[Step++] = std::atoi(json_value_as_number(ArrayEntry->value)->number);
            ArrayEntry = ArrayEntry->next;
         }
      }

      void DeserializeStatRollArray(StatRollReturn* TargetArray, struct json_object_element_s* JsonEntry)
      {
         struct json_array_s* JArray = json_value_as_array(JsonEntry->value);
         json_array_element_s* ArrayEntry = JArray->start;
         for (size_t Step = 0; Step < JArray->length;)
         {
            const json_object_s* ElementObject = json_value_as_object(ArrayEntry->value);
            const json_object_element_s* ElementInner = ElementObject->start;
            const size_t CurrentElementObjectCount = MinV<size_t>(ElementObject->length, 2);

            StatRollReturn StatRollStep;
            for (size_t InnerStep = 0; InnerStep < CurrentElementObjectCount;)
            {
               if (IsExpectedElement(ElementInner, "RollValue", json_type_number))
               {
                  StatRollStep.RollValue = std::atoi(json_value_as_number(ElementInner->value)->number);
               }
               if (IsExpectedElement(ElementInner, "ModifiedValue", json_type_number))
               {
                  StatRollStep.ModifiedValue = std::atoi(json_value_as_number(ElementInner->value)->number);
               }
               
               InnerStep++;
               ElementInner = ElementInner->next;
            }

            TargetArray[Step++] = StatRollStep;
            ArrayEntry = ArrayEntry->next;
         }
      }   

      void DeserializeName(GenericName& Destination, struct json_object_element_s* NameEntry)
      {
         strcpy_s(Destination, json_value_as_string(NameEntry->value)->string);         
      }
      
      void DeserializeInteger(int& TargetValue, struct json_object_element_s* NumberEntry)
      {
         TargetValue = std::atoi(json_value_as_number(NumberEntry->value)->number);      
      }   
   }
   
   namespace PoorMansSerializer
   {
      
      
   }
}

namespace JHelper
{
   namespace Class
   {
      CharacterClass LoadConfig(const std::string& FileName)
      {
         auto ExitEarly = [](struct json_value_s* root) -> CharacterClass
         {
            free(root);
            return CharacterClass{};
         };
         
         CharacterClass Class;
         struct json_value_s* root = nullptr;
         struct json_object_s* object = FromFileToRoot(FileName, root);
         if(object == nullptr) { return Class; /* @todo log error */ }
         
         // Assign Name Value
         struct json_object_element_s* NameEntry = object->start;
         if (IsExpectedElement(NameEntry, "Name", json_type_string) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeName(Class.Name, NameEntry);
         
         // Assign ID Value
         struct json_object_element_s* IDEntry = NameEntry->next;
         if (IsExpectedElement(IDEntry, "ID", json_type_number) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeInteger(Class.ID, IDEntry);

         // Assign prime state
         struct json_object_element_s* PrimeStatEntry = IDEntry->next;
         if (IsExpectedElement(PrimeStatEntry, "PrimeStat", json_type_number) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeEnum<Spec::Character::StatType>(Class.PrimeStat, PrimeStatEntry);
         
         // Assign hit die
         struct json_object_element_s* HitDieEntry = PrimeStatEntry->next;
         if (IsExpectedElement(HitDieEntry, "HitDieShape", json_type_number) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeEnum<Platonic::Dice>(Class.HitDieShape, HitDieEntry);

         free(root); // Free the malloced memory for this read
         return Class;
      }
      
      void SaveConfig(const CharacterClass& ClassDescr)
      {
         const std::string SerializedString =
         PoorMansSerializer::StartSerialize()
            + PoorMansSerializer::SerializeGenericName<true>("Name", ClassDescr.Name)
            + PoorMansSerializer::SerializeGenericNumber("ID", ClassDescr.ID)
            + PoorMansSerializer::SerializeGenericNumber("PrimeStat", ClassDescr.PrimeStat)
            + PoorMansSerializer::SerializeGenericNumber("HitDieShape", ClassDescr.HitDieShape)
         + PoorMansSerializer::EndSerialize();
         
         // // This won't be portable but works for now
         std::string ConfigsDir = "../../rsc/savedata/classes/";
         ConfigsDir = ConfigsDir + ClassDescr.Name + ".json";
         
         std::ofstream outfilestream(ConfigsDir.c_str());
         outfilestream << SerializedString;
      }
   }
   namespace Race
   {
      CharacterRace LoadConfig(const std::string& FileName)
      {
         auto ExitEarly = [](struct json_value_s* root) -> CharacterRace
         {
            free(root);
            return CharacterRace{};
         };
         
         CharacterRace Race;
         struct json_value_s* root = nullptr;
         const struct json_object_s* object = FromFileToRoot(FileName, root);
         if(object == nullptr) { return Race; /* @todo log error */ }
         
         // Assign Name Value
         struct json_object_element_s* NameEntry = object->start;
         if (IsExpectedElement(NameEntry, "Name", json_type_string) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeName(Race.Name, NameEntry);

         // Assign ID Value
         struct json_object_element_s* IDEntry = NameEntry->next;
         if (IsExpectedElement(IDEntry, "ID", json_type_number) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeInteger(Race.ID, IDEntry);
         
         // Ability Name Value
         struct json_object_element_s* AbilityEntry = IDEntry->next;
         if (IsExpectedElement(AbilityEntry, "AbilityName", json_type_string) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeName(Race.AbilityName, AbilityEntry);

         // Minimum stats array
         struct json_object_element_s* MinimumStatsEntry = AbilityEntry->next;
         if (IsExpectedElement(MinimumStatsEntry, "MinimumStats", json_type_array) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeArray(&Race.MinimumStats[0], MinimumStatsEntry);
         
         // Modifiers array
         struct json_object_element_s* ModifiersEntry = MinimumStatsEntry->next;
         if (IsExpectedElement(ModifiersEntry, "Modifiers", json_type_array) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeArray(&Race.Modifiers[0], ModifiersEntry);

         // AllowedClassIDs array
         struct json_object_element_s* AllowedClassIDsEntry = ModifiersEntry->next;
         if (IsExpectedElement(AllowedClassIDsEntry, "AllowedClassIDs", json_type_array) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeArray(&Race.AllowedClassIDs[0], AllowedClassIDsEntry);         

         free(root);
         return Race;
      }
    
      void SaveConfig(const CharacterRace& RaceDescr)
      {
         const std::string SerializedString =
         PoorMansSerializer::StartSerialize()
            + PoorMansSerializer::SerializeGenericName<true>("Name", RaceDescr.Name)
            + PoorMansSerializer::SerializeGenericNumber("ID", RaceDescr.ID)
            + PoorMansSerializer::SerializeGenericName("AbilityName", RaceDescr.AbilityName)
            + PoorMansSerializer::SerializeIntArray("MinimumStats", &RaceDescr.MinimumStats[0], 6)
            + PoorMansSerializer::SerializeIntArray("Modifiers", &RaceDescr.Modifiers[0], 6)
            + PoorMansSerializer::SerializeIntArray("AllowedClassIDs", &RaceDescr.AllowedClassIDs[0], 20)
         + PoorMansSerializer::EndSerialize();
         
         // // This won't be portable but works for now
         std::string ConfigsDir = "../../rsc/savedata/races/";
         ConfigsDir = ConfigsDir + RaceDescr.Name + ".json";
         
         std::ofstream outfilestream(ConfigsDir.c_str());
         outfilestream << SerializedString;         
         
      }
   }
   
   namespace Character
   {
      DnDCharacter LoadConfig(const std::string& FileName)
      {
         auto ExitEarly = [](struct json_value_s* root) -> DnDCharacter
         {
            free(root);
            return DnDCharacter{};
         };
         
         DnDCharacter Character;
         struct json_value_s* root = nullptr;
         struct json_object_s* object = FromFileToRoot(FileName, root);
         if(object == nullptr) { return Character; /* @todo log error */ }
         
         // Assign Name Value
         struct json_object_element_s* NameEntry = object->start;
         if (IsExpectedElement(NameEntry, "Name", json_type_string) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeName(Character.Name, NameEntry);

         // Assign Character ID Value
         struct json_object_element_s* IDEntry = NameEntry->next;
         if (IsExpectedElement(IDEntry, "ID", json_type_number) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeInteger(Character.ID, IDEntry);
         
         // Race ID
         struct json_object_element_s* RaceIDEntry = IDEntry->next;
         if (IsExpectedElement(RaceIDEntry, "RaceID", json_type_number) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeInteger(Character.Race.ID, RaceIDEntry);

         // Class ID
         struct json_object_element_s* ClassIDEntry = RaceIDEntry->next;
         if (IsExpectedElement(ClassIDEntry, "ClassID", json_type_number) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeInteger(Character.Class.ID, ClassIDEntry);
         
         // StatRoll Array[6]
         struct json_object_element_s* StatRollEntry = ClassIDEntry->next;
         if (IsExpectedElement(StatRollEntry, "AbilityScores", json_type_array) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeStatRollArray(&Character.AbilityScores[0], StatRollEntry);

         // Alignment Enum
         struct json_object_element_s* AlignmentEntry = StatRollEntry->next;
         if (IsExpectedElement(AlignmentEntry, "Alignment", json_type_number) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeEnum<Spec::Character::Alignment>(Character.Alignment, AlignmentEntry);                 

         // Hitpoints int
         struct json_object_element_s* HitpointsEntry = AlignmentEntry->next;
         if (IsExpectedElement(HitpointsEntry, "Hitpoints", json_type_number) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeInteger(Character.Hitpoints, HitpointsEntry);

         // Gold int
         struct json_object_element_s* GoldEntry = HitpointsEntry->next;
         if (IsExpectedElement(GoldEntry, "Gold", json_type_number) == false) { return ExitEarly(root); /* @todo log error */ }
         PoorMansDeserializer::DeserializeInteger(Character.Gold, GoldEntry);                 

         free(root);
         return Character;
      }
    
      void SaveConfig(const DnDCharacter& CharDescr)
      {
         const std::string SerializedString =
         PoorMansSerializer::StartSerialize()
            + PoorMansSerializer::SerializeGenericName<true>("Name", CharDescr.Name)
            + PoorMansSerializer::SerializeGenericNumber("ID", CharDescr.ID)
            + PoorMansSerializer::SerializeGenericNumber("RaceID", CharDescr.Race.ID)
            + PoorMansSerializer::SerializeGenericNumber("ClassID", CharDescr.Class.ID)
            + PoorMansSerializer::SerializeAbilityScores("AbilityScores", &CharDescr.AbilityScores[0], 6)
            + PoorMansSerializer::SerializeGenericNumber("Alignment", CharDescr.Alignment)
            + PoorMansSerializer::SerializeGenericNumber("Hitpoints", CharDescr.Hitpoints)
            + PoorMansSerializer::SerializeGenericNumber("Gold", CharDescr.Gold)
         + PoorMansSerializer::EndSerialize();

         // // This won't be portable but works for now
         std::string ConfigsDir = "../../rsc/savedata/characters/";
         ConfigsDir = ConfigsDir + CharDescr.Name + ".json";
         
         std::ofstream outfilestream(ConfigsDir.c_str());
         outfilestream << SerializedString;             
      }
   }
}