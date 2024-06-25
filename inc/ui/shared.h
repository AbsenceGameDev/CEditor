#pragma once
#ifndef SHAREDUI_H
#define SHAREDUI_H

#include "imgui.h"
#include <map>

#include "../editor_core.h"
#include "../../inc/ruleset.h"

//
#ifndef DEFAULT_RO5
#define DEFAULT_RO5(ClassName) \
   ClassName() = default; \
   virtual ~ClassName() = default; \
   ClassName(const ClassName& Other) = default; \
   ClassName(const ClassName&& Other) = default; \
   ClassName& operator=(const ClassName& Other) = default; \
   ClassName& operator=(ClassName&& Other) = default;
#endif // DEFAULT_RO5   

#ifndef NO_DEFAULT_RO5
#define NO_DEFAULT_RO5(ClassName) \
   ClassName() {}; \
   virtual ~ClassName() {}; \
   ClassName(const ClassName& Other) {}; \
   ClassName(const ClassName&& Other) {}; \
   ClassName& operator=(const ClassName& Other) {}; \
   ClassName& operator=(ClassName&& Other) {};
#endif // NO_DEFAULT_RO5   

using TemplateElementKeyIt = std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<struct Template* const, struct GridElement*>>>>&&;
using TemplateElementValueIt  = std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<struct GridElement* const,  struct Template*>>>>&&;

static int Dummy[MAX_ALLOWED_CLASSES] = {INVALID_INDEX};

// fwd decl.
struct Template;

auto DummyStateDelegate = [](const struct ButtonPayload&) -> bool { return false; };
auto DummyCallbackTarget = [](const struct ButtonPayload&) -> void { };
typedef void (*CallbackSpace)(struct ButtonPayload&) ; //= UIHandler::Callbacks::Race::SelectAllowedClass;

struct ElementStyle
{
   ImVec2 ElementSizes = {50, 50};
   ImVec2 Padding = {20, 20};
   ImTextureID ImageID{};
};

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

namespace UI
{
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
}




#define CtorAnyOrder_Force1Param(_0, _1, _2, _3, _4, _5, _6, _7)  \
         ( \
            const S##_0& In##_0, \
            const S##_1& In##_1 = S##_1{}, \
            const S##_2& In##_2 = S##_2{}, \
            const S##_3& In##_3 = S##_3{}, \
            const S##_4& In##_4 = S##_4{}, \
            const S##_5& In##_5 = S##_5{}, \
            const S##_6& In##_6 = S##_6{}, \
            const S##_7& In##_7 = S##_7{})

#define DefineShuffledCtors(Compound, _0, _1, _2, _3, _4, _5, _6, _7)  \
         Compound CtorAnyOrder_Force1Param(_0, _1, _2, _3, _4, _5, _6, _7) : _0(In##_0), _1(In##_1), _2(In##_2), _3(In##_3), _4(In##_4), _5(In##_5), _6(In##_6), _7(In##_7) {} \
         Compound CtorAnyOrder_Force1Param(_1, _2, _3, _4, _5, _6, _7, _0) : _0(In##_0), _1(In##_1), _2(In##_2), _3(In##_3), _4(In##_4), _5(In##_5), _6(In##_6), _7(In##_7) {} \
         Compound CtorAnyOrder_Force1Param(_2, _3, _4, _5, _6, _7, _0, _1) : _0(In##_0), _1(In##_1), _2(In##_2), _3(In##_3), _4(In##_4), _5(In##_5), _6(In##_6), _7(In##_7) {} \
         Compound CtorAnyOrder_Force1Param(_3, _4, _5, _6, _7, _0, _1, _2) : _0(In##_0), _1(In##_1), _2(In##_2), _3(In##_3), _4(In##_4), _5(In##_5), _6(In##_6), _7(In##_7) {} \
         Compound CtorAnyOrder_Force1Param(_4, _5, _6, _7, _0, _1, _2, _3) : _0(In##_0), _1(In##_1), _2(In##_2), _3(In##_3), _4(In##_4), _5(In##_5), _6(In##_6), _7(In##_7) {} \
         Compound CtorAnyOrder_Force1Param(_5, _6, _7, _0, _1, _2, _3, _4) : _0(In##_0), _1(In##_1), _2(In##_2), _3(In##_3), _4(In##_4), _5(In##_5), _6(In##_6), _7(In##_7) {} \
         Compound CtorAnyOrder_Force1Param(_6, _7, _0, _1, _2, _3, _4, _5) : _0(In##_0), _1(In##_1), _2(In##_2), _3(In##_3), _4(In##_4), _5(In##_5), _6(In##_6), _7(In##_7) {} \
         Compound CtorAnyOrder_Force1Param(_7, _0, _1, _2, _3, _4, _5, _6) : _0(In##_0), _1(In##_1), _2(In##_2), _3(In##_3), _4(In##_4), _5(In##_5), _6(In##_6), _7(In##_7) {}           


#endif // SHAREDUI_H
