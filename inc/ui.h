#pragma once
#ifndef UI_H
#define UI_H

#include "imgui.h"
#include <d3d11.h>
#include <deque>
#include <functional>
#include <map>
#include <set>
#include <vector>

#include "editor_core.h"
#include "../inc/dataformat.h"
#include "../inc/ruleset.h"


using TemplateElementKeyIt = std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<struct Template* const, struct GridElement*>>>>&&;
using TemplateElementValueIt  = std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<struct GridElement* const,  struct Template*>>>>&&;

namespace Settings
{
   enum GridElement
   {
      AllowSelection      = 0b0001,
      AllowNegativeValues = 0b0010,
      DisplayValue        = 0b0100,
         
      NOT_SET = 0b0111000
   };
   
}

namespace Override
{
   namespace Behaviour
   {
      using Category = Settings::GridElement;
      
      template<Override::Behaviour::Category CompareBit>
      inline bool IsOverriddenBehaviour(int Flags) { return false; };

      template<Override::Behaviour::Category CompareBit>
      inline bool GetOverriddenState(int Flags) { return false; };      
      
      template<Override::Behaviour::Category SetBit, bool TSetTrue>
      inline int OverrideBehaviour(int Flags) { return Flags; };

      template<Override::Behaviour::Category SetBit>
      inline int ResetOverrideState(int Flags) { return Flags; };            
   };
}

template<> inline bool Override::Behaviour::IsOverriddenBehaviour <Override::Behaviour::Category::AllowSelection>(int Flags)
{ return (Flags & 0b00010000) == 0; };
template<> inline bool Override::Behaviour::IsOverriddenBehaviour<Override::Behaviour::Category::AllowNegativeValues>(int Flags)
{ return (Flags & 0b00100000) == 0; };
template<> inline bool Override::Behaviour::IsOverriddenBehaviour<Override::Behaviour::Category::DisplayValue>(int Flags)
{ return (Flags & 0b01000000) == 0; };

template<> inline bool Override::Behaviour::GetOverriddenState<Override::Behaviour::Category::AllowSelection>(int Flags)
{ return (Flags & Override::Behaviour::Category::AllowSelection) > 0; };
template<> inline bool Override::Behaviour::GetOverriddenState<Override::Behaviour::Category::AllowNegativeValues>(int Flags)
{ return (Flags & Override::Behaviour::Category::AllowNegativeValues) > 0; };
template<> inline bool Override::Behaviour::GetOverriddenState<Override::Behaviour::Category::DisplayValue>(int Flags)
{ return (Flags & Override::Behaviour::Category::DisplayValue) > 0; };

template<> inline int Override::Behaviour::OverrideBehaviour<Override::Behaviour::Category::AllowSelection, true>(int Flags)
{ return  ((Flags & 0b11101111) | Category::AllowSelection); };
template<> inline int Override::Behaviour::OverrideBehaviour<Override::Behaviour::Category::AllowNegativeValues, true>(int Flags)
{ return  ((Flags & 0b11011111) | Category::AllowNegativeValues) ; };
template<> inline int Override::Behaviour::OverrideBehaviour<Override::Behaviour::Category::DisplayValue, true>(int Flags)
{ return  ((Flags & 0b10111111) | Category::DisplayValue); };   
template<> inline int Override::Behaviour::OverrideBehaviour <Override::Behaviour::Category::AllowSelection, false>(int Flags)
{ return  (Flags & 0b11101110) ; };
template<> inline int Override::Behaviour::OverrideBehaviour<Override::Behaviour::Category::AllowNegativeValues, false>(int Flags)
{ return  ((Flags & 0b11011101)) ; };
template<> inline int Override::Behaviour::OverrideBehaviour <Override::Behaviour::Category::DisplayValue, false>(int Flags)
{ return  ((Flags & 0b10111011)) ;};   

template<> inline int Override::Behaviour::ResetOverrideState<Override::Behaviour::Category::AllowSelection>(int Flags)
{ return  (Flags & 0b11101111) ;}
template<> inline int Override::Behaviour::ResetOverrideState<Override::Behaviour::Category::AllowNegativeValues>(int Flags)
{ return  (Flags & 0b11011111) ; };
template<> inline int Override::Behaviour::ResetOverrideState<Override::Behaviour::Category::DisplayValue>(int Flags)
{ return  (Flags & 0b10111111) ;};   



// fwd decl.
struct Template;

struct ButtonPayload
{
   ButtonPayload(GridElement& ElementBound)
      : CallingVisualElement(&ElementBound), Target(nullptr), SetOpen(false) {}
   ButtonPayload(GridElement& ElementBound, BaseCharacterEditorClass* BaseTarget)
   : CallingVisualElement(&ElementBound), Target(BaseTarget), SetOpen(false) {}
   ButtonPayload(GridElement& ElementBound, BaseCharacterEditorClass* BaseTarget, bool SetState)
      : CallingVisualElement(&ElementBound), Target(BaseTarget), SetOpen(SetState) {}

   GridElement* CallingVisualElement;
   BaseCharacterEditorClass* Target;
   bool SetOpen;
};
auto DummyStateDelegate = [](const struct ButtonPayload&) -> bool { return false; };
auto DummyCallbackTarget = [](const struct ButtonPayload&) -> void { };
typedef void (*CallbackSpace)(struct ButtonPayload&) ; //= UIHandler::Callbacks::Race::SelectAllowedClass;

typedef struct LinkedWidgetBase
{
   LinkedWidgetBase() = default;
   LinkedWidgetBase(std::vector<LinkedWidgetBase*> WidgetList)
   {
      for (LinkedWidgetBase* LinkedBase : WidgetList)
      {
         LinkWidget(LinkedBase);
      }
   }
   virtual ~LinkedWidgetBase() = default;

   void LinkWidget(LinkedWidgetBase* NewWidget)
   {
      if (NewWidget == nullptr || LinkedWidgets.find(NewWidget) != LinkedWidgets.end()) { return; }

      LinkedWidgets.emplace(NewWidget, NewWidget->IsVisible);
   }
   void RemoveLinkedWidget(LinkedWidgetBase* ToRemoveWidget)
   {
      if (ToRemoveWidget == nullptr || LinkedWidgets.find(ToRemoveWidget) == LinkedWidgets.end()) { return; }

      LinkedWidgets.erase(ToRemoveWidget);
   }

   virtual void PropagateVisibility(bool FlipSelfBefore = false) final
   {
      IsVisible = FlipSelfBefore ? IsVisible == false : IsVisible;
      
      for (std::pair<LinkedWidgetBase* const, bool> WidgetPair : LinkedWidgets)
      {
         WidgetPair.second = IsVisible;
         WidgetPair.first->IsVisible = IsVisible;
      }
   }

   void PropagateState(bool FlipSelfBefore = false)
   {
      AltState = FlipSelfBefore ? AltState == false : AltState;
      
      for (const std::pair<LinkedWidgetBase* const, bool> WidgetPair : LinkedWidgets)
      {
         const bool OldState = WidgetPair.first->AltState;
         WidgetPair.first->AltState = AltState ;
         WidgetPair.first->OnStateUpdate(OldState);
      }
   }

   virtual void OnStateUpdate(bool OldState) = 0;

   static void Paint(LinkedWidgetBase* OptionalBase = nullptr)
   {
      static LinkedWidgetBase* WidgetBase = OptionalBase;
      WidgetBase = OptionalBase != nullptr ? OptionalBase : WidgetBase;

      if (WidgetBase == nullptr) { return;}
      (*WidgetBase)();
   }
   
   virtual void operator()(...) {};
 
   // This macro allows us to override and overload at the same time in subclasses in a portable and legal manner
#define SCOPED_USE_LINKEDWIDGET_NAMESPACE \
using LinkedWidgetBase::operator(); \
using LinkedWidgetBase::LinkedWidgetBase;


   // This macro allows us to override and overload at the same time in subclasses in a portable and legal manner
#define SCOPED_USE_MAKE_NOT_ABSTRACT \
void operator()(...) final {};
   
public:

   bool IsVisible = true;
   bool AltState = true;
protected:
   std::map<LinkedWidgetBase*, bool> LinkedWidgets;
} linked_widget_base_t;

typedef struct LinkedButton final : public LinkedWidgetBase
{
   SCOPED_USE_LINKEDWIDGET_NAMESPACE
   SCOPED_USE_MAKE_NOT_ABSTRACT
   
   virtual void operator()(const char* Label, const ImVec2& Size, ...) final
   {
      if (IsVisible == false) { return; } 

      if (ImGui::Button(Label, Size))
      {
         AltState = AltState == false; // flip
         PropagateState();
      }
   }

   virtual void OnStateUpdate(bool OldState) {}
   
} linked_button_t;

struct LinkedText final : public LinkedWidgetBase
{
   SCOPED_USE_LINKEDWIDGET_NAMESPACE
   SCOPED_USE_MAKE_NOT_ABSTRACT
   
   virtual void operator()(const char* fmt, ...) final
   {
      if (IsVisible == false) { return; }
      
      int result;
      va_list list{};
      va_start(list, fmt);
      ImGui::TextV(fmt, list);
      printf("\n");
      va_end(list);
   }
   virtual void OnStateUpdate(bool OldState) {}
};

class UIHandler
{
public:

   typedef struct Callbacks
   {
      static bool SelectInclusive(ButtonPayload& ButtonPayload);
      static void SelectExclusive(ButtonPayload& ButtonPayload);
      typedef struct Class
      {
         static void SelectPrimeStat(ButtonPayload& ButtonPayload);
         static void SelectHitDie(ButtonPayload& ButtonPayload);
      }class_t;

      typedef struct Race
      {
         static void SelectMinimumStat(ButtonPayload& ButtonPayload);
         static void SelectModifier(ButtonPayload& ButtonPayload);
         static void SelectAllowedClass(ButtonPayload& ButtonPayload);
      }race_t;

      typedef struct Character
      {
         static void SelectRace(ButtonPayload& ButtonPayload);
         static void SelectClass(ButtonPayload& ButtonPayload);
         static void SelectAlignment(ButtonPayload& ButtonPayload);
         static void UpdateStatsEntry(ButtonPayload& ButtonPayload);
         static void SelectStat(ButtonPayload& ButtonPayload);
      }character_t;

      typedef struct CharacterPicker
      {
         static void SelectCharacter(ButtonPayload& ButtonPayload);
         static DnDCharacter& StowedCharacterForPicker(DnDCharacter* PotentialOverride = nullptr)
         {
            static DnDCharacter Character{INVALID_INDEX};
            if (PotentialOverride != nullptr) { Character = *PotentialOverride; }
            return Character;
         };
      }characterpicker_t;
   }callbacks_t;
   
   static void DeinitUI();

   static std::string GetCharacterEditorStateLabel(bool& State);
   static std::string GetClassEditorStateLabel(bool& State);
   static std::string GetRaceEditorStateLabel(bool& State);
   static std::string GetCharacterSelectorStateLabel(bool& State);
   static void DrawAppMainMenuBar();
   static void ProcessGridElement(Template* Category, GridElement& Element);
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

   template<class TType = CharacterClass, CallbackSpace TCallbackSpace = Callbacks::Race::SelectAllowedClass>
   static void RequestNewTemplateEntryForDataWithID(Template* TGroup, std::string FourCharacterPrefix, const TType& ClassData);
   static void RemoveGridElement(const GridElement* Consider, Template* TGroup);

   template<class TType = CharacterClass, class IteratorType, 
      CallbackSpace TCallbackSpace = Callbacks::Race::SelectAllowedClass>
   static void UpdateGridElements(
      Template* Group,
      std::map<int, TType>& MappedGroup,
      std::string FourCharacterPrefix,
      std::deque<const GridElement*> RemovalDeque);
   
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

   // These maps will trigger deallocation errors if manages as member variables,
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

   
   //
   //
   // static std::map<Template*, GridElement*> LastSelectedElementPerTemplate;
   // static std::map<GridElement*, Template*> LastSelectedElementPerTemplate_BackMapping;
   // static std::map<Template*, GridElement*> CurrentSelectedElementPerTemplate;
   // static std::map<GridElement*, Template*> CurrentSelectedElementPerTemplate_BackMapping;
   
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
      std::function<bool(ButtonPayload&)> InStateDelegate = {},
      std::function<void(ButtonPayload&)> InCallbackTarget = {},
      std::function<void(ButtonPayload&)> OnRefreshFunction = {}
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
        , OnRefresh(OnRefreshFunction)
   {
   }

   bool operator==(const GridElement& Other) const
   {
      return this->Label == Other.Label; // we only care about the label
   }
   bool operator!=(const GridElement& Other) const
   {
      return (*this == Other) == false;
   }
   bool operator<(const GridElement& Other) const
   {
      constexpr std::hash<std::string> StringHasher;
      return StringHasher(this->Label) < StringHasher(Other.Label);
   }
   
   std::string Label{"DefaultLabel"};
   ElementStyle Style               = {};
   bool         IsSelected          = false;
   int          CurrentValue        = 0;
   int          EnumSelectorValue   = 0;
   bool         AllowNegativeValues = false;
   bool         AllowSelection      = true;
   bool         DisplayValue        = true;

   std::function<bool(ButtonPayload&)> GetStateDelegate;
   std::function<void(ButtonPayload&)> CallbackTarget;
   std::function<void(ButtonPayload&)> OnRefresh;
};

typedef struct Template
{
   using  TemplateIt = const std::_Tree_const_iterator<std::_Tree_val<std::_Tree_simple_types<GridElement>>>;
   inline bool DoesGridElementExist(const GridElement& Other)
   {
      return Inner.find(Other) != Inner.end();
   }
   inline void Emplace(const GridElement& Consider)
   {
      Inner.emplace(Consider);
   }
   inline void Remove(const GridElement& Consider)
   {
      Inner.erase(Consider);
   }

   
   std::set<GridElement> Inner;

   ImVec2 MaxSize {-1, -1}; // {x,y} Negative one indicates there is no limit 
   int SharedSettingsFlags = 0b01110000;
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
            GridElement{StrLabel, DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Strength, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectPrimeStat , DummyCallbackTarget},
            GridElement{IntLabel, DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Intelligence, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectPrimeStat, DummyCallbackTarget},
            GridElement{WisLabel, DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Wisdom, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectPrimeStat, DummyCallbackTarget},
            GridElement{DexLabel, DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Dexterity, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectPrimeStat, DummyCallbackTarget},
            GridElement{ConLabel, DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Constitution, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectPrimeStat, DummyCallbackTarget},
            GridElement{ChaLabel, DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Charisma, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectPrimeStat, DummyCallbackTarget}
         }
      };

      static const std::string D4Label = ("ps::1d4");
      static const std::string D6Label = ("ps::1d6");
      static const std::string D8Label = ("ps::1d8");
      static const std::string D12Label = ("ps::1d12");
      static const std::string D20Label = ("ps::1d20");
      static Template PlatonicDiceTemplates{
         {
            GridElement{D4Label, DefaultStyle, INVALID_INDEX, Platonic::Dice::E4D,  false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectHitDie, DummyCallbackTarget},
            GridElement{D6Label, DefaultStyle, INVALID_INDEX, Platonic::Dice::E6D,  false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectHitDie, DummyCallbackTarget},
            GridElement{D8Label, DefaultStyle, INVALID_INDEX, Platonic::Dice::E8D,  false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectHitDie, DummyCallbackTarget},
            GridElement{D12Label, DefaultStyle, INVALID_INDEX, Platonic::Dice::E12D, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectHitDie, DummyCallbackTarget},
            GridElement{D20Label, DefaultStyle, INVALID_INDEX,  Platonic::Dice::E20D, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Class::SelectHitDie, DummyCallbackTarget}
         }
      };
   }


   namespace Race
   {
      // Potentially selectable classes, gets modified each step, 
      static Template ClassTemplates;

      
      static const std::string msStrLabel = ("ms::STR");
      static const std::string msIntLabel = ("ms::INT");
      static const std::string msWisLabel = ("ms::WIS");
      static const std::string msDexLabel = ("ms::DEX");
      static const std::string msConLabel = ("ms::CON");
      static const std::string msChaLabel = ("ms::CHA");

      static Template MinimumStatsTemplates{
         {
            GridElement{msStrLabel, DefaultStyle, 0, Spec::Character::StatType::Strength, false, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectMinimumStat, DummyCallbackTarget},
            GridElement{msIntLabel, DefaultStyle, 0, Spec::Character::StatType::Intelligence, false, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectMinimumStat, DummyCallbackTarget},
            GridElement{msWisLabel, DefaultStyle, 0, Spec::Character::StatType::Wisdom, false, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectMinimumStat, DummyCallbackTarget},
            GridElement{msDexLabel, DefaultStyle, 0, Spec::Character::StatType::Dexterity, false, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectMinimumStat, DummyCallbackTarget},
            GridElement{msConLabel, DefaultStyle, 0, Spec::Character::StatType::Constitution, false, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectMinimumStat, DummyCallbackTarget},
            GridElement{msChaLabel, DefaultStyle, 0, Spec::Character::StatType::Charisma, false, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectMinimumStat, DummyCallbackTarget}
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
            GridElement{mfStrLabel, DefaultStyle, 0, Spec::Character::StatType::Strength, true, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectModifier, DummyCallbackTarget},
            GridElement{mfIntLabel, DefaultStyle, 0, Spec::Character::StatType::Intelligence, true, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectModifier, DummyCallbackTarget},
            GridElement{mfWisLabel, DefaultStyle, 0, Spec::Character::StatType::Wisdom, true, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectModifier, DummyCallbackTarget},
            GridElement{mfDexLabel, DefaultStyle, 0, Spec::Character::StatType::Dexterity, true, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectModifier, DummyCallbackTarget},
            GridElement{mfConLabel, DefaultStyle, 0, Spec::Character::StatType::Constitution, true, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectModifier, DummyCallbackTarget},
            GridElement{mfChaLabel, DefaultStyle, 0, Spec::Character::StatType::Charisma, true, true, true, DummyStateDelegate, &UIHandler::Callbacks::Race::SelectModifier, DummyCallbackTarget}
         }
      };      
   }


   static constexpr ImVec2 CharacterEditorPadding = {20, 20};
   static constexpr ImVec2 CharacterEditorSizes = {120, 40};
   static constexpr ElementStyle CharacterEditorStyle = {CharacterEditorSizes, CharacterEditorPadding, ImTextureID{}};
   namespace Character
   {
      // Potentially selectable characters, gets modified each step, 
      static Template LoadedCharacterTemplates;      
            
      // Potentially selectable classes, gets modified each step, 
      static Template ClassTemplates;
      
      // Potentially selectable races, gets modified each step, 
      static Template RaceTemplates;
      
      static const std::string chStrLabel = ("ch::STR");
      static const std::string chIntLabel = ("ch::INT");
      static const std::string chWisLabel = ("ch::WIS");
      static const std::string chDexLabel = ("ch::DEX");
      static const std::string chConLabel = ("ch::CON");
      static const std::string chChaLabel = ("ch::CHA");      
      static Template CharacterStatTemplates{
         {
            GridElement{chStrLabel, DefaultStyle, 0, Spec::Character::StatType::Strength, true, false, true, DummyStateDelegate,&UIHandler::Callbacks::Character::SelectStat, &UIHandler::Callbacks::Character::UpdateStatsEntry},
            GridElement{chIntLabel, DefaultStyle, 0, Spec::Character::StatType::Intelligence, true, false, true, DummyStateDelegate,&UIHandler::Callbacks::Character::SelectStat, &UIHandler::Callbacks::Character::UpdateStatsEntry},
            GridElement{chWisLabel, DefaultStyle, 0, Spec::Character::StatType::Wisdom, true, false, true, DummyStateDelegate,&UIHandler::Callbacks::Character::SelectStat, &UIHandler::Callbacks::Character::UpdateStatsEntry},
            GridElement{chDexLabel, DefaultStyle, 0, Spec::Character::StatType::Dexterity, true, false, true, DummyStateDelegate,&UIHandler::Callbacks::Character::SelectStat, &UIHandler::Callbacks::Character::UpdateStatsEntry},
            GridElement{chConLabel, DefaultStyle, 0, Spec::Character::StatType::Constitution, true, false, true, DummyStateDelegate,&UIHandler::Callbacks::Character::SelectStat, &UIHandler::Callbacks::Character::UpdateStatsEntry},
            GridElement{chChaLabel, DefaultStyle, 0, Spec::Character::StatType::Charisma, true, false, true, DummyStateDelegate,&UIHandler::Callbacks::Character::SelectStat, &UIHandler::Callbacks::Character::UpdateStatsEntry}
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
            GridElement{agLawGoodLabel,        CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::LawGood, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment, DummyCallbackTarget},
            GridElement{agLawNeutralLabel,     CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::LawNeutral, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment, DummyCallbackTarget},
            GridElement{agLawEvilLabel,        CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::LawEvil, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment, DummyCallbackTarget},
            GridElement{agNeutralGoodLabel,    CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::NeutralGood, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment, DummyCallbackTarget},
            GridElement{agNeutralNeutralLabel, CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::NeutralNeutral, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment, DummyCallbackTarget},
            GridElement{agNeutralEvilLabel,    CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::NeutralEvil, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment, DummyCallbackTarget},
            GridElement{agChaoticGoodLabel,    CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::ChaosGood, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment, DummyCallbackTarget},
            GridElement{agChaoticNeutralLabel, CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::ChaosNeutral, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment, DummyCallbackTarget},
            GridElement{agChaoticEvilLabel,    CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::ChaosEvil, false, true, false, DummyStateDelegate, &UIHandler::Callbacks::Character::SelectAlignment, DummyCallbackTarget}
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
      // static const std::string LoadClassLabel = ("cl::Load Class Config @todo Finish");
      static const std::string CloseClassLabel = ("cl::Close Class Editor");
      static Template ClassMenuControlTemplates{
         {
            GridElement{SaveClassLabel, MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, DummyStateDelegate, &UIHandler::SaveClass, DummyCallbackTarget},
            GridElement{ CloseClassLabel, MenuStyle, INVALID_INDEX, INVALID_INDEX,  false, true, false, &UIHandler::GetIsClassEditorOpen, &UIHandler::SetClassEditorOpen, DummyCallbackTarget} 
         }
      };

      static const std::string SaveRaceLabel = ("ra::Save Race Config @todo Finish");
      // static const std::string LoadRaceLabel = ("ra::Load Race Config @todo Finish");
      static const std::string CloseRaceLabel = ("ra::Close Race Editor");
      static Template RaceMenuControlTemplates{
         {
            GridElement{SaveRaceLabel, MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, DummyStateDelegate, &UIHandler::SaveRace, DummyCallbackTarget},
            GridElement{CloseRaceLabel, MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, &UIHandler::GetIsRaceEditorOpen, &UIHandler::SetRaceEditorOpen, DummyCallbackTarget}
         }
      };


      static const std::string SaveCharacterLabel = ("ch::Save Character Config @todo Finish");
      static const std::string CloseCharacterLabel = ("ch::Close Character Editor");
      static Template CharacterMenuControlTemplates{
         {
            GridElement{SaveCharacterLabel, MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, DummyStateDelegate, &UIHandler::SaveCharacter, DummyCallbackTarget} ,
            GridElement{CloseCharacterLabel, MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, &UIHandler::GetIsCharacterEditorOpen, &UIHandler::SetCharacterEditorOpen, DummyCallbackTarget}
         }
      };

      static const std::string LoadCharacterLabel = ("ch::Load Character Config @todo Finish");
      static const std::string CloseCharacterSelectorLabel = ("ch::Close Character Selector");
      static Template CharacterSelectorControlTemplates{
            {
               GridElement{LoadCharacterLabel, MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, DummyStateDelegate, &UIHandler::LoadCharacter, DummyCallbackTarget},
               GridElement{CloseCharacterLabel, MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, &UIHandler::GetIsCharacterSelectorOpen, &UIHandler::SetCharacterSelectorOpen, DummyCallbackTarget}
            }
      };      
      
   }
}

struct LinkedStatCategory : public LinkedWidgetBase
{
   SCOPED_USE_LINKEDWIDGET_NAMESPACE
   SCOPED_USE_MAKE_NOT_ABSTRACT

   LinkedStatCategory(std::vector<LinkedWidgetBase*> Widgets, Template* Inner)
      : linked_widget_base_t(Widgets)
   {
      LinkInner(Inner);
   }
   
   virtual void operator()() final
   {
      UIHandler::DrawTemplates(InnerTemplate);
   }
   virtual void OnStateUpdate(bool OldState) final
   {
      InnerTemplate->SharedSettingsFlags = 
      AltState == false
         ? Override::Behaviour::OverrideBehaviour<Override::Behaviour::Category::AllowSelection, true>(InnerTemplate->SharedSettingsFlags)
         : Override::Behaviour::OverrideBehaviour<Override::Behaviour::Category::AllowSelection, false>(InnerTemplate->SharedSettingsFlags);
   }

   void LinkInner(Template* NewInner) { InnerTemplate = NewInner;}
   
   struct Template* InnerTemplate = nullptr;
};


#endif // UI_H
