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

ImFont* UIHandler::GlobalFont = nullptr;
bool UIHandler::DrawClassEditor = true;
bool UIHandler::DrawRaceEditor = true;
bool UIHandler::DrawCharacterEditor = true;

GenericName DummyCharName = "Default\0";

bool DummyInitializer;
CharacterClass UIHandler::EditableClass = {DummyCharName, Character::Spec::StatType::Charisma, Platonic::Dice::E4D, DEFAULT_START_STATE };
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
         UIHandler::SetCharacterEditorOpen(true);
      }

      if (ImGui::Button("Open Class Editor"))
      {
         UIHandler::SetClassEditorOpen(true);
      }

      if (ImGui::Button("Open Race Editor"))
      {
         UIHandler::SetRaceEditorOpen(true);
      }            

      ImGui::EndMainMenuBar();
   }
   
   ImGui::PopStyleVar();   
   ImGui::PopStyleVar();   
   ImGui::PopStyleVar();   
}

static void TooltipMarker(const char* desc)
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

   ImGuiInputTextFlags SharedInputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;


   ImGui::PushFont(GlobalFont);
   
   // Create a Class editor window
   if (DrawClassEditor)
   {
      static char classbuf[Data::Format::NameSize];
      
      ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(600, 450));
      ImGui::Begin("CEditor Class Creator", 0, ImGuiWindowFlags_NoCollapse);

      const char* Input = "Class Base";
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
      Template ClassTemplates{ INVALID_INDEX, INVALID_INDEX, {} };
      MakeTree("{Allowed Classes Selector}",
         {
            for (const std::pair<const int, CharacterClass>& ClassPair : EditorCore::MappedClasses)
            {
               const std::string NameAsString = ClassListPrefix + ClassPair.second.Name;
               ClassTemplates.Inner.emplace_back(NameAsString, UI::CharacterEditorStyle, INVALID_INDEX, false, DummyStateDelegate, DummyCallbackTarget);
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
      Template ClassTemplates{INVALID_INDEX, INVALID_INDEX, {} };
      MakeTree("{Class Selector}", 
         {
            for (const std::pair<const int, CharacterClass>& ClassPair : EditorCore::MappedClasses)
            {
               const std::string NameAsString = ClassListPrefix + ClassPair.second.Name;
               ClassTemplates.Inner.emplace_back(NameAsString, UI::CharacterEditorStyle, INVALID_INDEX, false, DummyStateDelegate, DummyCallbackTarget);
            }
            DrawTemplates(&ClassTemplates);
         }
      )
      
      //
      // Race selector tree @todo Finish race selection
      static const std::string RaceListPrefix = "rs::";
      Template RaceTemplates{ INVALID_INDEX, INVALID_INDEX,{}};
      MakeTree("{Race Selector} : @todo Selected and/or Rolled Race",
         {
            for (const std::pair<const int, CharacterRace>& RacePair : EditorCore::MappedRaces)
            {
               const std::string NameAsString = RaceListPrefix + RacePair.second.Name;
               RaceTemplates.Inner.emplace_back(NameAsString, UI::CharacterEditorStyle, INVALID_INDEX, false, DummyStateDelegate, DummyCallbackTarget);
            }
            DrawTemplates(&RaceTemplates);
         }
      )
      
      // Alignment selector tree @todo Finish Alignment selection, make sure we can't slect from teh different aligment templates, or put them all in one template and wrap by 3 element
      MakeTree("{Alignment Selector - @todo Selected or rolled}",
         {
            DrawTemplates(&UI::Character::LawfulAlignmentTemplates);
            ImGui::NewLine();
            DrawTemplates(&UI::Character::NeutralAlignmentTemplates);
            ImGui::NewLine();
            DrawTemplates(&UI::Character::ChaoticAlignmentTemplates);
         }
      )
      
      
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

void UIHandler::SaveClass(bool SetOpen)
{
   EditorCore::CreateClass(UIHandler::EditableClass);
}

void UIHandler::SaveRace(bool SetOpen)
{
   EditorCore::CreateRace(UIHandler::EditableRace);
}

void UIHandler::SaveCharacter(bool SetOpen)
{
   EditorCore::CreateCharacter(UIHandler::EditableCharacter);
}


//
// Grid element viewer
void UIHandler::DrawTemplates(Template* Category)
{
   const ImVec2& CursorScreenPos = ImGui::GetCursorScreenPos();
   const ImVec2& Origin = ImGui::GetCursorPos();

   float X = 0.f;
   float Y = 0.f;

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

      bool OutHovered;
      bool OutHeld;
      ImGui::ButtonBehavior(ImRect(X, Y, X + SizeX, Y + SizeY), id, &OutHovered, &OutHeld, ImGuiButtonFlags_PressedOnClick);

      ImVec2 MousePos = ImGui::GetMousePos();
      MousePos.x -= CursorScreenPos.x;
      MousePos.y -= CursorScreenPos.y;
      OutHovered = (MousePos.x > X && MousePos.x < X + SizeX && MousePos.y > Y && MousePos.y < Y + SizeY);

      //---- Selection
      if (OutHovered || Element.IsSelected)
      {
         const float OffsetX = CursorScreenPos.x + HalfPaddingX;
         const float OffsetY = CursorScreenPos.y + HalfPaddingY;

         const ImU32 Column = ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered]);
         X += OffsetX;
         Y += OffsetY;
         ImGui::RenderFrame(ImVec2(X, Y), ImVec2(X + SizeX, Y + SizeY), Column, true, 3.f);
         X -= OffsetX;
         Y -= OffsetY;
      }

      ImGui::SetCursorPosX(X + Origin.x + HalfPaddingX);
      ImGui::SetCursorPosY(Y + Origin.y + HalfPaddingY);
      // CreateTexture(Element);
      // ImGui::Image(Element.ImageID, ImVec2(50, 50));


      std::string Name = Element.Label;
      Name = Name.substr(4, Name.length() - 4);

      const bool DidPressButton = ImGui::Button(Name.c_str(), Style.ElementSizes);
      if (DidPressButton)
      {
         Category->LastSelectedElement = Category->CurrentSelectedElement;
         Category->CurrentSelectedElement = static_cast<int>(i);

         Element.CallbackTarget(Element.GetStateDelegate() == false); //Flip state
      }

      // Check if mouse button is still active and if this is an element wiht a visible value, ignore those with INVALID_INDEX, they are not meant to have values
      if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && (Element.AllowNegativeValues || Element.CurrentValue != INVALID_INDEX))
      {
         const float YAcc = -ImGui::GetIO().MouseDelta.y / 2;
         Element.CurrentValue = ClampData(Element.CurrentValue + static_cast<int>(YAcc), Element.AllowNegativeValues ? -20 : 0, 20);
      }
      

      if (OutHovered)
      {
         const bool SelectNewItem = DidPressButton && Category->LastSelectedElement != Category->CurrentSelectedElement;
         if (Element.IsSelected == false)
         {
            Element.IsSelected = SelectNewItem; // select new

            // deselect old
            if (Element.IsSelected && Category->LastSelectedElement != INVALID_INDEX)
            {
               GridElement& LastSelectedElement = Category->Inner[Category->LastSelectedElement];
               LastSelectedElement.IsSelected = false;
            }
         }
      }

      //---- Text
      ImGui::SetCursorPosX(X + Origin.x + HalfPaddingX);
      ImGui::SetCursorPosY(Y + Origin.y + SizeY);
      ImGui::PushFont(GlobalFont);

      if (Element.AllowNegativeValues || Element.CurrentValue >= 0)
      {
         ImGui::Text("%02d", Element.CurrentValue);
      }

      ImGui::PopFont();

      //---- Next position
      X += SizeX + HalfPaddingX * 2;
      if (X + SizeX > ImGui::GetWindowWidth())
      {
         X = 0;
         Y += SizeY + HalfPaddingY * 2;
      }

      ImGui::PopID();
   }
}



//
// Utility

ImFont* UIHandler::LoadFonts(ImGuiIO& io)
{
   io.Fonts->AddFontDefault();
   io.Fonts->AddFontFromFileTTF("../../rsc/Fonts/Besley/static/Besley-BlackItalic.ttf", 15.0f);
   ImFont* font = io.Fonts->AddFontFromFileTTF("../../rsc/Fonts/Besley/static/Besley-Black.ttf", 18.0f, nullptr,
                                               io.Fonts->GetGlyphRangesDefault());
   IM_ASSERT(font != nullptr);

   return font;
}
