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
#include "../../inc/ui/editor_widgets.h"
#include "../../inc/editor_core.h"


#define PDDEBUG

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
DnDCharacter UIHandler::EditableCharacter = DnDCharacter::ConstructOnStack(DummyCharName, DummyInitializer, UIHandler::EditableRace, UIHandler::EditableClass, DEFAULT_START_STATE);

void UIHandler::DeinitUI()
{
   BlockEditors = true;

   LastSelectedElementPerTemplate().clear();
   LastSelectedElementPerTemplate_BackMapping().clear();
   CurrentSelectedElementPerTemplate().clear();
   CurrentSelectedElementPerTemplate_BackMapping().clear();
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
   WMainMenu::Draw(UIHandler::Stylings[CE::Style::StylingSelector::MainMenuBar]);
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

void UIHandler::PaintClassEditor(ImGuiInputTextFlags InputTextFlags)
{
   if (DrawClassEditor == false ) { return;}

   WClassEditor::Draw(UIHandler::Stylings[CE::Style::StylingSelector::ClassEditor]);
}

void UIHandler::PaintRaceEditor(ImGuiInputTextFlags InputTextFlags)
{
   if (DrawRaceEditor == false) { return;}

   WRaceEditor::Draw(UIHandler::Stylings[CE::Style::StylingSelector::RaceEditor]);
}

void UIHandler::PaintCharacterEditor(ImGuiInputTextFlags InputTextFlags)
{
   if (DrawCharacterEditor == false) { return; }
   
   WCharacterEditor::Draw(UIHandler::Stylings[CE::Style::StylingSelector::CharacterEditor]);
}

void UIHandler::PaintCharacterSelector()
{
   if (DrawCharacterSelector == false) { return;}

   WCharacterSelector::Draw(UIHandler::Stylings[CE::Style::StylingSelector::CharacterSelector]);
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
   
   ImGui::PushFont(GlobalSmallFont);

   // Create a Class editor window
   PaintClassEditor(SharedInputTextFlags);

   // Create a Race editor window
   PaintRaceEditor(SharedInputTextFlags);

   // Create a Character editor window
   PaintCharacterEditor(SharedInputTextFlags);

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

void UIHandler::DrawTemplates(Template* Category)
{
   Category->Draw();
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
