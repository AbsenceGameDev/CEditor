#include "imgui.h"
#include "imgui_internal.h"

#include <vector>
#include <queue>

#include "../../inc/ui/shared.h"
#include "../../inc/ui/settings.h"
#include "../../inc/ui/styling.h"
#include "../../inc/ui/handler.h"
#include "../../inc/dataformat.h"
#include "../../inc/ui/widgets.h"
#include "../../inc/editor_core.h"


#define PDDEBUG

#define MakeTree(str_literal, scoped_payload) \
ImGui::NewLine(); \
if (ImGui::TreeNode(str_literal)) \
{\
\
   scoped_payload \
   ImGui::TreePop(); \
}

//
// Static initializations
ImFont* UIHandler::GlobalSmallFont = nullptr;
ImFont* UIHandler::GlobalLargeFont = nullptr;
bool UIHandler::DrawClassEditor = true;
bool UIHandler::DrawRaceEditor = true;
bool UIHandler::DrawCharacterEditor = true;
bool UIHandler::DrawCharacterSelector = true;
bool UIHandler::BlockEditors = true;
std::string UIHandler::LastEditedClassName = "\0";
std::string UIHandler::LastEditedRaceName = "\0";
std::string UIHandler::LastEditedCharacterName = "\0";

GenericName DummyCharName = "Default\0";
GridElement DummyElement;

bool DummyInitializer;
CharacterClass UIHandler::EditableClass = {DummyCharName, Spec::Character::StatType::Charisma, Platonic::Dice::E4D, DEFAULT_START_STATE};
CharacterRace UIHandler::EditableRace = CharacterRace::ConstructOnStack();
DnDCharacter UIHandler::EditableCharacter = DnDCharacter::ConstructOnStack(DummyCharName, DummyInitializer, UIHandler::EditableRace,
                                                                           UIHandler::EditableClass, DEFAULT_START_STATE);

void UIHandler::DeinitUI()
{
   BlockEditors = true;

   LastSelectedElementPerTemplate().clear();
   LastSelectedElementPerTemplate_BackMapping().clear();
   CurrentSelectedElementPerTemplate().clear();
   CurrentSelectedElementPerTemplate_BackMapping().clear();
}


void UIHandler::ApplyStyling(
   ImVec2 FramePadding= {20,20},
   ImVec2 ItemSpacing = {10,0},
   float TabBorderSize = 50.f,
   std::function<void(void)> Scoped = [](){})
{
   ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, FramePadding);
   ImGui::PushStyleVar(ImGuiStyleVar_TabBorderSize, TabBorderSize);
   ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ItemSpacing);


   Scoped();
   

   ImGui::PopStyleVar();
   ImGui::PopStyleVar();
   ImGui::PopStyleVar();
}

std::string UIHandler::GetCharacterEditorStateLabel(bool& State)
{
   static std::string IsOpened = "Close Character Editor";
   static std::string IsClosed = "Open Character Editor";

   static ButtonPayload Payload{DummyElement, nullptr, false};

   State = UIHandler::GetIsCharacterEditorOpen(Payload);
   return State ? IsOpened : IsClosed;
}

std::string UIHandler::GetClassEditorStateLabel(bool& State)
{
   static std::string IsOpened = "Close Class Editor";
   static std::string IsClosed = "Open Class Editor";

   static ButtonPayload Payload{DummyElement, nullptr, false};

   State = UIHandler::GetIsClassEditorOpen(Payload);
   return State ? IsOpened : IsClosed;
}

std::string UIHandler::GetRaceEditorStateLabel(bool& State)
{
   static std::string IsOpened = "Close Race Editor";
   static std::string IsClosed = "Open Race Editor";

   static ButtonPayload Payload{DummyElement, nullptr, false};

   State = UIHandler::GetIsRaceEditorOpen(Payload);
   return State ? IsOpened : IsClosed;
}

std::string UIHandler::GetCharacterSelectorStateLabel(bool& State)
{
   static std::string IsOpened = "Close Character Selector";
   static std::string IsClosed = "Open Character Selector";

   static ButtonPayload Payload{DummyElement, nullptr, false};

   State = UIHandler::GetIsCharacterSelectorOpen(Payload);
   return State ? IsOpened : IsClosed;
}

void UIHandler::DrawAppMainMenuBar()
{
   // @todo turn into some styling construct which handles pushing and popping full style overhauls for us? 
   ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20));
   ImGui::PushStyleVar(ImGuiStyleVar_TabBorderSize, 50.f);
   ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));

   // TODO
   // UIHandler::ApplyStyling(
   //    
   //    );
   

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

   ImGui::PopStyleVar();
   ImGui::PopStyleVar();
   ImGui::PopStyleVar();
}


static void TooltipMarker(const char* desc) // NO-LINT
{
   ImGui::TextDisabled("(?)");
   if (ImGui::BeginItemTooltip())
   {
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextUnformatted(desc);
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
   }
}

void UIHandler::PaintClassEditor(ImGuiInputTextFlags SharedInputTextFlags)
{
   if (DrawClassEditor == false ) { return;}
   
   static char classbuf[Data::Format::NameSize];

   ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(600, 450));
   ImGui::Begin("CEditor Class Creator", 0, ImGuiWindowFlags_NoCollapse);

   const char* Input = "Class Base\0";
   ImGui::Text(Input);

   const std::string NameToString = EditableClass.Name;
   const std::string ConcStr = "{Class Name}: " + NameToString;
   ImGui::InputText(ConcStr.c_str(), classbuf, Data::Format::NameSize, SharedInputTextFlags, &UIHandler::CallbackClassName);

   MakeTree("{Prime Stat Selector}", { DrawTemplates(&UI::Class::PrimeStatTemplates); })
   MakeTree("{Hit-die Selector}", { DrawTemplates(&UI::Class::PlatonicDiceTemplates); })

   //---- Class Menu buttons
   ImGui::NewLine();
   ImGui::NewLine();
   DrawTemplates(&UI::Menu::ClassMenuControlTemplates);

   ImGui::End();
   ImGui::PopStyleVar();
}

void UIHandler::PaintRaceEditor(ImGuiInputTextFlags SharedInputTextFlags)
{
   if (DrawRaceEditor == false) { return;}

   static char namebuf[Data::Format::NameSize];
   static char abilitybuf[Data::Format::NameSize];

   ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(500, 600));
   ImGui::Begin("CEditor Race Creator", 0, ImGuiWindowFlags_NoCollapse);

   const char* Input = "Race Base";
   ImGui::Text(Input);

   const std::string NameToString = EditableRace.Name;
   const std::string ConcStr = "{Race Name}: " + NameToString;
   ImGui::InputText(ConcStr.c_str(), namebuf, Data::Format::NameSize, SharedInputTextFlags, &UIHandler::CallbackRaceName);

   MakeTree("{Minimum Stats}", { DrawTemplates(&UI::Race::MinimumStatsTemplates); })
   MakeTree("{Modifiers}", { DrawTemplates(&UI::Race::ModifierTemplates); })

   //
   // Class selector tree
   static const std::string ClassListPrefix = "cl::";
#define RaceEditor_ClassSelectors CharacterClass, ClassTreeIt, UI::Callbacks::Race::SelectAllowedClass
   MakeTree("{Allowed Classes Selector}",
      {
         // Prepass
         std::deque<const GridElement*> RemovalDeque;
         std::set<int> Matcher;
         UpdateGridElements<RaceEditor_ClassSelectors>(&UI::Race::ClassTemplates, EditorCore::MappedClasses, ClassListPrefix, RemovalDeque);
      }
   )

   //
   // Misc
   ImGui::NewLine();
   const std::string AbilityString = EditableRace.AbilityName;
   const std::string AbilityConcStr = "{Race Ability}: " + AbilityString;
   ImGui::InputText(AbilityConcStr.c_str(), abilitybuf, Data::Format::NameSize);


   //---- Race Menu buttons
   ImGui::NewLine();
   ImGui::NewLine();
   DrawTemplates(&UI::Menu::RaceMenuControlTemplates);

   ImGui::End();
   ImGui::PopStyleVar();
}

void UIHandler::PaintCharacterEditor(ImGuiInputTextFlags SharedInputTextFlags)
{
   if (DrawCharacterEditor == false) { return; }
   
   // @todo turn into some styling construct which handles pushing and popping full style overhauls for us? 
   ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(600, 500));
   ImGui::Begin("CEditor Character Creator", 0, ImGuiWindowFlags_NoCollapse);

   const char* Input = "Character Base";
   ImGui::Text(Input);

   static char charbuf[Data::Format::NameSize];
   const std::string NameToString = EditableCharacter.Name;
   const std::string InputBoxLabel = "<{Name Entry Box} ";
   ImGui::InputText(InputBoxLabel.c_str(), charbuf, Data::Format::NameSize, SharedInputTextFlags, &UIHandler::CallbackCharacterName);
   const std::string FinalName = "Character Name}: " + NameToString;
   ImGui::Text(FinalName.c_str());

   // Create a Character Selector window
   PaintCharacterSelector();
   
   //
   // Race selector tree 
   static const std::string RaceListPrefix = "rs::";
#define CharEditor_RaceTypes CharacterRace, RaceTreeIt, UI::Callbacks::Character::SelectRace
   MakeTree("{Race Selector} : @todo Selected and/or Rolled Race",
      {
         std::deque<const GridElement*> RemovalDeque; // for Prepass @todo unneeded, remove
         UpdateGridElements<CharEditor_RaceTypes>(&UI::Character::RaceTemplates, EditorCore::MappedRaces, RaceListPrefix, RemovalDeque);
      }
   )

   if (UIHandler::EditableCharacter.Race.ID == INVALID_INDEX)
   {
      // @todo turn into some styling construct which handles pushing and popping full style overhauls for us? 
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20));
      ImGui::PushStyleVar(ImGuiStyleVar_TabBorderSize, 50.f);
      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));      
      ImGui::NewLine();
      ImGui::Text("Select Race first to see possible classes");
      ImGui::NewLine();
      // @todo turn into some styling construct which handles pushing and popping full style overhauls for us? 
      ImGui::PopStyleVar();
      ImGui::PopStyleVar();
      ImGui::PopStyleVar();
   }
   else
   {
      
      // Class selector tree / Filter based on selected race
      static const std::string ClassListPrefix = "cl::";
   #define CharEditor_ClassTypes CharacterClass, ClassTreeIt, UI::Callbacks::Character::SelectClass
      MakeTree("{Class Selector}",
         {
            std::deque<const GridElement*> RemovalDeque; // for Prepass @todo unneeded: Remove the Removal deque (You are your worst enemy :O)
            UpdateGridElements<CharEditor_ClassTypes>(&UI::Character::ClassTemplates, EditorCore::MappedClasses, ClassListPrefix, RemovalDeque, true, UIHandler::EditableCharacter.Race.AllowedClassIDs);
         }
      )
   }

   
   
   // Alignment selector tree 
   MakeTree("{Alignment Selector - @todo Selected or rolled}", { DrawTemplates(&UI::Character::AlignmentTemplates); })
   
   static LinkedStatCategory StatCategories{ std::vector<LinkedWidgetBase*>{}, &UI::Character::CharacterStatTemplates};
   static LinkedButton Button{{&StatCategories}}; // ::Paint();

   // @todo move out of lambda
   auto ButtonConstructs =
      [&]()
      {
         if (ImGui::Button("Re-roll Stats"))
         {
            // todo
            UIHandler::EditableCharacter.RerollStats(static_cast<int>(ImGui::GetTime() * 100.f));
         }
         ImGui::SameLine(0, 50.0);         
         Button(Button.AltState ? "Unlock Stats" : "Lock Stats", ImVec2(0, 0));
      };
   
   MakeTree("{Current Stats}",
      {
         
         ButtonConstructs();
         DrawTemplates(&UI::Character::CharacterStatTemplates);
      })


   // @todo Display calculated hitpoints, have a roll button that rolls our abilities and our hitpoint
   ImGui::NewLine();
   ImGui::Text("{Hitpoints}");
   ImGui::Text(": %04i", EditableCharacter.Hitpoints);

   // @todo Display calculated starter gold
   ImGui::NewLine();
   ImGui::Text("{Starting Gold}");
   ImGui::Text(": %04i", EditableCharacter.Gold);

   ImGui::NewLine();
   if (ImGui::Button("{Roll Character Die}"))
   {
      EditableCharacter.RollHPAndGold(10, static_cast<int>(ImGui::GetTime()));
   }

   //---- Character Menu buttons
   ImGui::NewLine();
   ImGui::NewLine();
   DrawTemplates(&UI::Menu::CharacterMenuControlTemplates);

   ImGui::End();
   ImGui::PopStyleVar();
}

void UIHandler::PaintCharacterSelector()
{
   if (DrawCharacterSelector == false) { return;}
   
   // Character selector tree 
   
   static const std::string CharacterListPrefix = "cx::";
#define CharEditor_CharTypes DnDCharacter, CharTreeIt, UI::Callbacks::CharacterPicker::SelectCharacter
   MakeTree("{Character Selector}",
      {
         ImGui::BeginListBox("");
         // Prepass
         std::deque<const GridElement*> RemovalDeque;
         UpdateGridElements<CharEditor_CharTypes>(&UI::Character::LoadedCharacterTemplates, EditorCore::MappedCharacters, CharacterListPrefix, RemovalDeque);
         ImGui::NewLine();
         ImGui::NewLine();
         ImGui::NewLine();

         //
         //---- Character Menu buttons
         ImGui::NewLine();
         DrawTemplates(&UI::Menu::CharacterSelectorControlTemplates);
         ImGui::EndListBox();
      }
   )
}

// Editors OnPaint 
void UIHandler::OnPaint(ImGuiIO& io, ImVec4 ClearColor)
{
#ifdef PDDEBUG
   ImGui::ShowIDStackToolWindow(); // @imgui debug tools
#endif // PDDEBUG

   // Main menu bar
   UIHandler::DrawAppMainMenuBar();

   if (BlockEditors) { return; }

   ImGuiInputTextFlags SharedInputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll |
      ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
   ImGui::PushFont(GlobalSmallFont);

   // Create a Class editor window
   PaintClassEditor(SharedInputTextFlags);

   // Create a Race editor window
   PaintRaceEditor(SharedInputTextFlags);

   // Create a Character editor window
   PaintCharacterEditor(SharedInputTextFlags);

   // Moved into Character Editor
   // // Create a Character Selector window
   // PaintCharacterSelector();


   ImGui::PopFont();
}

int UIHandler::CallbackCharacterName(ImGuiInputTextCallbackData* data)
{
   if (data->BufSize <= 0) { return 1; }

   strcpy_s(UIHandler::EditableCharacter.Name, MinV(Data::Format::NameSize, data->BufSize), data->Buf);
   return 0;
}

int UIHandler::CallbackClassName(ImGuiInputTextCallbackData* data)
{
   if (data->BufSize <= 0) { return 1; }

   strcpy_s(UIHandler::EditableClass.Name, MinV(Data::Format::NameSize, data->BufSize), data->Buf);
   return 0;
}

int UIHandler::CallbackRaceName(ImGuiInputTextCallbackData* data)
{
   if (data->BufSize <= 0) { return 1; }

   strcpy_s(UIHandler::EditableRace.Name, MinV(Data::Format::NameSize, data->BufSize), data->Buf);
   return 0;
}

void UIHandler::SaveClass(ButtonPayload& Payload)
{
   if (EditableClass.Name != LastEditedClassName)
   {
      EditableClass.NewID();
      EditorCore::CreateClass(UIHandler::EditableClass);

      LastEditedClassName = EditableClass.Name;
   }
}

void UIHandler::SaveRace(ButtonPayload& Payload)
{
   if (EditableRace.Name != LastEditedRaceName)
   {
      EditableRace.NewID();
      EditorCore::CreateRace(UIHandler::EditableRace);

      LastEditedRaceName = EditableRace.Name;
   }
}

void UIHandler::SaveCharacter(ButtonPayload& Payload)
{
   if (EditableCharacter.Name != LastEditedCharacterName)
   {
      EditorCore::CreateCharacter(UIHandler::EditableCharacter);
      EditableCharacter.NewID();

      LastEditedCharacterName = EditableCharacter.Name;
   }
}

void UIHandler::LoadCharacter(ButtonPayload& Payload)
{
   DnDCharacter* TrueTarget = static_cast<DnDCharacter*>(Payload.Target);
   const DnDCharacter& StowedPicker = UI::Callbacks::CharacterPicker::StowedCharacterForPicker();
   if (StowedPicker.ID == INVALID_INDEX)
   {
      return;
   }

   UIHandler::EditableCharacter = StowedPicker.ID != INVALID_INDEX ? StowedPicker : *TrueTarget;
   EditorCore::CreateCharacter(UIHandler::EditableCharacter, false); // if we have stale data in the core
}

//
// Grid element viewer
template <class TType = CharacterClass, class IteratorType = ClassTreeIt,
          CallbackSpace TCallbackSpace = &UI::Callbacks::Race::SelectAllowedClass>
void UIHandler::UpdateGridElements(
   Template* Group,
   std::map<int, TType>& MappedGroup,
   std::string FourCharacterPrefix,
   std::deque<const GridElement*> RemovalDeque,
   bool ShouldFilter,
   int AllowedClassIDs[MAX_ALLOWED_CLASSES])
{
   const bool AllowAll = AllowedClassIDs[0] == INVALID_INDEX;
   ShouldFilter &= AllowAll == false;

   int TargetIdx = 0;
   for (const std::pair<const int, TType>& ClassPair : MappedGroup)
   {
      // SLow filter, lucky we cap out
      if (ShouldFilter)
      {
         bool FoundMatch = false;
         for (int Step = 0; Step < MAX_ALLOWED_CLASSES; Step++)
         {
            FoundMatch = AllowedClassIDs[Step] == ClassPair.first;
            if (FoundMatch) { break; }
         }

         if (FoundMatch == false) { continue; }
         
      }

      UIHandler::RequestNewTemplateEntryForDataWithID<TType, TCallbackSpace>(Group, FourCharacterPrefix, ClassPair.second, TargetIdx);
      ++TargetIdx;
   }
   DrawTemplates(Group);
}


template <class TType = CharacterClass, CallbackSpace TCallbackSpace = &UI::Callbacks::Race::SelectAllowedClass>
void UIHandler::RequestNewTemplateEntryForDataWithID(Template* TGroup, std::string FourCharacterPrefix, const TType& ClassData, int Override)
{
   const std::string NameAsString = FourCharacterPrefix + ClassData.Name;
   const GridElement PotentiallyNewElement = GridElement(NameAsString, UI::CharacterEditorStyle, ClassData.ID, Override, false, true,
                                                         false, DummyStateDelegate, TCallbackSpace, DummyCallbackTarget);

   const bool DidElementExist = TGroup->DoesGridElementExist(PotentiallyNewElement);
   if (DidElementExist == false)
   {
      TGroup->Emplace(PotentiallyNewElement);
   }
}

void UIHandler::RemoveGridElement(const GridElement* Consider, Template* TGroup)
{
   Template::TemplateIt Inner = TGroup->Inner.find(*Consider);
   const bool IsValidElement = Inner != TGroup->Inner.end();

   bool ElementFound = false;

   if (
      IsValidElement
      && UIHandler::LastSelectedElementPerTemplate().empty() == false
      && UIHandler::CurrentSelectedElementPerTemplate().empty() == false)
   {
      TemplateElementKeyIt LastSelectionTemplateKeyIt = UIHandler::LastSelectedElementPerTemplate().find(TGroup);
      const bool FoundLastTemplate = LastSelectionTemplateKeyIt != UIHandler::LastSelectedElementPerTemplate().end();
      if (FoundLastTemplate)
      {
         TemplateElementValueIt LastElement = UIHandler::LastSelectedElementPerTemplate_BackMapping().find(
            LastSelectionTemplateKeyIt->second);
         const bool FoundLastElement = LastElement != UIHandler::LastSelectedElementPerTemplate_BackMapping().end();
         if (FoundLastElement)
         {
            LastSelectedElementPerTemplate_BackMapping().erase(LastElement);
            ElementFound = true;
         }
         LastSelectedElementPerTemplate().erase(LastSelectionTemplateKeyIt);
      }

      TemplateElementKeyIt CurrentSelectionTemplateKeyIt = UIHandler::CurrentSelectedElementPerTemplate().find(TGroup);
      const bool CurrentLastTemplate = CurrentSelectionTemplateKeyIt != UIHandler::CurrentSelectedElementPerTemplate().end();
      if (CurrentLastTemplate)
      {
         TemplateElementValueIt CurrentElement = UIHandler::CurrentSelectedElementPerTemplate_BackMapping().find(
            CurrentSelectionTemplateKeyIt->second);
         const bool FoundCurrentElement = CurrentElement != UIHandler::CurrentSelectedElementPerTemplate_BackMapping().end();
         if (FoundCurrentElement)
         {
            CurrentSelectedElementPerTemplate_BackMapping().erase(CurrentElement);
            ElementFound = true;
         }
         CurrentSelectedElementPerTemplate().erase(CurrentSelectionTemplateKeyIt);
      }

      if (ElementFound) { TGroup->Remove(*Consider); }
   }
}

void UIHandler::DrawTemplates(Template* Category)
{
   const ImVec2& CursorScreenPos = ImGui::GetCursorScreenPos();
   const ImVec2& Origin = ImGui::GetCursorPos();

   ImVec2 CalcPos = {0, 0};

   int StepID = 0;
   for (const GridElement& ImmutableElement : Category->Inner)
   {
      GridElement& MutableElement = const_cast<GridElement&>(ImmutableElement);
      const ElementStyle& Style = MutableElement.Style;

      ImGui::PushID(static_cast<int>(StepID++));
      const float HalfPaddingX = Style.Padding.x / 2;
      const float HalfPaddingY = Style.Padding.y / 2;

      const float SizeX = Style.ElementSizes.x;
      const float SizeY = Style.ElementSizes.y;


      //----
      ImGuiWindow* Window = ImGui::GetCurrentWindow();
      const ImGuiID id = Window->GetID(Category);

      bool OutHovered = false;
      bool OutHeld = false;
      ImGui::ButtonBehavior(ImRect(CalcPos.x, CalcPos.y, CalcPos.x + SizeX, CalcPos.y + SizeY), id, &OutHovered, &OutHeld,
                            ImGuiButtonFlags_PressedOnClick);
      ImVec2 MousePos = ImGui::GetMousePos();
      MousePos.x -= CursorScreenPos.x;
      MousePos.y -= CursorScreenPos.y;

      
      const bool AllowSelectionWasOverridden = Override::Behaviour::IsOverriddenBehaviour<Override::Behaviour::Category::AllowSelection>(Category->SharedSettingsFlags);
      const bool AllowSelectionOverrideState = Override::Behaviour::GetOverriddenState<Override::Behaviour::Category::AllowSelection>(Category->SharedSettingsFlags);
      const bool AllowSelection = AllowSelectionWasOverridden ? AllowSelectionOverrideState : MutableElement.AllowSelection;
      
      //
      // const bool AllowNegativeValuesWasOverridden = Override::Behaviour::IsOverriddenBehaviour<Override::Behaviour::Category::AllowNegativeValues>(Category->SharedSettings);
      // const bool AllowNegativeValuesOverrideState = Override::Behaviour::GetOverriddenState<Override::Behaviour::Category::AllowNegativeValues>(Category->SharedSettings);
      // bool AllowNegativeValues = AllowSelectionWasOverridden ? AllowSelectionOverrideState : MutableElement.AllowNegativeValues;      

      const bool DisplayValueWasOverridden = Override::Behaviour::IsOverriddenBehaviour<Override::Behaviour::Category::DisplayValue>(Category->SharedSettingsFlags);
      const bool DisplayValueOverrideState = Override::Behaviour::GetOverriddenState<Override::Behaviour::Category::DisplayValue>(Category->SharedSettingsFlags);
      const bool DisplayValue = DisplayValueWasOverridden ? DisplayValueOverrideState : MutableElement.DisplayValue;            

      if (AllowSelection)
      {
         OutHovered = (MousePos.x > CalcPos.x && MousePos.x < CalcPos.x + SizeX && MousePos.y > CalcPos.y && MousePos.y < CalcPos.y +
            SizeY);
         //---- Visualize Selection
         if (OutHovered || MutableElement.IsSelected)
         {
            const float OffsetX = CursorScreenPos.x + HalfPaddingX;
            const float OffsetY = CursorScreenPos.y + HalfPaddingY;

            const ImU32 Column = ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered]);
            CalcPos.x += OffsetX;
            CalcPos.y += OffsetY;
            ImGui::RenderFrame(ImVec2(CalcPos.x, CalcPos.y), ImVec2(CalcPos.x + SizeX, CalcPos.y + SizeY), Column, true, 3.f);
            CalcPos.x -= OffsetX;
            CalcPos.y -= OffsetY;
         }
      }

      ImGui::SetCursorPosX(CalcPos.x + Origin.x + HalfPaddingX);
      ImGui::SetCursorPosY(CalcPos.y + Origin.y + HalfPaddingY);
      // CreateTexture(Element);
      // ImGui::Image(Element.ImageID, ImVec2(50, 50));


      std::string Name = MutableElement.Label;
      Name = Name.substr(4, Name.length() - 4);

      if (AllowSelection)
      {
         const bool DidPressButton = ImGui::Button(Name.c_str(), Style.ElementSizes);
         if (DidPressButton)
         {
            ProcessGridElement(Category, MutableElement);
         }

         // Check if mouse button is still active and if this is an element with a visible value, ignore those with INVALID_INDEX, they are not meant to have values
         if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && DisplayValue)
         {
            const float YAcc = -ImGui::GetIO().MouseDelta.y / 2;
            MutableElement.CurrentValue = ClampData(MutableElement.CurrentValue + static_cast<int>(YAcc),
                                                    MutableElement.AllowNegativeValues ? -20 : 0, 20);
         }
      }
      else
      {
         const float OffsetX = CursorScreenPos.x + HalfPaddingX;
         const float OffsetY = CursorScreenPos.y + HalfPaddingY;

         const ImU32 Column = ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]);
         CalcPos.x += OffsetX;
         CalcPos.y += OffsetY;
         ImGui::RenderFrame(ImVec2(CalcPos.x, CalcPos.y), ImVec2(CalcPos.x + SizeX, CalcPos.y + SizeY), Column, true, 3.f);

         ImGui::PushFont(GlobalLargeFont);
         ImGui::RenderText(ImVec2(CalcPos.x + HalfPaddingX * 0.5f, CalcPos.y + HalfPaddingY * 0.5f), Name.c_str(),
                           Name.c_str() + Name.size());
         ImGui::PopFont();
         CalcPos.x -= OffsetX;
         CalcPos.y -= OffsetY;
      }

      ButtonPayload Payload = {MutableElement};
      MutableElement.OnRefresh(Payload);

      
      //---- Text
      ImGui::SetCursorPosX(CalcPos.x + Origin.x + HalfPaddingX);
      ImGui::SetCursorPosY(CalcPos.y + Origin.y + SizeY);
      ImGui::PushFont(GlobalSmallFont);

      if (DisplayValue)
      {
         ImGui::Text("%02d", MutableElement.CurrentValue);
      }

      ImGui::PopFont();

      //---- Next position
      CalcPos.x += SizeX + HalfPaddingX * 2;

      const float WindowWidth = ImGui::GetWindowWidth();
      const float MaxWidth = Category->MaxSize.x > 0 ? MinV(Category->MaxSize.x, WindowWidth) : WindowWidth;
      if (CalcPos.x + SizeX > MaxWidth)
      {
         CalcPos.x = 0;
         CalcPos.y += SizeY + HalfPaddingY * 2;
      }

      ImGui::PopID();
   }
}


void UIHandler::ProcessGridElement(Template* Category, GridElement& Element)
{
   TemplateElementKeyIt CurrentSelectionTemplateKeyIt = UIHandler::CurrentSelectedElementPerTemplate().find(Category);
   const bool FoundCurrentTemplate = CurrentSelectionTemplateKeyIt != UIHandler::CurrentSelectedElementPerTemplate().end() &&
      CurrentSelectionTemplateKeyIt->first != nullptr;

   TemplateElementKeyIt LastSelectionTemplateKeyIt = UIHandler::LastSelectedElementPerTemplate().find(Category);
   const bool FoundLastTemplate = LastSelectionTemplateKeyIt != UIHandler::LastSelectedElementPerTemplate().end() &&
      LastSelectionTemplateKeyIt->first != nullptr;

   {
      GridElement* SelectedGridElement = FoundCurrentTemplate ? UIHandler::CurrentSelectedElementPerTemplate()[Category] : nullptr;
      TemplateElementValueIt OverwriteElementIt = UIHandler::LastSelectedElementPerTemplate_BackMapping().find(SelectedGridElement);
      if (FoundLastTemplate == false)
      {
         UIHandler::LastSelectedElementPerTemplate().emplace(Category, SelectedGridElement);
         UIHandler::LastSelectedElementPerTemplate_BackMapping().emplace(SelectedGridElement, Category);
      }
      else
      {
         LastSelectionTemplateKeyIt->second = SelectedGridElement;
         if (OverwriteElementIt != LastSelectedElementPerTemplate_BackMapping().end())
         {
            LastSelectedElementPerTemplate_BackMapping()[SelectedGridElement] = Category;
         }
         else
         {
            LastSelectedElementPerTemplate_BackMapping().emplace(SelectedGridElement, Category);
         }
      }
   }

   TemplateElementValueIt OverwriteElementIt = UIHandler::CurrentSelectedElementPerTemplate_BackMapping().find(&Element);
   if (FoundCurrentTemplate == false)
   {
      UIHandler::CurrentSelectedElementPerTemplate().emplace(Category, &Element);
      UIHandler::CurrentSelectedElementPerTemplate_BackMapping().emplace(&Element, Category);
   }
   else
   {
      CurrentSelectionTemplateKeyIt->second = &Element;
      if (OverwriteElementIt != CurrentSelectedElementPerTemplate_BackMapping().end())
      {
         CurrentSelectedElementPerTemplate_BackMapping()[&Element] = Category;
      }
      else
      {
         CurrentSelectedElementPerTemplate_BackMapping().emplace(&Element, Category);
      }
   }

   // Always call after making sure we've progressed our states

   ButtonPayload InnerPayload{Element};
   ButtonPayload Payload{Element, nullptr, Element.GetStateDelegate(InnerPayload) == false};
   Element.CallbackTarget(Payload); //Flip state
}


//
// Utility

ImFont* UIHandler::LoadFonts(ImGuiIO& io)
{
   io.Fonts->AddFontDefault();
   io.Fonts->AddFontFromFileTTF("../../rsc/Fonts/Besley/static/Besley-BlackItalic.ttf", 15.0f);
   ImFont* SmallFont = io.Fonts->AddFontFromFileTTF("../../rsc/Fonts/Besley/static/Besley-Black.ttf", 18.0f, nullptr,
                                                    io.Fonts->GetGlyphRangesDefault());
   ImFont* LargeFont = io.Fonts->AddFontFromFileTTF("../../rsc/Fonts/Besley/static/Besley-Black.ttf", 24.0f, nullptr,
                                                    io.Fonts->GetGlyphRangesDefault());
   IM_ASSERT(SmallFont != nullptr);
   IM_ASSERT(LargeFont != nullptr);

   GlobalSmallFont = SmallFont;
   GlobalLargeFont = LargeFont;

   return SmallFont;
}
