#include "../inc/editor_core.h"

#include "../inc/jsonhelper.h"
#include "../inc/ui.h"

std::map<int, DnDCharacter> EditorCore::MappedCharacters = {};
std::map<int, CharacterRace> EditorCore::MappedRaces     = {};
std::map<int, CharacterClass> EditorCore::MappedClasses  = {};

std::condition_variable EditorCore::LoadConfigCondVar = {};
std::mutex EditorCore::LoadConfigMutex                = {};
ConfigData EditorCore::SharedConfigData               = {};

std::vector<CharacterClass> ClassLoaderUtility::CachedStructs;
std::vector<CharacterRace>  RaceLoaderUtility::CachedStructs;
std::vector<DnDCharacter>   CharacterLoaderUtility::CachedStructs;

std::thread EditorCore::LoadDataThread        = {};
std::thread EditorCore::CopyLoadedDataThread  = {};

EditorCore::~EditorCore()
{
}

void EditorCore::CreateCharacter(const DnDCharacter& Character, bool Save)
{
   CharTreeIt it = MappedCharacters.find(Character.ID);
   if (it == MappedCharacters.end())
   {
      MappedCharacters.emplace(Character.ID, Character);
   }
   else
   {
      MappedCharacters[Character.ID] = Character;
   }
   
   if (Save == false) { return; }
   JHelper::Character::SaveConfig(Character);
}
void EditorCore::CreateClass(const CharacterClass& Class, bool Save)
{
   ClassTreeIt it = MappedClasses.find(Class.ID);
   if (it == MappedClasses.end())
   {
      MappedClasses.emplace(Class.ID, Class);   
   }
   else
   {
      MappedClasses[Class.ID] = Class;
   }
   
   if (Save == false) { return; }
   JHelper::Class::SaveConfig(Class);
}
void EditorCore::CreateRace(const CharacterRace& Race, bool Save)
{
   RaceTreeIt it = MappedRaces.find(Race.ID);
   if (it == MappedRaces.end())
   {
      MappedRaces.emplace(Race.ID, Race);   
   }
   else
   {
      MappedRaces[Race.ID] = Race;
   }

   if (Save == false) { return; }
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

void EditorCore::LoadConfigData()
{
   ClassLoaderUtility::Init();     // Classes have no dependencies, load them first
   RaceLoaderUtility::Init();      // Races depend on class IDs (of allowed classes) load Races second
   CharacterLoaderUtility::Init(); // Characters depend on Class and Race IDs (of allowed classes) load Characters last
   
   const ConfigData NewData{
      CharacterLoaderUtility::CachedStructs,
      ClassLoaderUtility::CachedStructs,
      RaceLoaderUtility::CachedStructs};
   // Work is done and data is ready
   
   {
      // Lock the mutex then write to the queue and notify the waiting thread
      std::lock_guard<std::mutex> l{LoadConfigMutex};
      SharedConfigData = NewData;
      SharedConfigData.FinishThread = true;
   }
   LoadConfigCondVar.notify_one();
}


void EditorCore::StoreData()
{
   
   // unique_lock and check the condition with a predicate
   std::unique_lock<std::mutex> Lock{LoadConfigMutex};
   LoadConfigCondVar.wait(Lock, []()
   {
      return EditorCore::SharedConfigData.FinishThread;
   });

   ConfigData ConfigData = EditorCore::SharedConfigData;
   Lock.unlock();

   EndReadConfigs(ConfigData);
}

void EditorCore::StartReadConfigs()
{
   UIHandler::BlockEditors = true;
   EditorCore::LoadDataThread  = std::thread(LoadConfigData);
   EditorCore::CopyLoadedDataThread  = std::thread(StoreData);
   EditorCore::LoadDataThread.detach();
   EditorCore::CopyLoadedDataThread.detach();
}

void EditorCore::EndReadConfigs(const ConfigData& Data)
{
   for (const DnDCharacter& Character : Data.Characters)
   {
      EditorCore::CreateCharacter(Character, false);
   }

   for (const CharacterClass& Class : Data.Classes)
   {
      EditorCore::CreateClass(Class, false);
   }

   for (const CharacterRace& Race : Data.Races)
   {
      EditorCore::CreateRace(Race, false);
   }
   UIHandler::BlockEditors = false;
}