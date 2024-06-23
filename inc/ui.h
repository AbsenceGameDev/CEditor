#pragma once
#ifndef UI_H
#define UI_H

#include "imgui.h"
#include <d3d11.h>
#include <functional>
#include <map>
#include <vector>

#include "../inc/dataformat.h"
#include "../inc/ruleset.h"

auto DummyStateDelegate = [](const struct GridElement&) -> bool { return false; };
auto DummyCallbackTarget = [](const struct GridElement&, bool) -> void { };

using TemplateElementKeyIt = std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<struct Template* const, struct GridElement*>>>>&&;
using TemplateElementValueIt  = std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<struct GridElement* const,  struct Template*>>>>&&;

// fwd decl.
struct Template;

class UIHandler
{
public:
   static void DrawAppMainMenuBar();
   static void ProcessGridElement(Template* Category, GridElement& Element);
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

   static void SaveClass(GridElement& CallingViualElement, bool SetOpen);
   static void SaveRace(GridElement& CallingViualElement, bool SetOpen);
   static void SaveCharacter(GridElement& CallingViualElement, bool SetOpen);

   typedef struct Callbacks
   {
      static bool SelectInclusive(GridElement& CallingVisualElement);
      static void SelectExclusive(GridElement& CallingVisualElement);
      typedef struct Class
      {
         static void SelectPrimeStat(GridElement& CallingVisualElement, bool Select);
         static void SelectHitDie(GridElement& CallingVisualElement, bool Select);
      }class_t;

      typedef struct Race
      {
         static void SelectMinimumStat(GridElement& CallingVisualElement, bool Select);
         static void SelectModifier(GridElement& CallingVisualElement, bool Select);
         static void SelectAllowedClass(GridElement& CallingVisualElement, bool Select);
      }race_t;

      typedef struct Character
      {
         static void SelectAlignment(GridElement& CallingVisualElement, bool Select);
      }character_t;   
   }callbacks_t;
   
   static void SetClassEditorOpen(GridElement&       CallingViualElement, bool SetOpen) { DrawClassEditor     = SetOpen; }
   static void SetRaceEditorOpen(GridElement&        CallingViualElement, bool SetOpen) { DrawRaceEditor      = SetOpen; }
   static void SetCharacterEditorOpen(GridElement&   CallingViualElement, bool SetOpen) { DrawCharacterEditor = SetOpen; }
   static bool GetIsClassEditorOpen(GridElement&     CallingViualElement) { return DrawClassEditor; }
   static bool GetIsRaceEditorOpen(GridElement&      CallingViualElement) { return DrawRaceEditor; }
   static bool GetIsCharacterEditorOpen(GridElement& CallingViualElement) { return DrawCharacterEditor; }


   // 
   // Utility
   static ImFont* LoadFonts(ImGuiIO& io);

   static std::map<Template*, GridElement*> LastSelectedElementPerTemplate;
   static std::map<GridElement*, Template*> LastSelectedElementPerTemplate_BackMapping;
   static std::map<Template*, GridElement*> CurrentSelectedElementPerTemplate;
   static std::map<GridElement*, Template*> CurrentSelectedElementPerTemplate_BackMapping;
   
   static ImFont* GlobalSmallFont;
   static ImFont* GlobalLargeFont;
   static bool DrawClassEditor;
   static bool DrawRaceEditor;
   static bool DrawCharacterEditor;
   static bool BlockEditors;

   static std::string LastEditedClassName;
   static std::string LastEditedRaceName;
   static std::string LastEditedCharacterName;
   
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
      int InEnumSelectorValue = 0,
      bool InAllowNegativeValues = false,
      bool InAllowSelection      = true,
      bool InDisplayValue        = true, 
      std::function<bool(GridElement&)> InStateDelegate = {},
      std::function<void(GridElement&, bool)> InCallbackTarget = {}
   )
      : Label(InLabel)
        , Style(InStyle)
        , IsSelected(false)
        , CurrentValue(InCurrentValue)
        , EnumSelectorValue(InEnumSelectorValue)
        , AllowNegativeValues(InAllowNegativeValues)
        , AllowSelection(InAllowSelection)
        , DisplayValue(InDisplayValue)
        , GetStateDelegate(InStateDelegate)
        , CallbackTarget(InCallbackTarget)
   {
   }

   std::string Label{"DefaultLabel"};
   ElementStyle Style               = {};
   bool         IsSelected          = false;
   int          CurrentValue        = 0;
   int          EnumSelectorValue   = 0;
   bool         AllowNegativeValues = false;
   bool         AllowSelection      = true;
   bool         DisplayValue        = true;

   std::function<bool(GridElement&)> GetStateDelegate;
   std::function<void(GridElement&, bool)> CallbackTarget;
};

typedef struct Template
{
   std::vector<GridElement> Inner;

   ImVec2 MaxSize {-1, -1}; // {x,y} Negative one indicates there is no limit 
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
         {
            GridElement{StrLabel, DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Strength, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectPrimeStat},
            GridElement{IntLabel, DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Intelligence, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectPrimeStat},
            GridElement{WisLabel, DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Wisdom, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectPrimeStat},
            GridElement{DexLabel, DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Dexterity, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectPrimeStat},
            GridElement{ConLabel, DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Constitution, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectPrimeStat},
            GridElement{ChaLabel, DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Charisma, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectPrimeStat}
         }
      };

      static const std::string D4Label = ("ps::1d4");
      static const std::string D6Label = ("ps::1d6");
      static const std::string D8Label = ("ps::1d8");
      static const std::string D12Label = ("ps::1d12");
      static const std::string D20Label = ("ps::1d20");
      static Template PlatonicDiceTemplates{
         {
            GridElement{D4Label, DefaultStyle, INVALID_INDEX, Platonic::Dice::E4D,  false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectHitDie},
            GridElement{D6Label, DefaultStyle, INVALID_INDEX, Platonic::Dice::E6D,  false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectHitDie},
            GridElement{D8Label, DefaultStyle, INVALID_INDEX, Platonic::Dice::E8D,  false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectHitDie},
            GridElement{D12Label, DefaultStyle, INVALID_INDEX, Platonic::Dice::E12D, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectHitDie},
            GridElement{D20Label, DefaultStyle, INVALID_INDEX,  Platonic::Dice::E20D, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectHitDie}
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
         {
            GridElement{msStrLabel, DefaultStyle, 0, Spec::Character::StatType::Strength, false, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectMinimumStat},
            GridElement{msIntLabel, DefaultStyle, 0, Spec::Character::StatType::Intelligence, false, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectMinimumStat},
            GridElement{msWisLabel, DefaultStyle, 0, Spec::Character::StatType::Wisdom, false, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectMinimumStat},
            GridElement{msDexLabel, DefaultStyle, 0, Spec::Character::StatType::Dexterity, false, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectMinimumStat},
            GridElement{msConLabel, DefaultStyle, 0, Spec::Character::StatType::Constitution, false, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectMinimumStat},
            GridElement{msChaLabel, DefaultStyle, 0, Spec::Character::StatType::Charisma, false, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectMinimumStat}
         }
      };

      static const std::string mfStrLabel = ("mf::STR");
      static const std::string mfIntLabel = ("mf::INT");
      static const std::string mfWisLabel = ("mf::WIS");
      static const std::string mfDexLabel = ("mf::DEX");
      static const std::string mfConLabel = ("mf::CON");
      static const std::string mfChaLabel = ("mf::CHA");      
      static Template ModifierTemplates{
         {
            GridElement{mfStrLabel, DefaultStyle, 0, Spec::Character::StatType::Strength, true, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectModifier},
            GridElement{mfIntLabel, DefaultStyle, 0, Spec::Character::StatType::Intelligence, true, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectModifier},
            GridElement{mfWisLabel, DefaultStyle, 0, Spec::Character::StatType::Wisdom, true, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectModifier},
            GridElement{mfDexLabel, DefaultStyle, 0, Spec::Character::StatType::Dexterity, true, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectModifier},
            GridElement{mfConLabel, DefaultStyle, 0, Spec::Character::StatType::Constitution, true, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectModifier},
            GridElement{mfChaLabel, DefaultStyle, 0, Spec::Character::StatType::Charisma, true, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectModifier}
         }
      };      
   }

   static constexpr ImVec2 CharacterEditorPadding = {20, 20};
   static constexpr ImVec2 CharacterEditorSizes = {120, 40};
   static constexpr ElementStyle CharacterEditorStyle = {CharacterEditorSizes, CharacterEditorPadding, ImTextureID{}};
   namespace Character
   {
      static const std::string chStrLabel = ("ch::STR");
      static const std::string chIntLabel = ("ch::INT");
      static const std::string chWisLabel = ("ch::WIS");
      static const std::string chDexLabel = ("ch::DEX");
      static const std::string chConLabel = ("ch::CON");
      static const std::string chChaLabel = ("ch::CHA");      
      static Template CharacterStatTemplates{
         {
            GridElement{chStrLabel, DefaultStyle, 0, Spec::Character::StatType::Strength, true, false, true, DummyStateDelegate, DummyCallbackTarget},
            GridElement{chIntLabel, DefaultStyle, 0, Spec::Character::StatType::Intelligence, true, false, true, DummyStateDelegate, DummyCallbackTarget},
            GridElement{chWisLabel, DefaultStyle, 0, Spec::Character::StatType::Wisdom, true, false, true, DummyStateDelegate, DummyCallbackTarget},
            GridElement{chDexLabel, DefaultStyle, 0, Spec::Character::StatType::Dexterity, true, false, true, DummyStateDelegate, DummyCallbackTarget},
            GridElement{chConLabel, DefaultStyle, 0, Spec::Character::StatType::Constitution, true, false, true, DummyStateDelegate, DummyCallbackTarget},
            GridElement{chChaLabel, DefaultStyle, 0, Spec::Character::StatType::Charisma, true, false, true, DummyStateDelegate, DummyCallbackTarget}
         }
      };
      
      static const std::string agLawGoodLabel        = ("ag::Lawful Good");
      static const std::string agLawNeutralLabel     = ("ag::Lawful Neutral");
      static const std::string agLawEvilLabel        = ("ag::Lawful Evil");
      
      static const std::string agNeutralGoodLabel    = ("ag::Neutral Good");
      static const std::string agNeutralNeutralLabel = ("ag::Neutral Neutral");
      static const std::string agNeutralEvilLabel    = ("ag::Neutral Evil");

      static const std::string agChaoticGoodLabel    = ("ag::Chaotic Good");
      static const std::string agChaoticNeutralLabel = ("ag::Chaotic Neutral");
      static const std::string agChaoticEvilLabel    = ("ag::Chaotic Evil");      
      
      
      static Template AlignmentTemplates{
         {
            GridElement{agLawGoodLabel,        CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::LawGood, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment},
            GridElement{agLawNeutralLabel,     CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::LawNeutral, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment},
            GridElement{agLawEvilLabel,        CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::LawEvil, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment},
            GridElement{agNeutralGoodLabel,    CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::NeutralGood, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment},
            GridElement{agNeutralNeutralLabel, CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::NeutralNeutral, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment},
            GridElement{agNeutralEvilLabel,    CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::NeutralEvil, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment},
            GridElement{agChaoticGoodLabel,    CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::ChaosGood, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment},
            GridElement{agChaoticNeutralLabel, CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::ChaosNeutral, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment},
            GridElement{agChaoticEvilLabel,    CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::ChaosEvil, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment}
         },
         ImVec2{3 * (CharacterEditorStyle.ElementSizes.x + CharacterEditorStyle.Padding.x), 3 * (CharacterEditorStyle.ElementSizes.y + CharacterEditorStyle.Padding.y)}
      };
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
         {
            GridElement{SaveClassLabel, MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, DummyStateDelegate, &UIHandler::SaveClass},
            GridElement{LoadClassLabel, MenuStyle, INVALID_INDEX, INVALID_INDEX,  false, true, false, DummyStateDelegate, DummyCallbackTarget},
            GridElement{ CloseClassLabel, MenuStyle, INVALID_INDEX, INVALID_INDEX,  false, true, false, &UIHandler::GetIsClassEditorOpen, &UIHandler::SetClassEditorOpen} 
         }
      };

      static const std::string SaveRaceLabel = ("ra::Save Race Config @todo Finish");
      static const std::string LoadRaceLabel = ("ra::Load Race Config @todo Finish");
      static const std::string CloseRaceLabel = ("ra::Close Race Editor");
      static Template RaceMenuControlTemplates{
         {
            GridElement{SaveRaceLabel, MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, DummyStateDelegate, &UIHandler::SaveRace},
            GridElement{LoadRaceLabel, MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, DummyStateDelegate, DummyCallbackTarget},
            GridElement{CloseRaceLabel, MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, &UIHandler::GetIsRaceEditorOpen, &UIHandler::SetRaceEditorOpen}
         }
      };


      static const std::string SaveCharacterLabel = ("ch::Save Character Config @todo Finish");
      static const std::string LoadCharacterLabel = ("ch::Load Character Config @todo Finish");
      static const std::string CloseCharacterLabel = ("ch::Close Character Editor");
      static Template CharacterMenuControlTemplates{
         {
            GridElement{SaveCharacterLabel, MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, DummyStateDelegate, &UIHandler::SaveCharacter},
            GridElement{LoadCharacterLabel, MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, DummyStateDelegate, DummyCallbackTarget},
            GridElement{CloseCharacterLabel, MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, &UIHandler::GetIsCharacterEditorOpen, &UIHandler::SetCharacterEditorOpen}
         }
      };
   }
}

#endif // UI_H
