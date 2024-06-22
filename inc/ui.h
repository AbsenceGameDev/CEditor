#pragma once
#ifndef UI_H
#define UI_H

#include "imgui.h"
#include <d3d11.h>
#include <functional>
#include <vector>

#include "../inc/dataformat.h"

auto DummyStateDelegate = [](void) -> bool { return false; };
auto DummyCallbackTarget = [](bool) -> void
{
};

// fwd decl.
struct Template;

class UIHandler
{
public:
   static void DrawAppMainMenuBar();
   static void DrawTemplates(Template* Category);
   static void OnPaint(ImGuiIO& io, ImVec4 ClearColor);

   // text callback sig
   // int(*ImGuiInputTextCallback)(ImGuiInputTextCallbackData* data);
   static int CallbackCharacterName(ImGuiInputTextCallbackData* data);
   static int CallbackClassName(ImGuiInputTextCallbackData* data);
   static int CallbackRaceName(ImGuiInputTextCallbackData* data);

   
   // static void SaveClass(bool SetOpen);
   // static void SaveRace(bool SetOpen);
   // static void SaveCharacter(bool SetOpen);

   static void SetClassEditorOpen(bool SetOpen)      { DrawClassEditor     = SetOpen; }
   static void SetRaceEditorOpen(bool SetOpen)       { DrawRaceEditor      = SetOpen; }
   static void SetCharacterEditorOpen(bool SetOpen)  { DrawCharacterEditor = SetOpen; }
   static bool GetIsClassEditorOpen()                { return DrawClassEditor; }
   static bool GetIsRaceEditorOpen()                 { return DrawRaceEditor; }
   static bool GetIsCharacterEditorOpen()            { return DrawCharacterEditor; }


   // 
   // Utility
   static ImFont* LoadFonts(ImGuiIO& io);

   
   
   static ImFont* GlobalFont;
   static bool DrawClassEditor;
   static bool DrawRaceEditor;
   static bool DrawCharacterEditor;

   static CharacterClass EditableClass;
   static CharacterRace  EditableRace;
   static DnDCharacter   EditableCharacter;
};

struct ElementStyle
{
   ImVec2 ElementSizes = {50, 50};
   ImVec2 Padding = {20, 20};
   ImTextureID ImageID{};
};

struct GridElement
{
   GridElement()
   {
   }

   GridElement(
      const std::string& InLabel,
      ElementStyle InStyle,
      int InCurrentValue = 0,
      std::function<bool(void)> InStateDelegate = {},
      std::function<void(bool)> InCallbackTarget = {}
   )
      : Label(InLabel)
        , Style(InStyle)
        , IsSelected(false)
        , CurrentValue(InCurrentValue)
        , GetStateDelegate(InStateDelegate)
        , CallbackTarget(InCallbackTarget)
   {
   }

   std::string Label{"Test"};
   ElementStyle Style{};
   bool IsSelected = false;
   int CurrentValue = 0;

   std::function<bool(void)> GetStateDelegate;
   std::function<void(bool)> CallbackTarget;
};

typedef struct Template
{
   int LastSelectedElement = -1;
   int CurrentSelectedElement = -1;
   std::vector<GridElement> Inner;
} Template_t;

namespace UI
{
   static constexpr ImVec2 DefaultPadding = {20, 20};
   static constexpr ImVec2 DefaultSizes = {50, 50};
   static constexpr ElementStyle DefaultStyle = {DefaultSizes, DefaultPadding, ImTextureID{}};

   namespace Class
   {
      static const std::string StrLabel = ("ps::STR");
      static const std::string IntLabel = ("ps::INT");
      static const std::string WisLabel = ("ps::WIS");
      static const std::string DexLabel = ("ps::DEX");
      static const std::string ConLabel = ("ps::CON");
      static const std::string ChaLabel = ("ps::CHA");
      static Template PrimeStatTemplates{
         INVALID_INDEX,
         INVALID_INDEX,
         {
            GridElement{StrLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{IntLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{WisLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{DexLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{ConLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{ChaLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget}
         }
      };

      static const std::string D4Label = ("ps::1d4");
      static const std::string D6Label = ("ps::1d6");
      static const std::string D8Label = ("ps::1d8");
      static const std::string D12Label = ("ps::1d12");
      static const std::string D20Label = ("ps::1d20");
      static Template PlatonicDiceTemplates{
         INVALID_INDEX,
         INVALID_INDEX,
         {
            GridElement{D4Label, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{D6Label, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{D8Label, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{D12Label, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{D20Label, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget}
         }
      };
   }


   namespace Race
   {
      static const std::string msStrLabel = ("ms::STR");
      static const std::string msIntLabel = ("ms::INT");
      static const std::string msWisLabel = ("ms::WIS");
      static const std::string msDexLabel = ("ms::DEX");
      static const std::string msConLabel = ("ms::CON");
      static const std::string msChaLabel = ("ms::CHA");

      static Template MinimumStatsTemplates{
         INVALID_INDEX,
         INVALID_INDEX,
         {
            GridElement{msStrLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{msIntLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{msWisLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{msDexLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{msConLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{msChaLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget}
         }
      };

      static const std::string mfStrLabel = ("mf::STR");
      static const std::string mfIntLabel = ("mf::INT");
      static const std::string mfWisLabel = ("mf::WIS");
      static const std::string mfDexLabel = ("mf::DEX");
      static const std::string mfConLabel = ("mf::CON");
      static const std::string mfChaLabel = ("mf::CHA");      
      static Template ModifierTemplates{
         INVALID_INDEX,
         INVALID_INDEX,
         {
            GridElement{mfStrLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{mfIntLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{mfWisLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{mfDexLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{mfConLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget},
            GridElement{mfChaLabel, DefaultStyle, 0, DummyStateDelegate, DummyCallbackTarget}
         }
      };      
   }

   namespace Character
   {
      static constexpr ImVec2 CharacterEditorPadding = {20, 20};
      static constexpr ImVec2 CharacterEditorSizes = {120, 40};
      static constexpr ElementStyle CharacterEditorStyle = {CharacterEditorSizes, CharacterEditorPadding, ImTextureID{}};

      
      static const std::string agLawGoodLabel        = ("ag::Lawful Good");
      static const std::string agLawNeutralLabel     = ("ag::Lawful Neutral");
      static const std::string agLawEvilLabel        = ("ag::Lawful Evil");
      
      static const std::string agNeutralGoodLabel    = ("ag::Neutral Good");
      static const std::string agNeutralNeutralLabel = ("ag::Neutral Neutral");
      static const std::string agNeutralEvilLabel    = ("ag::Neutral Evil");

      static const std::string agChaoticGoodLabel    = ("ag::Chaotic Good");
      static const std::string agChaoticNeutralLabel = ("ag::Chaotic Neutral");
      static const std::string agChaoticEvilLabel    = ("ag::Chaotic Evil");      
      
      static Template LawfulAlignmentTemplates{
         INVALID_INDEX,
         INVALID_INDEX,
         {
            GridElement{agLawGoodLabel,        CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{agLawNeutralLabel,     CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{agLawEvilLabel,        CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget}
         }
      };

      static Template NeutralAlignmentTemplates{
         INVALID_INDEX,
         INVALID_INDEX,
         {
            GridElement{agNeutralGoodLabel,    CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{agNeutralNeutralLabel, CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{agNeutralEvilLabel,    CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget}
         }
      };

      static Template ChaoticAlignmentTemplates{
         INVALID_INDEX,
         INVALID_INDEX,
         {
            GridElement{agChaoticGoodLabel,    CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{agChaoticNeutralLabel, CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{agChaoticEvilLabel,    CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget}
         }
      };
            
      /*
      static Template AlignmentTemplates{
         INVALID_INDEX,
         INVALID_INDEX,
         {
            GridElement{agLawGoodLabel,        CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{agLawNeutralLabel,     CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{agLawEvilLabel,        CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},

            GridElement{agNeutralGoodLabel,    CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{agNeutralNeutralLabel, CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{agNeutralEvilLabel,    CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            
            GridElement{agChaoticGoodLabel,    CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{agChaoticNeutralLabel, CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{agChaoticEvilLabel,    CharacterEditorStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget}
         }
      };
       */

      
      
   }


   static constexpr ImVec2 MenuPadding = {15, 15};
   static constexpr ImVec2 MenuSizes = {160, 30};
   static constexpr ElementStyle MenuStyle = {MenuSizes, MenuPadding, ImTextureID{}};

   namespace Menu
   {
      static const std::string SaveClassLabel = ("cl::Save Class Config @todo Finish");
      static const std::string LoadClassLabel = ("cl::Load Class Config @todo Finish");
      static const std::string CloseClassLabel = ("cl::Close Class Editor");
      static Template ClassMenuControlTemplates{
         INVALID_INDEX,
         INVALID_INDEX,
         {
            GridElement{SaveClassLabel, MenuStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{LoadClassLabel, MenuStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{ CloseClassLabel, MenuStyle, INVALID_INDEX, &UIHandler::GetIsClassEditorOpen, &UIHandler::SetClassEditorOpen} 
         }
      };

      static const std::string SaveRaceLabel = ("ra::Save Race Config @todo Finish");
      static const std::string LoadRaceLabel = ("ra::Load Race Config @todo Finish");
      static const std::string CloseRaceLabel = ("ra::Close Race Editor");
      static Template RaceMenuControlTemplates{
         INVALID_INDEX,
         INVALID_INDEX,
         {
            GridElement{SaveRaceLabel, MenuStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{LoadRaceLabel, MenuStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{CloseRaceLabel, MenuStyle, INVALID_INDEX, &UIHandler::GetIsRaceEditorOpen, &UIHandler::SetRaceEditorOpen}
         }
      };


      static const std::string SaveCharacterLabel = ("ch::Save Character Config @todo Finish");
      static const std::string LoadCharacterLabel = ("ch::Load Character Config @todo Finish");
      static const std::string CloseCharacterLabel = ("ch::Close Character Editor");
      static Template CharacterMenuControlTemplates{
         INVALID_INDEX,
         INVALID_INDEX,
         {
            GridElement{SaveCharacterLabel, MenuStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{LoadCharacterLabel, MenuStyle, INVALID_INDEX, DummyStateDelegate, DummyCallbackTarget},
            GridElement{CloseCharacterLabel, MenuStyle, INVALID_INDEX, &UIHandler::GetIsCharacterEditorOpen, &UIHandler::SetCharacterEditorOpen}
         }
      };
   }
}

#endif // UI_H
