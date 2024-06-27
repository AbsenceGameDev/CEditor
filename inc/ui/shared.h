#pragma once
#ifndef SHAREDUI_H
#define SHAREDUI_H

#include "imgui.h"
#include <map>

#include "../editor_core.h"
#include "../../inc/ruleset.h"

constexpr ImGuiInputTextFlags SharedInputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll |
      ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;

#define MakeTree(str_literal, scoped_payload) \
ImGui::NewLine(); \
if (ImGui::TreeNode(str_literal)) \
{\
\
   scoped_payload \
   ImGui::TreePop(); \
}

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
   ClassName& operator=(const ClassName& Other) { return *this; }; \
   ClassName& operator=(ClassName&& Other) { return *this; };
#endif // NO_DEFAULT_RO5


#define IsValidVec2(Vec2) (Vec2).x >= (0.0f - FLT_EPSILON) && (Vec2).y >= (0.0f - FLT_EPSILON)
#define IsValidFloat(float) (float) >= (0.0f - FLT_EPSILON)
#define PushStyleV(PopCount, Flag, Input) (PopCount)++; ImGui::PushStyleVar(Flag, Input)
#define PushIf(Cond, PopCount, Flag, Input)  if (Cond) { PushStyleV(PopCount, Flag, Input); }


// non portable macro hackery
// // do not use, does not expand properly on msvc right now, but expands properly by resharper
// (so it can be used as an intermediary step for quickly writing 'Rule of 5' compatible types which have members with uniform data-type
#define CONCAT_HELPER(In, Token) In##Token
#define TOKEN_SEPARATOR(Token) Token
#define STEAL_TOKEN_(MemberType, FirstToken) this->TOKEN_SEPARATOR(FirstToken) = Other.TOKEN_SEPARATOR(FirstToken);
#define STEAL_TOKEN_1_(MemberType, FirstToken) STEAL_TOKEN_(MemberType, FirstToken)
#define STEAL_TOKEN_2_(MemberType, FirstToken, ...)  STEAL_TOKEN_1_(MemberType, __VA_ARGS__) STEAL_TOKEN_(MemberType, FirstToken)
#define STEAL_TOKEN_3_(MemberType, FirstToken, ...)  STEAL_TOKEN_2_(MemberType, __VA_ARGS__) STEAL_TOKEN_(MemberType, FirstToken)
#define STEAL_TOKEN_4_(MemberType, FirstToken, ...)  STEAL_TOKEN_3_(MemberType, __VA_ARGS__) STEAL_TOKEN_(MemberType, FirstToken)
#define STEAL_TOKEN_5_(MemberType, FirstToken, ...)  STEAL_TOKEN_4_(MemberType, __VA_ARGS__) STEAL_TOKEN_(MemberType, FirstToken)
#define STEAL_TOKEN_6_(MemberType, FirstToken, ...)  STEAL_TOKEN_5_(MemberType, __VA_ARGS__) STEAL_TOKEN_(MemberType, FirstToken)
#define STEAL_TOKEN_7_(MemberType, FirstToken, ...)  STEAL_TOKEN_6_(MemberType, __VA_ARGS__) STEAL_TOKEN_(MemberType, FirstToken)
#define STEAL_TOKEN_8_(MemberType, FirstToken, ...)  STEAL_TOKEN_7_(MemberType, __VA_ARGS__) STEAL_TOKEN_(MemberType, FirstToken)
#define STEAL_TOKEN_9_(MemberType, FirstToken, ...)  STEAL_TOKEN_8_(MemberType, __VA_ARGS__) STEAL_TOKEN_(MemberType, FirstToken)
#define STEAL_TOKEN_10_(MemberType, FirstToken, ...) STEAL_TOKEN_9_(MemberType, __VA_ARGS__) STEAL_TOKEN_(MemberType, FirstToken)
#define STEAL_TOKEN_N_BASE(N) STEAL_TOKEN_##N##_
#define STEAL_TOKEN_N_REROUTE(MemberType, N, ...) STEAL_TOKEN_N_BASE(N)(MemberType, __VA_ARGS__)

#define BUILD_MEMBER_LIST_(UniformValue, MemberType, FirstToken) FirstToken = {UniformValue}; 
#define BUILD_MEMBER_LIST_1_(UniformValue, MemberType, FirstToken)  MemberType BUILD_MEMBER_LIST_(UniformValue, MemberType, FirstToken)
#define BUILD_MEMBER_LIST_2_(UniformValue, MemberType, FirstToken, ...)  BUILD_MEMBER_LIST_1_(UniformValue, MemberType, __VA_ARGS__) MemberType BUILD_MEMBER_LIST_(UniformValue, MemberType, FirstToken)
#define BUILD_MEMBER_LIST_3_(UniformValue, MemberType, FirstToken, ...)  BUILD_MEMBER_LIST_2_(UniformValue, MemberType, __VA_ARGS__) MemberType BUILD_MEMBER_LIST_(UniformValue, MemberType, FirstToken)
#define BUILD_MEMBER_LIST_4_(UniformValue, MemberType, FirstToken, ...)  BUILD_MEMBER_LIST_3_(UniformValue, MemberType, __VA_ARGS__) MemberType BUILD_MEMBER_LIST_(UniformValue, MemberType, FirstToken)
#define BUILD_MEMBER_LIST_5_(UniformValue, MemberType, FirstToken, ...)  BUILD_MEMBER_LIST_4_(UniformValue, MemberType, __VA_ARGS__) MemberType BUILD_MEMBER_LIST_(UniformValue, MemberType, FirstToken)
#define BUILD_MEMBER_LIST_6_(UniformValue, MemberType, FirstToken, ...)  BUILD_MEMBER_LIST_5_(UniformValue, MemberType, __VA_ARGS__) MemberType BUILD_MEMBER_LIST_(UniformValue, MemberType, FirstToken)
#define BUILD_MEMBER_LIST_7_(UniformValue, MemberType, FirstToken, ...)  BUILD_MEMBER_LIST_6_(UniformValue, MemberType, __VA_ARGS__) MemberType BUILD_MEMBER_LIST_(UniformValue, MemberType, FirstToken)
#define BUILD_MEMBER_LIST_8_(UniformValue, MemberType, FirstToken, ...)  BUILD_MEMBER_LIST_7_(UniformValue, MemberType, __VA_ARGS__) MemberType BUILD_MEMBER_LIST_(UniformValue, MemberType, FirstToken)
#define BUILD_MEMBER_LIST_9_(UniformValue, MemberType, FirstToken, ...)  BUILD_MEMBER_LIST_8_(UniformValue, MemberType, __VA_ARGS__) MemberType BUILD_MEMBER_LIST_(UniformValue, MemberType, FirstToken)
#define BUILD_MEMBER_LIST_10_(UniformValue, MemberType, FirstToken, ...) BUILD_MEMBER_LIST_9_(UniformValue, MemberType, __VA_ARGS__) MemberType BUILD_MEMBER_LIST_(UniformValue, MemberType, FirstToken)
#define BUILD_MEMBER_LIST_N_BASE(N) BUILD_MEMBER_LIST_##N##_
#define BUILD_MEMBER_LIST_N_REROUTE(UniformValue, MemberType, N, ...) BUILD_MEMBER_LIST_N_BASE(N)(MemberType, __VA_ARGS__)

#define PS_TOKEN_(MemberType, FirstToken) CONCAT_HELPER(In, FirstToken)
#define PS_TOKEN_1_(MemberType, FirstToken)  MemberType PS_TOKEN_(MemberType, FirstToken)
#define PS_TOKEN_2_(MemberType, FirstToken, ...)  PS_TOKEN_1_(MemberType, __VA_ARGS__) , MemberType PS_TOKEN_(MemberType, FirstToken)
#define PS_TOKEN_3_(MemberType, FirstToken, ...)  PS_TOKEN_2_(MemberType, __VA_ARGS__) , MemberType PS_TOKEN_(MemberType, FirstToken)
#define PS_TOKEN_4_(MemberType, FirstToken, ...)  PS_TOKEN_3_(MemberType, __VA_ARGS__) , MemberType PS_TOKEN_(MemberType, FirstToken)
#define PS_TOKEN_5_(MemberType, FirstToken, ...)  PS_TOKEN_4_(MemberType, __VA_ARGS__) , MemberType PS_TOKEN_(MemberType, FirstToken)
#define PS_TOKEN_6_(MemberType, FirstToken, ...)  PS_TOKEN_5_(MemberType, __VA_ARGS__) , MemberType PS_TOKEN_(MemberType, FirstToken)
#define PS_TOKEN_7_(MemberType, FirstToken, ...)  PS_TOKEN_6_(MemberType, __VA_ARGS__) , MemberType PS_TOKEN_(MemberType, FirstToken)
#define PS_TOKEN_8_(MemberType, FirstToken, ...)  PS_TOKEN_7_(MemberType, __VA_ARGS__) , MemberType PS_TOKEN_(MemberType, FirstToken)
#define PS_TOKEN_9_(MemberType, FirstToken, ...)  PS_TOKEN_8_(MemberType, __VA_ARGS__) , MemberType PS_TOKEN_(MemberType, FirstToken)
#define PS_TOKEN_10_(MemberType, FirstToken, ...) PS_TOKEN_9_(MemberType, __VA_ARGS__) , MemberType PS_TOKEN_(MemberType, FirstToken)
#define PS_TOKEN_N_BASE(N) PS_TOKEN_##N##_
#define PS_TOKEN_N_REROUTE(MemberType, N, ...) PS_TOKEN_N_BASE(N)(MemberType, __VA_ARGS__)

#define ST_TOKEN_(MemberType, FirstToken) TOKEN_SEPARATOR(FirstToken) (CONCAT_HELPER(In, FirstToken))
#define ST_TOKEN_1_(MemberType, FirstToken) ST_TOKEN_(MemberType, FirstToken)
#define ST_TOKEN_2_(MemberType, FirstToken, ...)  ST_TOKEN_1_(MemberType, __VA_ARGS__) , ST_TOKEN_(MemberType, FirstToken)
#define ST_TOKEN_3_(MemberType, FirstToken, ...)  ST_TOKEN_2_(MemberType, __VA_ARGS__) , ST_TOKEN_(MemberType, FirstToken)
#define ST_TOKEN_4_(MemberType, FirstToken, ...)  ST_TOKEN_3_(MemberType, __VA_ARGS__) , ST_TOKEN_(MemberType, FirstToken)
#define ST_TOKEN_5_(MemberType, FirstToken, ...)  ST_TOKEN_4_(MemberType, __VA_ARGS__) , ST_TOKEN_(MemberType, FirstToken)
#define ST_TOKEN_6_(MemberType, FirstToken, ...)  ST_TOKEN_5_(MemberType, __VA_ARGS__) , ST_TOKEN_(MemberType, FirstToken)
#define ST_TOKEN_7_(MemberType, FirstToken, ...)  ST_TOKEN_6_(MemberType, __VA_ARGS__) , ST_TOKEN_(MemberType, FirstToken)
#define ST_TOKEN_8_(MemberType, FirstToken, ...)  ST_TOKEN_7_(MemberType, __VA_ARGS__) , ST_TOKEN_(MemberType, FirstToken)
#define ST_TOKEN_9_(MemberType, FirstToken, ...)  ST_TOKEN_8_(MemberType, __VA_ARGS__) , ST_TOKEN_(MemberType, FirstToken)
#define ST_TOKEN_10_(MemberType, FirstToken, ...) ST_TOKEN_9_(MemberType, __VA_ARGS__) , ST_TOKEN_(MemberType, FirstToken)
#define ST_TOKEN_N_BASE(N) ST_TOKEN_##N##_
#define ST_TOKEN_N_REROUTE(MemberType, N, ...) ST_TOKEN_N_BASE(N)(MemberType, __VA_ARGS__)

#define UNIFORM_ARG_(FirstToken) {FirstToken}
#define UNIFORM_ARG_1_(FirstToken, ...) UNIFORM_ARG_ (FirstToken)
#define UNIFORM_ARG_2_(FirstToken, ...) UNIFORM_ARG_1_(FirstToken) , UNIFORM_ARG_(FirstToken)
#define UNIFORM_ARG_3_(FirstToken, ...) UNIFORM_ARG_2_(FirstToken) , UNIFORM_ARG_(FirstToken)
#define UNIFORM_ARG_4_(FirstToken, ...) UNIFORM_ARG_3_(FirstToken) , UNIFORM_ARG_(FirstToken)
#define UNIFORM_ARG_5_(FirstToken, ...) UNIFORM_ARG_4_(FirstToken) , UNIFORM_ARG_(FirstToken)
#define UNIFORM_ARG_6_(FirstToken, ...) UNIFORM_ARG_5_(FirstToken) , UNIFORM_ARG_(FirstToken)
#define UNIFORM_ARG_7_(FirstToken, ...) UNIFORM_ARG_6_(FirstToken) , UNIFORM_ARG_(FirstToken)
#define UNIFORM_ARG_8_(FirstToken, ...) UNIFORM_ARG_7_(FirstToken) , UNIFORM_ARG_(FirstToken)
#define UNIFORM_ARG_9_(FirstToken, ...) UNIFORM_ARG_8_(FirstToken) , UNIFORM_ARG_(FirstToken)
#define UNIFORM_ARG_10_(FirstToken,...) UNIFORM_ARG_9_(FirstToken) , UNIFORM_ARG_(FirstToken)
#define UNIFORM_ARG_START_(N, FirstToken) UNIFORM_ARG_##N##_(FirstToken)

#define UNIFORM_ARGS_ANY(N, FirstToken) UNIFORM_ARG_START_(N, FirstToken)
#define EMPTY_ARGS_ANY(N) UNIFORM_ARG_START_(N, /**/)


#define Ctor_Base_Override(Type, Parent) \
Type() {} \
virtual ~Type() override = default; \
Type(const Type& Other) : Parent(Other) { *this = Other; } \
Type(const Type&& Other) noexcept { *this = Other; }

#define Ctor_Base_NoOverride(Type) \
Type() {} \
virtual ~Type() = default; \
Type(const Type& Other) { *this = Other; } \
Type(const Type&& Other) noexcept { *this = Other; }

#define Ctor_N_UniformParams_WithoutPragma(Type, UniformValue, MemberTypes,  N, ...) \
Ctor_Base_Override(Type, StyleBase) \
Type(PS_TOKEN_N_BASE(N)(MemberTypes, __VA_ARGS__)) \
   : ST_TOKEN_N_BASE(N)(MemberTypes, __VA_ARGS__) {} \
Type& operator=(const Type& Other) { STEAL_TOKEN_N_BASE(N)(MemberTypes, __VA_ARGS__) return *this; } \
Type& operator=(Type&& Other) noexcept { STEAL_TOKEN_N_BASE(N)(MemberTypes, __VA_ARGS__) return *this; } \
BUILD_MEMBER_LIST_N_BASE(N)(UniformValue, MemberTypes, __VA_ARGS__) 

#define Ctor_N_UniformParams(Type, UniformValue, MemberTypes,  N, ...) \
_##Pra##gma("EXPANDED_MACRO")  \
Ctor_N_UniformParams_WithoutPragma(Type, UniformValue, MemberTypes,  N, __VA_ARGS__) \
_##Pra##gma("EXPANDED_MACRO")

using TemplateElementKeyIt = std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<struct Template* const, struct GridElement*>>>>&&;
using TemplateElementValueIt  = std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<struct GridElement* const,  struct Template*>>>>&&;

static int DummyAllowedClasses[MAX_ALLOWED_CLASSES] = {INVALID_INDEX};

// fwd decl.
struct Template;

auto DummyStateDelegate = [](const struct ButtonPayload&) -> bool { return false; };
auto DummyCallbackTarget = [](const struct ButtonPayload&) -> void { };
typedef void (*CallbackSpace)(struct ButtonPayload&) ; //= UIHandler::Callbacks::Race::SelectAllowedClass;

// Just extending vec2 so we can have uniform initializers
struct PDVec2 : ImVec2
{
   PDVec2() : ImVec2(-1.f,-1.f) {}
   PDVec2(const ImVec2& Other) : ImVec2(Other) {}
   PDVec2(float x, float y) : ImVec2(x, y) {}
   PDVec2(float f) : ImVec2(f,f) {}
   static ImVec2 CreateUniform(float f) { return ImVec2(f,f); }
};


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




#define CtorAnyOrder_Force1Param(_0, _1, _2, _3, _4, _5, _6, _7, _8)  \
         ( \
            const S##_0& In##_0, \
            const S##_1& In##_1 = S##_1{}, \
            const S##_2& In##_2 = S##_2{}, \
            const S##_3& In##_3 = S##_3{}, \
            const S##_4& In##_4 = S##_4{}, \
            const S##_5& In##_5 = S##_5{}, \
            const S##_6& In##_6 = S##_6{}, \
            const S##_7& In##_7 = S##_7{}, \
            const S##_8& In##_8 = S##_8{})

#define DefineShuffledCtors(Compound, _0, _1, _2, _3, _4, _5, _6, _7, _8)  \
         Compound CtorAnyOrder_Force1Param(_0, _1, _2, _3, _4, _5, _6, _7, _8) : _0(In##_0), _1(In##_1), _2(In##_2), _3(In##_3), _4(In##_4), _5(In##_5), _6(In##_6), _7(In##_7), _8(In##_8) {} \
         Compound CtorAnyOrder_Force1Param(_1, _2, _3, _4, _5, _6, _7, _8, _0) : _0(In##_0), _1(In##_1), _2(In##_2), _3(In##_3), _4(In##_4), _5(In##_5), _6(In##_6), _7(In##_7), _8(In##_8) {} \
         Compound CtorAnyOrder_Force1Param(_2, _3, _4, _5, _6, _7, _8, _0, _1) : _0(In##_0), _1(In##_1), _2(In##_2), _3(In##_3), _4(In##_4), _5(In##_5), _6(In##_6), _7(In##_7), _8(In##_8) {} \
         Compound CtorAnyOrder_Force1Param(_3, _4, _5, _6, _7, _8, _0, _1, _2) : _0(In##_0), _1(In##_1), _2(In##_2), _3(In##_3), _4(In##_4), _5(In##_5), _6(In##_6), _7(In##_7), _8(In##_8) {} \
         Compound CtorAnyOrder_Force1Param(_4, _5, _6, _7, _8, _0, _1, _2, _3) : _0(In##_0), _1(In##_1), _2(In##_2), _3(In##_3), _4(In##_4), _5(In##_5), _6(In##_6), _7(In##_7), _8(In##_8) {} \
         Compound CtorAnyOrder_Force1Param(_5, _6, _7, _8, _0, _1, _2, _3, _4) : _0(In##_0), _1(In##_1), _2(In##_2), _3(In##_3), _4(In##_4), _5(In##_5), _6(In##_6), _7(In##_7), _8(In##_8) {} \
         Compound CtorAnyOrder_Force1Param(_6, _7, _8, _0, _1, _2, _3, _4, _5) : _0(In##_0), _1(In##_1), _2(In##_2), _3(In##_3), _4(In##_4), _5(In##_5), _6(In##_6), _7(In##_7), _8(In##_8) {} \
         Compound CtorAnyOrder_Force1Param(_8, _0, _1, _2, _3, _4, _5, _6, _7) : _0(In##_0), _1(In##_1), _2(In##_2), _3(In##_3), _4(In##_4), _5(In##_5), _6(In##_6), _7(In##_7), _8(In##_8) {}           


#endif // SHAREDUI_H
