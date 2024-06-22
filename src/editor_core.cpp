#include "../inc/editor_core.h"

#include "../inc/jsonhelper.h"

std::map<int, DnDCharacter> EditorCore::MappedCharacters = {};
std::map<int, CharacterRace> EditorCore::MappedRaces     = {};
std::map<int, CharacterClass> EditorCore::MappedClasses  = {};

void EditorCore::CreateCharacter(const DnDCharacter& Character)
{
   CharTreeIt it = MappedCharacters.find(Character.ID);
   if (it == MappedCharacters.end())
   {
      MappedCharacters.emplace(Character.ID, Character);
      return;
   }
      
   MappedCharacters[Character.ID] = Character;
   
   JHelper::Character::SaveConfig(Character);
}
void EditorCore::CreateClass(const CharacterClass& Class)
{
   ClassTreeIt it = MappedClasses.find(Class.ID);
   if (it == MappedClasses.end())
   {
      MappedClasses.emplace(Class.ID, Class);   
      return;
   }
      
   MappedClasses[Class.ID] = Class;

   JHelper::Class::SaveConfig(Class);
}
void EditorCore::CreateRace(const CharacterRace& Race)
{
   RaceTreeIt it = MappedRaces.find(Race.ID);
   if (it == MappedRaces.end())
   {
      MappedRaces.emplace(Race.ID, Race);   
      return;
   }
      
   MappedRaces[Race.ID] = Race;
   
   JHelper::Race::SaveConfig(Race); // save to file
}

void EditorCore::DeleteCharacter(int CharacterID)
{
   CharTreeIt it = MappedCharacters.find(CharacterID);
   if (it == MappedCharacters.end()) { return; }

   MappedCharacters.erase(CharacterID);
}
void EditorCore::DeleteClass(int ClassID)
{
   ClassTreeIt it = MappedClasses.find(ClassID);
   if (it == MappedClasses.end()) { return; }

   MappedClasses.erase(ClassID);
}
void EditorCore::DeleteRace(int RaceID)
{
   RaceTreeIt it = MappedRaces.find(RaceID);
   if (it == MappedRaces.end()) { return; }

   MappedRaces.erase(RaceID);
} 