#include "imgui.h"
#include "imgui_internal.h"

#include <vector>

#include "../inc/dataformat.h"
#include "../inc/ui.h"

#include <iostream>

#include "../inc/editor_core.h"


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
bool UIHandler::BlockEditors = true;


std::string UIHandler::LastEditedClassName = "\0";
std::string UIHandler::LastEditedRaceName = "\0";
std::string UIHandler::LastEditedCharacterName = "\0";

std::map<Template*, GridElement*> UIHandler::LastSelectedElementPerTemplate = {};
std::map<GridElement*, Template*> UIHandler::LastSelectedElementPerTemplate_BackMapping = {};
std::map<Template*, GridElement*> UIHandler::CurrentSelectedElementPerTemplate = {};
std::map<GridElement*, Template*> UIHandler::CurrentSelectedElementPerTemplate_BackMapping = {};

GenericName DummyCharName = "Default\0";

GridElement DummyElement;

bool DummyInitializer;
CharacterClass UIHandler::EditableClass = {DummyCharName, Spec::Character::StatType::Charisma, Platonic::Dice::E4D, DEFAULT_START_STATE };
CharacterRace UIHandler::EditableRace = CharacterRace::ConstructOnStack();
DnDCharacter UIHandler::EditableCharacter = DnDCharacter::ConstructOnStack(DummyCharName,DummyInitializer, UIHandler::EditableRace,UIHandler::EditableClass, DEFAULT_START_STATE);

void UIHandler::DrawAppMainMenuBar()
{
   // @todo turn into some styling construct which handles pushing and popping full style overhauls for us? 
   ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20));
   ImGui::PushStyleVar(ImGuiStyleVar_TabBorderSize, 50.f); 
   ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
   
   if (ImGui::BeginMainMenuBar())
   {
      if (ImGui::Button("Open Character Editor"))
      {
         UIHandler::SetCharacterEditorOpen(DummyElement, true);
      }

      if (ImGui::Button("Open Class Editor"))
      {
         UIHandler::SetClassEditorOpen(DummyElement, true);
      }

      if (ImGui::Button("Open Race Editor"))
      {
         UIHandler::SetRaceEditorOpen(DummyElement, true);
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

// Editors OnPaint 
void UIHandler::OnPaint(ImGuiIO& io, ImVec4 ClearColor)
{
#ifdef PDDEBUG
   ImGui::ShowIDStackToolWindow(); // @imgui debug tools
#endif // PDDEBUG
   
   // Main menu bar
   UIHandler::DrawAppMainMenuBar();

   if (BlockEditors) { return; }
   
   ImGuiInputTextFlags SharedInputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
   ImGui::PushFont(GlobalSmallFont);
   
   // Create a Class editor window
   if (DrawClassEditor)
   {
      static char classbuf[Data::Format::NameSize];
      
      ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(600, 450));
      ImGui::Begin("CEditor Class Creator", 0, ImGuiWindowFlags_NoCollapse);

      const char* Input = "Class Base\0";
      ImGui::Text(Input);

      const std::string NameToString = EditableClass.Name;
      const std::string ConcStr = "{Class Name}: " + NameToString;      
      ImGui::InputText(ConcStr.c_str(), classbuf, Data::Format::NameSize, SharedInputTextFlags, &UIHandler::CallbackClassName);

      MakeTree("{Prime Stat Selector}", { DrawTemplates(&UI::Class::PrimeStatTemplates); } )
      MakeTree("{Hit-die Selector}", { DrawTemplates(&UI::Class::PlatonicDiceTemplates); } )

      //---- Class Menu buttons
      ImGui::NewLine();
      ImGui::NewLine();
      DrawTemplates(&UI::Menu::ClassMenuControlTemplates);

      ImGui::End();
      ImGui::PopStyleVar();
   }


   // Create a Race editor window
   if (DrawRaceEditor)
   {
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
      Template ClassTemplates{ {} };
      MakeTree("{Allowed Classes Selector}",
         {
            for (const std::pair<const int, CharacterClass>& ClassPair : EditorCore::MappedClasses)
            {
               const std::string NameAsString = ClassListPrefix + ClassPair.second.Name;

               ClassTemplates.Inner.emplace_back(NameAsString, UI::CharacterEditorStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectAllowedClass);
            }
            DrawTemplates(&ClassTemplates);
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

   // Create a Character editor window
   if (DrawCharacterEditor)
   {
      // @todo turn into some styling construct which handles pushing and popping full style overhauls for us? 
      ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(600, 500));
      ImGui::Begin("CEditor Character Creator", 0, ImGuiWindowFlags_NoCollapse);
      
      const char* Input = "Character Base";
      ImGui::Text(Input);
      
      static char charbuf[Data::Format::NameSize];
      const std::string NameToString = EditableCharacter.Name;
      const std::string ConcStr = "{Character Name}: " + NameToString;
      ImGui::InputText(ConcStr.c_str(), charbuf, Data::Format::NameSize, SharedInputTextFlags, &UIHandler::CallbackCharacterName);
      
      // Class selector tree @todo Finish class selection
      static const std::string ClassListPrefix = "cl::";
      Template ClassTemplates{{} };
      MakeTree("{Class Selector}", 
         {
            for (const std::pair<const int, CharacterClass>& ClassPair : EditorCore::MappedClasses)
            {
               const std::string NameAsString = ClassListPrefix + ClassPair.second.Name;
               ClassTemplates.Inner.emplace_back(NameAsString, UI::CharacterEditorStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, DummyStateDelegate, DummyCallbackTarget);
            }
            DrawTemplates(&ClassTemplates);
         }
      )
      
      //
      // Race selector tree @todo Finish race selection
      static const std::string RaceListPrefix = "rs::";
      Template RaceTemplates{ {}};
      MakeTree("{Race Selector} : @todo Selected and/or Rolled Race",
         {
            for (const std::pair<const int, CharacterRace>& RacePair : EditorCore::MappedRaces)
            {
               const std::string NameAsString = RaceListPrefix + RacePair.second.Name;
               RaceTemplates.Inner.emplace_back(NameAsString, UI::CharacterEditorStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, DummyStateDelegate, DummyCallbackTarget);
            }
            DrawTemplates(&RaceTemplates);
         }
      )
      
      // Alignment selector tree @todo Finish Alignment selection, make sure we can't slect from teh different aligment templates, or put them all in one template and wrap by 3 element
      MakeTree("{Alignment Selector - @todo Selected or rolled}",
         {
            DrawTemplates(&UI::Character::AlignmentTemplates);
         }
      )


      MakeTree("{Current Stats}", { DrawTemplates(&UI::Character::CharacterStatTemplates); })

      
      
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
         EditableCharacter.RollHPAndGold(10, EditableCharacter.CachedSeed);
      }
      
      //---- Character Menu buttons
      ImGui::NewLine();
      ImGui::NewLine();
      DrawTemplates(&UI::Menu::CharacterMenuControlTemplates);
      
      ImGui::End();
      ImGui::PopStyleVar();
   }

   ImGui::PopFont();
}

int UIHandler::CallbackCharacterName(ImGuiInputTextCallbackData* data)
{
   if (data->BufSize <= 0) { return 1;}
   
   strcpy_s(UIHandler::EditableCharacter.Name, MinV(Data::Format::NameSize, data->BufSize) , data->Buf);
   return 0;
}

int UIHandler::CallbackClassName(ImGuiInputTextCallbackData* data)
{
   if (data->BufSize <= 0) { return 1;}
   
   strcpy_s(UIHandler::EditableClass.Name, MinV(Data::Format::NameSize, data->BufSize) , data->Buf);
   return 0;
}

int UIHandler::CallbackRaceName(ImGuiInputTextCallbackData* data)
{
   if (data->BufSize <= 0) { return 1;}
   
   strcpy_s(UIHandler::EditableRace.Name, MinV(Data::Format::NameSize, data->BufSize) , data->Buf);
   return 0;
}

void UIHandler::SaveClass(GridElement& CallingViualElement, bool SetOpen)
{
   if (EditableClass.Name != LastEditedClassName)
   {
      EditorCore::CreateClass(UIHandler::EditableClass);
      EditableClass.NewID();
      
      LastEditedClassName = EditableClass.Name;
   }
}

void UIHandler::SaveRace(GridElement& CallingViualElement,bool SetOpen)
{
   if (EditableRace.Name != LastEditedRaceName)
   {
      EditorCore::CreateRace(UIHandler::EditableRace);
      EditableRace.NewID();
      
      LastEditedRaceName = EditableRace.Name;
   }   
}

void UIHandler::SaveCharacter(GridElement& CallingViualElement,bool SetOpen)
{
   if (EditableCharacter.Name != LastEditedCharacterName)
   {
      EditorCore::CreateCharacter(UIHandler::EditableCharacter);
      EditableCharacter.NewID();

      LastEditedCharacterName = EditableCharacter.Name;
   }   
}

//
// Grid element viewer
void UIHandler::DrawTemplates(Template* Category)
{
   const ImVec2& CursorScreenPos = ImGui::GetCursorScreenPos();
   const ImVec2& Origin = ImGui::GetCursorPos();

   ImVec2 CalcPos = {0,0};

   for (size_t i = 0; i < Category->Inner.size(); i++)
   {
      GridElement& Element = Category->Inner[i];
      const ElementStyle& Style = Element.Style;

      ImGui::PushID(static_cast<int>(i));
      const float HalfPaddingX = Style.Padding.x / 2;
      const float HalfPaddingY = Style.Padding.y / 2;

      const float SizeX = Style.ElementSizes.x;
      const float SizeY = Style.ElementSizes.y;


      //----
      ImGuiWindow* Window = ImGui::GetCurrentWindow();
      const ImGuiID id = Window->GetID(Category);

      bool OutHovered = false;
      bool OutHeld = false;
      ImGui::ButtonBehavior(ImRect(CalcPos.x, CalcPos.y, CalcPos.x + SizeX, CalcPos.y + SizeY), id, &OutHovered, &OutHeld, ImGuiButtonFlags_PressedOnClick);
      ImVec2 MousePos = ImGui::GetMousePos();
      MousePos.x -= CursorScreenPos.x;
      MousePos.y -= CursorScreenPos.y;

      
      if (Element.AllowSelection)
      {
         OutHovered = (MousePos.x > CalcPos.x && MousePos.x < CalcPos.x + SizeX && MousePos.y > CalcPos.y && MousePos.y < CalcPos.y + SizeY);
         //---- Visualize Selection
         if (OutHovered || Element.IsSelected)
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

      
      std::string Name = Element.Label;
      Name = Name.substr(4, Name.length() - 4);

      if (Element.AllowSelection)
      {
         const bool DidPressButton = ImGui::Button(Name.c_str(), Style.ElementSizes);
         if (DidPressButton)
         {
            ProcessGridElement(Category, Element);
         }
   
         // Check if mouse button is still active and if this is an element with a visible value, ignore those with INVALID_INDEX, they are not meant to have values
         if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && Element.DisplayValue)
         {
            const float YAcc = -ImGui::GetIO().MouseDelta.y / 2;
            Element.CurrentValue = ClampData(Element.CurrentValue + static_cast<int>(YAcc), Element.AllowNegativeValues ? -20 : 0, 20);
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
         ImGui::RenderText(ImVec2(CalcPos.x + HalfPaddingX * 0.5f, CalcPos.y + HalfPaddingY * 0.5f), Name.c_str(), Name.c_str() + Name.size() ); 
         ImGui::PopFont();
         CalcPos.x -= OffsetX;
         CalcPos.y -= OffsetY;
      }

      //---- Text
      ImGui::SetCursorPosX(CalcPos.x + Origin.x + HalfPaddingX);
      ImGui::SetCursorPosY(CalcPos.y + Origin.y + SizeY);
      ImGui::PushFont(GlobalSmallFont);

      if (Element.DisplayValue)
      {
         ImGui::Text("%02d", Element.CurrentValue);
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
   TemplateElementKeyIt CurrentSelectionTemplateKeyIt = UIHandler::CurrentSelectedElementPerTemplate.find(Category);
   const bool FoundCurrentTemplate = CurrentSelectionTemplateKeyIt != UIHandler::CurrentSelectedElementPerTemplate.end() && CurrentSelectionTemplateKeyIt->first != nullptr;
            
   TemplateElementKeyIt LastSelectionTemplateKeyIt = UIHandler::LastSelectedElementPerTemplate.find(Category);
   const bool FoundLastTemplate = LastSelectionTemplateKeyIt != UIHandler::LastSelectedElementPerTemplate.end() && LastSelectionTemplateKeyIt->first != nullptr;
            
   {
      GridElement* SelectedGridElement = FoundCurrentTemplate ? UIHandler::CurrentSelectedElementPerTemplate[Category] : nullptr;
      TemplateElementValueIt OverwriteElementIt = UIHandler::LastSelectedElementPerTemplate_BackMapping.find(SelectedGridElement);
      if (FoundLastTemplate == false)
      {
         UIHandler::LastSelectedElementPerTemplate.emplace(Category, SelectedGridElement);
         UIHandler::LastSelectedElementPerTemplate_BackMapping.emplace(SelectedGridElement, Category);
      }
      else
      {
         LastSelectionTemplateKeyIt->second = SelectedGridElement;
         if (OverwriteElementIt != LastSelectedElementPerTemplate_BackMapping.end())
         {
            LastSelectedElementPerTemplate_BackMapping[SelectedGridElement] = Category;
         }
         else
         {
            LastSelectedElementPerTemplate_BackMapping.emplace(SelectedGridElement,  Category);
         }
      }
   }

   TemplateElementValueIt OverwriteElementIt = UIHandler::CurrentSelectedElementPerTemplate_BackMapping.find(&Element);
   if (FoundCurrentTemplate == false)
   {
      UIHandler::CurrentSelectedElementPerTemplate.emplace(Category, &Element);
      UIHandler::CurrentSelectedElementPerTemplate_BackMapping.emplace(&Element, Category);
   }
   else
   {
      CurrentSelectionTemplateKeyIt->second = &Element;
      if (OverwriteElementIt != CurrentSelectedElementPerTemplate_BackMapping.end())
      {
         CurrentSelectedElementPerTemplate_BackMapping[&Element] = Category;
      }
      else
      {
         CurrentSelectedElementPerTemplate_BackMapping.emplace(&Element,  Category);
      }               
   }

   // Always call after making sure we've progressed our states
   Element.CallbackTarget(Element, Element.GetStateDelegate(Element) == false); //Flip state
}


//
// Behaviours (via delegate)
// Selection callback behaviour base
bool UIHandler::Callbacks::SelectInclusive(GridElement& CallingVisualElement)
{
   return CallingVisualElement.IsSelected = CallingVisualElement.IsSelected == false;
}

void UIHandler::Callbacks::SelectExclusive(GridElement& CallingVisualElement)
{
   // Only allow one selected at a time
   CallingVisualElement.IsSelected = true;

   TemplateElementValueIt CurrentSelectionTemplateKeyIt = UIHandler::CurrentSelectedElementPerTemplate_BackMapping.find(&CallingVisualElement);
   const bool FoundCurrentTemplate = CurrentSelectionTemplateKeyIt != UIHandler::CurrentSelectedElementPerTemplate_BackMapping.end() && CurrentSelectionTemplateKeyIt->second != nullptr;
   if (FoundCurrentTemplate == false) { return; }
   
   TemplateElementKeyIt LastSelectionTemplateKeyIt = LastSelectedElementPerTemplate.find(CurrentSelectionTemplateKeyIt->second);
   const bool FoundLastTemplate = LastSelectionTemplateKeyIt != UIHandler::LastSelectedElementPerTemplate.end() && LastSelectionTemplateKeyIt->second != nullptr;
   if (FoundLastTemplate == false) { return;}
   
   LastSelectionTemplateKeyIt->second->IsSelected = false;
}

//
// Selection callback behaviours -- Class Editor
void UIHandler::Callbacks::Class::SelectPrimeStat(GridElement& CallingVisualElement, bool Select)
{
   SelectExclusive(CallingVisualElement);

   if (CallingVisualElement.IsSelected)
   {
      UIHandler::EditableClass.PrimeStat = static_cast<Spec::Character::StatType>(CallingVisualElement.EnumSelectorValue);
   }
   else
   {
      UIHandler::EditableClass.PrimeStat = Spec::Character::StatType::Charisma;
   }
}   

void UIHandler::Callbacks::Class::SelectHitDie(GridElement& CallingVisualElement, bool Select)
{
   SelectExclusive(CallingVisualElement);

   if (CallingVisualElement.IsSelected)
   {
      UIHandler::EditableClass.HitDieShape = static_cast<Platonic::Dice>(CallingVisualElement.EnumSelectorValue);
   }
   else
   {
      UIHandler::EditableClass.HitDieShape = Platonic::Dice::E6D;
   }
}

//
// Selection callback behaviours -- Race Editor
void UIHandler::Callbacks::Race::SelectMinimumStat(GridElement& CallingVisualElement, bool Select)
{
   SelectInclusive(CallingVisualElement);

   if (CallingVisualElement.IsSelected)
   {
      const Spec::Character::StatType StatType = static_cast<Spec::Character::StatType>(CallingVisualElement.EnumSelectorValue);
      UIHandler::EditableRace.MinimumStats[StatType] = CallingVisualElement.CurrentValue;
   }
   else
   {
      const Spec::Character::StatType StatType = static_cast<Spec::Character::StatType>(CallingVisualElement.EnumSelectorValue);
      UIHandler::EditableRace.MinimumStats[StatType] = 0;      
   }
}   


void UIHandler::Callbacks::Race::SelectModifier(GridElement& CallingVisualElement, bool Select)
{
   SelectInclusive(CallingVisualElement);

   if (CallingVisualElement.IsSelected)
   {
      Spec::Character::StatType StatType = static_cast<Spec::Character::StatType>(CallingVisualElement.EnumSelectorValue);
      UIHandler::EditableRace.Modifiers[StatType] = CallingVisualElement.CurrentValue;
   }
   else
   {
      Spec::Character::StatType StatType = static_cast<Spec::Character::StatType>(CallingVisualElement.EnumSelectorValue);
      UIHandler::EditableRace.Modifiers[StatType] = 0;      
   }
}

void UIHandler::Callbacks::Race::SelectAllowedClass(GridElement& CallingVisualElement, bool Select)
{
   SelectInclusive(CallingVisualElement);
   

   if (CallingVisualElement.IsSelected)
   {
      Spec::Character::StatType StatType = static_cast<Spec::Character::StatType>(CallingVisualElement.EnumSelectorValue);
      UIHandler::EditableRace.AllowedClassIDs[StatType] = CallingVisualElement.CurrentValue;
   }
   else
   {
      Spec::Character::StatType StatType = static_cast<Spec::Character::StatType>(CallingVisualElement.EnumSelectorValue);
      UIHandler::EditableRace.AllowedClassIDs[StatType] = 0;      
   }
}   

//
// Selection callback behaviours -- Character Editor
void UIHandler::Callbacks::Character::SelectAlignment(GridElement& CallingVisualElement, bool Select)
{
   SelectExclusive(CallingVisualElement);

   if (CallingVisualElement.IsSelected)
   {
      UIHandler::EditableCharacter.Alignment = static_cast<Spec::Character::Alignment>(CallingVisualElement.EnumSelectorValue);
   }
   else // nothing is selected / was deselected
   {
      UIHandler::EditableCharacter.Alignment = Spec::Character::NeutralNeutral;
   }      
}



//
// Utility

ImFont* UIHandler::LoadFonts(ImGuiIO& io)
{
   io.Fonts->AddFontDefault();
   io.Fonts->AddFontFromFileTTF("../../rsc/Fonts/Besley/static/Besley-BlackItalic.ttf", 15.0f);
   ImFont* SmallFont = io.Fonts->AddFontFromFileTTF("../../rsc/Fonts/Besley/static/Besley-Black.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
   ImFont* LargeFont = io.Fonts->AddFontFromFileTTF("../../rsc/Fonts/Besley/static/Besley-Black.ttf", 24.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
   IM_ASSERT(SmallFont != nullptr);
   IM_ASSERT(LargeFont != nullptr);

   GlobalSmallFont = SmallFont;
   GlobalLargeFont = LargeFont;
   
   return SmallFont;
}
