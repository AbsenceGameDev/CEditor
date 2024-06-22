#pragma once
#ifndef UI_H
#define UI_H

#include "imgui.h"
#include <d3d11.h>
#include <functional>
#include <vector>

#include "../inc/dataformat.h"

// fwd decl.
struct Template;
class UIHandler
{
public:
   static void DrawTemplates(Template* Category);
   static void OnPaint(ImGuiIO& io, ImVec4 ClearColor);
   static ImFont* LoadFonts(ImGuiIO& io);

   static ImFont* GlobalFont;
};

struct ElementStyle
{
   ImVec2 ElementSizes = {50, 50};
   ImVec2 Padding      = {20, 20};
   ImTextureID ImageID{};
};

struct GridElement
{
   GridElement(){}
   
   GridElement(
      const std::string& InLabel,
      ElementStyle InStyle,
      int InCurrentValue = 0,
      std::function<int(double)> InCallbackTarget = {})
      : Label(InLabel)
      , Style(InStyle)
      , IsSelected(false)
      , CurrentValue(InCurrentValue)
      , CallbackTarget(InCallbackTarget) {}
   
   std::string  Label{"Test"};
   ElementStyle Style{};
   bool         IsSelected = false;
   int          CurrentValue = 0;


   std::function<int(double)> CallbackTarget;
};

typedef struct Template
{
   int LastSelectedElement    = -1;
   int CurrentSelectedElement = -1;
   std::vector<GridElement> Inner;
}Template_t;

namespace UI
{
   static constexpr ImVec2       DefaultPadding = { 20, 20};
   static constexpr ImVec2       DefaultSizes   = { 50, 50};
   static constexpr ElementStyle DefaultStyle   = {DefaultSizes, DefaultPadding, ImTextureID{}};
      
   namespace Class
   {
      static const std::string StrLabel = ("STR");
      static const std::string IntLabel = ("INT");
      static const std::string WisLabel = ("WIS");
      static const std::string DexLabel = ("DEX");
      static const std::string ConLabel = ("CON");
      static const std::string ChaLabel = ("CHA");
      static Template PrimeStatTemplates{
         INVALID_INDEX,
         INVALID_INDEX,
         {
            GridElement{StrLabel, DefaultStyle, 0, [](double) -> int{ return 0;}},
            GridElement{IntLabel, DefaultStyle, 0, [](double) -> int{ return 0;}},
            GridElement{WisLabel, DefaultStyle, 0, [](double) -> int{ return 0;}},
            GridElement{DexLabel, DefaultStyle, 0, [](double) -> int{ return 0;}},
            GridElement{ConLabel, DefaultStyle, 0, [](double) -> int{ return 0;}},
            GridElement{ChaLabel, DefaultStyle, 0, [](double) -> int{ return 0;}}
         }
      }; 
      
      static const std::string D4Label  = ("1d4");
      static const std::string D6Label  = ("1d6");
      static const std::string D8Label  = ("1d8");
      static const std::string D12Label = ("1d12");
      static const std::string D20Label = ("1d20");
      static Template PlatonicDiceTemplates{
         INVALID_INDEX,
         INVALID_INDEX,
         {
            GridElement{D4Label,  DefaultStyle, 0, [](double) -> int{ return 0;}},
            GridElement{D6Label,  DefaultStyle, 0, [](double) -> int{ return 0;}},
            GridElement{D8Label,  DefaultStyle, 0, [](double) -> int{ return 0;}},
            GridElement{D12Label, DefaultStyle, 0, [](double) -> int{ return 0;}},
            GridElement{D20Label, DefaultStyle, 0, [](double) -> int{ return 0;}}
         }
      };
   }


   namespace Menu
   {
      static const std::string SaveLabel = ("SaveConfig");
      static Template MenuControlTemplates{
         INVALID_INDEX,
         INVALID_INDEX,
         {
            GridElement{SaveLabel, DefaultStyle, INVALID_INDEX, [](double) -> int{ return 0;}}
         }         
      }; 
   }
}

#endif // UI_H
