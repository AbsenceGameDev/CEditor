#include "../../inc/ui/editor_widgets.h"


#include "imgui_internal.h"
#include "../../inc/ui/settings.h"
#include "../../inc/ui/handler.h"

//
// Grid element viewer
template <class TType = CharacterClass, CallbackSpace TCallbackSpace = &UI::Callbacks::Race::SelectAllowedClass>
void RequestNewTemplateEntryForDataWithID(Template* TGroup, std::string FourCharacterPrefix, const TType& ClassData, int Override)
{
   const std::string NameAsString = FourCharacterPrefix + ClassData.Name;
   const GridElement PotentiallyNewElement = GridElement(NameAsString, UI::CharacterEditorStyle, ClassData.ID, Override, false, true,
                                                         false, DummyStateDelegate, TCallbackSpace, DummyCallbackTarget);

   const bool DidElementExist = TGroup->DoesGridElementExist(PotentiallyNewElement);
   if (DidElementExist == false) { TGroup->Emplace(PotentiallyNewElement); }
}
template <class TType = CharacterClass, class IteratorType = ClassTreeIt,
          CallbackSpace TCallbackSpace = &UI::Callbacks::Race::SelectAllowedClass>
void UpdateGridElements(
   Template* Group,
   std::map<int, TType>& MappedGroup,
   std::string FourCharacterPrefix,
   bool ShouldFilter = false,
   int AllowedClassIDs[] = DummyAllowedClasses)
{
   const bool AllowAll = AllowedClassIDs[0] == INVALID_INDEX;
   ShouldFilter &= AllowAll == false;

   int TargetIdx = 0;
   bool FoundMatch = false;
   for (const std::pair<const int, TType>& ClassPair : MappedGroup)
   {
      // SLow filter, lucky we cap out
      if (ShouldFilter)
      {
         for (int Step = 0; Step < MAX_ALLOWED_CLASSES; Step++)
         {
            FoundMatch = AllowedClassIDs[Step] == ClassPair.first;
            if (FoundMatch) { break; }
         }

         // skip to next step uf we are filtering
         if (FoundMatch == false) { continue; }
         
      }

      RequestNewTemplateEntryForDataWithID<TType, TCallbackSpace>(Group, FourCharacterPrefix, ClassPair.second, TargetIdx);
      ++TargetIdx;
   }
   UIHandler::DrawTemplates(Group);
}


static GridElement DummyElement = {};
void WMainMenu::Draw(CE::Style::Compound& StaticWidgetStyling)
{
   StaticWidgetStyling.WrapStyle(
      [&]()
      {
         bool State;
         if (ImGui::BeginMainMenuBar())
         {
            const std::string CharacterEditorLabel = UIHandler::GetCharacterEditorStateLabel(State);
            if (ImGui::Button(CharacterEditorLabel.c_str()))
            {
               ButtonPayload Payload{DummyElement, nullptr, State == false};
               UIHandler::SetCharacterEditorOpen(Payload);
            }
      
            const std::string ClassEditorLabel = UIHandler::GetClassEditorStateLabel(State);
            if (ImGui::Button(ClassEditorLabel.c_str()))
            {
               ButtonPayload Payload{DummyElement, nullptr, State == false};
               UIHandler::SetClassEditorOpen(Payload);
            }
      
            const std::string RaceEditorLabel = UIHandler::GetRaceEditorStateLabel(State);
            if (ImGui::Button(RaceEditorLabel.c_str()))
            {
               ButtonPayload Payload{DummyElement, nullptr, State == false};
               UIHandler::SetRaceEditorOpen(Payload);
            }
      
            ImGui::EndMainMenuBar();
         }
   });        
}

void WClassEditor::Draw(CE::Style::Compound& StaticWidgetStyling)
{
   static char classbuf[Data::Format::NameSize];
   StaticWidgetStyling.WrapStyle(
      [&]()
      {
         ImGui::Begin("CEditor Class Creator", 0, ImGuiWindowFlags_NoCollapse);

         const char* Input = "Class Base\0";
         ImGui::Text(Input);

         const std::string NameToString = UIHandler::EditableClass.Name;
         const std::string ConcStr = "{Class Name}: " + NameToString;
         ImGui::InputText(ConcStr.c_str(), classbuf, Data::Format::NameSize, SharedInputTextFlags, &UIHandler::CallbackClassName);

         MakeTree("{Prime Stat Selector}", { UIHandler::DrawTemplates(&UI::Class::PrimeStatTemplates); })
         MakeTree("{Hit-die Selector}", { UIHandler::DrawTemplates(&UI::Class::PlatonicDiceTemplates); })

         //---- Class Menu buttons
         ImGui::NewLine();
         ImGui::NewLine();
         UIHandler::DrawTemplates(&UI::Menu::ClassMenuControlTemplates);

         ImGui::End();
      }); 
}

void WRaceEditor::Draw(CE::Style::Compound& StaticWidgetStyling)
{
   static char namebuf[Data::Format::NameSize];
   static char abilitybuf[Data::Format::NameSize];
   StaticWidgetStyling.WrapStyle(
      [&]()
      {
         ImGui::Begin("CEditor Race Creator", 0, ImGuiWindowFlags_NoCollapse);

         const char* Input = "Race Base";
         ImGui::Text(Input);

         const std::string NameToString = UIHandler::EditableRace.Name;
         const std::string ConcStr = "{Race Name}: " + NameToString;
         ImGui::InputText(ConcStr.c_str(), namebuf, Data::Format::NameSize, SharedInputTextFlags, &UIHandler::CallbackRaceName);

         MakeTree("{Minimum Stats}", { UIHandler::DrawTemplates(&UI::Race::MinimumStatsTemplates); })
         MakeTree("{Modifiers}", { UIHandler::DrawTemplates(&UI::Race::ModifierTemplates); })

         //
         // Class selector tree
         static const std::string ClassListPrefix = "cl::";
      #define RaceEditor_ClassSelectors CharacterClass, ClassTreeIt, UI::Callbacks::Race::SelectAllowedClass
         MakeTree("{Allowed Classes Selector}",
            {
               UpdateGridElements<RaceEditor_ClassSelectors>(&UI::Race::ClassTemplates, EditorCore::MappedClasses, ClassListPrefix);
            }
         )

         //
         // Misc
         ImGui::NewLine();
         const std::string AbilityString = UIHandler::EditableRace.AbilityName;
         const std::string AbilityConcStr = "{Race Ability}: " + AbilityString;
         ImGui::InputText(AbilityConcStr.c_str(), abilitybuf, Data::Format::NameSize);


         //---- Race Menu buttons
         ImGui::NewLine();
         ImGui::NewLine();
         UIHandler::DrawTemplates(&UI::Menu::RaceMenuControlTemplates);

         ImGui::End();
      });    
}

void WCharacterEditor::Draw(CE::Style::Compound& StaticWidgetStyling)
{
   StaticWidgetStyling.WrapStyle(
      [&]()
      {
         ImGui::Begin("CEditor Character Creator", 0, ImGuiWindowFlags_NoCollapse);
      
         const char* Input = "Character Base";
         ImGui::Text(Input);
      
         static char charbuf[Data::Format::NameSize];
         const std::string NameToString = UIHandler::EditableCharacter.Name;
         const std::string InputBoxLabel = "<{Name Entry Box} ";
         ImGui::InputText(InputBoxLabel.c_str(), charbuf, Data::Format::NameSize, SharedInputTextFlags, &UIHandler::CallbackCharacterName);
         const std::string FinalName = "Character Name}: " + NameToString;
         ImGui::Text(FinalName.c_str());
      
         // Create a Character Selector window
         WCharacterSelector::Draw(UIHandler::Stylings[CE::Style::StylingSelector::CharacterSelector]);
         
         //
         // Race selector tree 
         static const std::string RaceListPrefix = "rs::";
      #define CharEditor_RaceTypes CharacterRace, RaceTreeIt, UI::Callbacks::Character::SelectRace
         MakeTree("{Race Selector} : @todo Selected and/or Rolled Race",
            {
               UpdateGridElements<CharEditor_RaceTypes>(&UI::Character::RaceTemplates, EditorCore::MappedRaces, RaceListPrefix);
            }
         )
      
         if (UIHandler::EditableCharacter.Race.ID == INVALID_INDEX)
         {
            ImGui::NewLine();
            WText::Draw(GlobalSharedStylings::GenericTextCompound, "Select Race first to see possible classes");
         }
         else
         {
            //
            // Class selector tree / Filter based on selected race
            static const std::string ClassListPrefix = "cl::";
         #define CharEditor_ClassTypes CharacterClass, ClassTreeIt, UI::Callbacks::Character::SelectClass
            MakeTree("{Class Selector}",
               {
                  UpdateGridElements<CharEditor_ClassTypes>(&UI::Character::ClassTemplates, EditorCore::MappedClasses, ClassListPrefix, true, UIHandler::EditableCharacter.Race.AllowedClassIDs);
               }
            )
         }
         
         // Alignment selector tree 
         MakeTree("{Alignment Selector - @todo Selected or rolled}", { UIHandler::DrawTemplates(&UI::Character::AlignmentTemplates); })
         
         static LinkedStatCategory StatCategories{ std::vector<LinkedWidgetBase*>{}, &UI::Character::CharacterStatTemplates};
         static LinkedButton Button{{&StatCategories}}; // ::Paint();
      
         // @todo move out of lambda
         auto ButtonConstructs =
            [&]()
            {
               if (ImGui::Button("Re-roll Stats"))
               {
                  UIHandler::EditableCharacter.RerollStats(static_cast<int>(ImGui::GetTime() * 100.f));
               }
               ImGui::SameLine(0, 50.0);         
               Button(Button.AltState ? "Unlock Stats" : "Lock Stats", ImVec2(0, 0));
            };
         
         MakeTree("{Current Stats}",
            {
               ButtonConstructs();
               UIHandler::DrawTemplates(&UI::Character::CharacterStatTemplates);
            })
      
      
         // @todo Display calculated hitpoints, have a roll button that rolls our abilities and our hitpoint
         ImGui::NewLine();
         ImGui::Text("{Hitpoints}");
         ImGui::Text(": %04i", UIHandler::EditableCharacter.Hitpoints);
      
         // @todo Display calculated starter gold
         ImGui::NewLine();
         ImGui::Text("{Starting Gold}");
         ImGui::Text(": %04i", UIHandler::EditableCharacter.Gold);
      
         ImGui::NewLine();
         if (ImGui::Button("{Roll Character Die}"))
         {
            UIHandler::EditableCharacter.RollHPAndGold(10, static_cast<int>(ImGui::GetTime()));
         }
      
         //---- Character Menu buttons
         ImGui::NewLine();
         ImGui::NewLine();
         UIHandler::DrawTemplates(&UI::Menu::CharacterMenuControlTemplates);
      
         ImGui::End();
      });   
}

void WCharacterSelector::Draw(CE::Style::Compound& StaticWidgetStyling)
{
   StaticWidgetStyling.WrapStyle(
      [&]()
      {
         // Character selector tree 
         static const std::string CharacterListPrefix = "cx::";
      #define CharEditor_CharTypes DnDCharacter, CharTreeIt, UI::Callbacks::CharacterPicker::SelectCharacter
         MakeTree("{Character Selector}",
            {
               ImGui::BeginListBox("");
               UpdateGridElements<CharEditor_CharTypes>(&UI::Character::LoadedCharacterTemplates, EditorCore::MappedCharacters, CharacterListPrefix);
               ImGui::NewLine();
               ImGui::NewLine();
               ImGui::NewLine();

               //
               //---- Character Menu buttons
               ImGui::NewLine();
               UIHandler::DrawTemplates(&UI::Menu::CharacterSelectorControlTemplates);
               ImGui::EndListBox();
            }
         )         
      });   
}