#pragma once
#ifndef HANDLERUI_H
#define HANDLERUI_H

#include "imgui.h"
#include <map>
#include "shared.h"
#include "../editor_core.h"
#include "../../inc/ruleset.h"
#include "../../inc/ui/styling.h"

// fwd decl.
struct Template;

class UIHandler
{
public:
   static void DeinitUI();
   
   static std::string GetCharacterEditorStateLabel(bool& State);
   static std::string GetClassEditorStateLabel(bool& State);
   static std::string GetRaceEditorStateLabel(bool& State);
   static std::string GetCharacterSelectorStateLabel(bool& State);
   static void DrawAppMainMenuBar();
   static void DrawTemplates(Template* Category);
   static void PaintClassEditor(ImGuiInputTextFlags SharedInputTextFlags);
   static void PaintRaceEditor(ImGuiInputTextFlags SharedInputTextFlags);
   static void PaintCharacterEditor(ImGuiInputTextFlags SharedInputTextFlags);
   static void PaintCharacterSelector();
   static void OnPaint(ImGuiIO& io, ImVec4 ClearColor);

   // text callback sig
   // int(*ImGuiInputTextCallback)(ImGuiInputTextCallbackData* data);
   static int CallbackCharacterName(ImGuiInputTextCallbackData* data);
   static int CallbackClassName(ImGuiInputTextCallbackData* data);
   static int CallbackRaceName(ImGuiInputTextCallbackData* data);

   
   // static void SaveClass(bool SetOpen);
   // static void SaveRace(bool SetOpen);
   // static void SaveCharacter(bool SetOpen);

   static void SaveClass(ButtonPayload& ButtonPayload);
   static void SaveRace(ButtonPayload& ButtonPayload);
   static void SaveCharacter(ButtonPayload& ButtonPayload);
   static void LoadCharacter(ButtonPayload& ButtonPayload);
   
   static void SetClassEditorOpen(ButtonPayload& Payload)         { DrawClassEditor       = Payload.SetOpen; }
   static void SetRaceEditorOpen(ButtonPayload& Payload)          { DrawRaceEditor        = Payload.SetOpen; }
   static void SetCharacterEditorOpen(ButtonPayload& Payload)     { DrawCharacterEditor   = Payload.SetOpen; }
   static void SetCharacterSelectorOpen(ButtonPayload& Payload)   { DrawCharacterSelector = Payload.SetOpen; }
   static bool GetIsClassEditorOpen(ButtonPayload& Payload)       { return DrawClassEditor;}
   static bool GetIsRaceEditorOpen(ButtonPayload& Payload)        { return DrawRaceEditor; }
   static bool GetIsCharacterEditorOpen(ButtonPayload& Payload)   { return DrawCharacterEditor; }
   static bool GetIsCharacterSelectorOpen(ButtonPayload& Payload) { return DrawCharacterSelector; }
   
   // 
   // Utility
   static ImFont* LoadFonts(ImGuiIO& io);

   // These maps will trigger deallocation errors if managed as member variables,
   // this way we can avoid them being counted as non-local objects and thus avoid them
   // being initialized/deinitialized by the module
   static std::map<Template*, GridElement*>& LastSelectedElementPerTemplate()
   {
      static std::map<Template*, GridElement*> Map;
      return Map;
   }
   static std::map<GridElement*, Template*>& LastSelectedElementPerTemplate_BackMapping()
   {
      static std::map<GridElement*, Template*> Map;
      return Map;
   }
   static std::map<Template*, GridElement*>& CurrentSelectedElementPerTemplate()
   {
      static std::map<Template*, GridElement*> Map;
      return Map;
   }
   static std::map<GridElement*, Template*>& CurrentSelectedElementPerTemplate_BackMapping()
   {
      static std::map<GridElement*, Template*> Map;
      return Map;
   }
   
   static ImFont* GlobalSmallFont;
   static ImFont* GlobalLargeFont;
   static bool DrawClassEditor;
   static bool DrawRaceEditor;
   static bool DrawCharacterEditor;
   static bool DrawCharacterSelector;
   static bool BlockEditors;

   static std::string LastEditedClassName;
   static std::string LastEditedRaceName;
   static std::string LastEditedCharacterName;
   
   static CharacterClass EditableClass;
   static CharacterRace  EditableRace;
   static DnDCharacter   EditableCharacter;


   //@todo possibly just configure via json, having already sey up serialization for the stylings
   static CE::Style::Compound Stylings[CE::Style::StylingSelector::End];
};

#endif // HANDLERUI_H
