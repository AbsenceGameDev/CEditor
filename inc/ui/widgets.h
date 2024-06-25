#pragma once
#ifndef WIDGETSUI_H
#define WIDGETSUI_H

#include "imgui.h"
#include <d3d11.h>
#include <functional>
#include <map>
#include <set>
#include <vector>

#include "shared.h"
#include "../editor_core.h"
#include "../../inc/dataformat.h"

// fwd decl.
struct Template;

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
#define SCOPED_USE_MAKE_WIDGET_NOT_ABSTRACT \
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
   SCOPED_USE_MAKE_WIDGET_NOT_ABSTRACT
   
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
   SCOPED_USE_MAKE_WIDGET_NOT_ABSTRACT
   
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

// Predefined groups, most of the groups can be constructed at compile-time
namespace UI
{
   static constexpr ImVec2 DefaultPadding = {20, 20};
   static constexpr ImVec2 DefaultSizes = {50, 50};
   static constexpr ElementStyle DefaultStyle = {DefaultSizes, DefaultPadding, ImTextureID{}};

   static const std::string ClassStatLabels[6] = { "ps::STR", "ps::INT", "ps::WIS", "ps::DEX", "ps::CON", "ps::CHA" };
   static const std::string ClassDieLabels[6] = { "ps::1d4", "ps::1d6", "ps::1d8", "ps::1d12", "ps::20" };
   struct Class
   {

      static std::set<GridElement> PrimeStatConstructorList();
      static Template PrimeStatTemplates;
      
      static std::set<GridElement> PlatonicDiceConstructorList();
      static Template PlatonicDiceTemplates;
   };

   static const std::string RaceMinStatLabels[6] = { "ms::STR", "ms::INT", "ms::WIS", "ms::DEX", "ms::CON", "ms::CHA" };
   static const std::string RaceModStatLabels[6] = { "mf::STR", "mf::INT", "mf::WIS", "mf::DEX", "mf::CON", "mf::CHA" };
   struct Race
   {
      // Potentially selectable classes, gets modified each step, 
      static Template ClassTemplates;
      

      static std::set<GridElement> MinimumStatsConstructorList();
      static Template MinimumStatsTemplates;
      
      static std::set<GridElement> ModifierConstructorList();
      static Template ModifierTemplates;      
   };


   static constexpr ImVec2 CharacterEditorPadding = {20, 20};
   static constexpr ImVec2 CharacterEditorSizes = {120, 40};
   static constexpr ElementStyle CharacterEditorStyle = {CharacterEditorSizes, CharacterEditorPadding, ImTextureID{}};

   namespace Control
   {
      enum BaseType : uint8_t { Class = 0, Race, Character };
      enum EditorType : uint8_t { ClassEditor = 0, RaceEditor, CharacterEditor, CharacterSelector };
      
      namespace Menu
      {
         using Save = BaseType;
         using Load = BaseType;
         using Open = EditorType;
         using Close = EditorType;
      }
   }
   
   static const std::string CharacterStatLabels[6] = { "ch::STR", "ch::INT", "ch::WIS", "ch::DEX", "ch::CON", "ch::CHA" };
   static const std::string CharacterAlignmentLabels[9] = {
      "ag::Lawful Good", "ag::Lawful Neutral", "ag::Lawful Evil",
      "ag::Neutral Good", "ag::Neutral Neutral", "ag::Neutral Evil",
      "ag::Chaotic Good", "ag::Chaotic Neutral", "ag::Chaotic Evil"};
   
   struct Character
   {
      // Potentially selectable characters, gets modified each step, 
      static Template LoadedCharacterTemplates;      
            
      // Potentially selectable classes, gets modified each step, 
      static Template ClassTemplates;
      
      // Potentially selectable races, gets modified each step, 
      static Template RaceTemplates;
      
      static std::set<GridElement> CharacterStatConstructorList();
      static Template CharacterStatTemplates;
      
      ImVec2 static MaxSizeLimited3_3;
      static std::set<GridElement> AlignmentConstructorList();
      static Template AlignmentTemplates;
   };


   static constexpr ImVec2 MenuPadding = {15, 15};
   static constexpr ImVec2 MenuSizes = {160, 30};
   static constexpr ElementStyle MenuStyle = {MenuSizes, MenuPadding, ImTextureID{}};
   
   static const std::string MenuSaveLabels[3] = {
         "cl::Save Class Config",
         "ra::Save Race Config",
         "ch::Save Character Config"};

   static const std::string MenuLoadLabels[3] = {
         "ch::Load Class Config @todo",
         "ch::Load Race Config @todo",
         "ch::Load Character"};
   
   static const std::string MenuCloseLabels[4] = {
      "cl::Close Class Editor",
      "cl::Close Race Editor",
      "ch::Close Character Editor",
      "ch::Close Character Selector"};
   
   struct Menu
   {
      static std::set<GridElement> ClassMenuControlConstructorList();
      static Template ClassMenuControlTemplates;

      static std::set<GridElement> RaceMenuControlConstructorList();
      static Template RaceMenuControlTemplates;
      
      static std::set<GridElement> CharacterMenuControlConstructorList();
      static Template CharacterMenuControlTemplates;
      
      static std::set<GridElement> CharacterSelectorControlConstructorList();
      static Template CharacterSelectorControlTemplates;
   };      
      
}

struct LinkedStatCategory : public LinkedWidgetBase
{
   SCOPED_USE_LINKEDWIDGET_NAMESPACE
   SCOPED_USE_MAKE_WIDGET_NOT_ABSTRACT

   LinkedStatCategory(std::vector<LinkedWidgetBase*> Widgets, Template* Inner)
      : linked_widget_base_t(Widgets)
   {
      LinkInner(Inner);
   }
   
   virtual void operator()() final;
   virtual void OnStateUpdate(bool OldState) final;

   void LinkInner(Template* NewInner) { InnerTemplate = NewInner;}
   
   struct Template* InnerTemplate = nullptr;
};


#endif // WIDGETSUI_H
