#pragma once
#ifndef STYLINGUI_H
#define STYLINGUI_H

#include "imgui.h"
#include <d3d11.h>
#include <deque>
#include <functional>
#include <map>
#include <vector>

#include "shared.h"
#include "../editor_core.h"
#include "../../inc/dataformat.h"

#define IsValidVec2(Vec2) (Vec2).x >= (0.0f - FLT_EPSILON) && (Vec2).y >= (0.0f - FLT_EPSILON)
#define IsValidFloat(float) (float) >= (0.0f - FLT_EPSILON)
#define PushStyleV(PopCount, Flag, Input) (PopCount)++; ImGui::PushStyleVar(Flag, Input)
#define PushIf(Cond, PopCount, Flag, Input)  if (Cond) { PushStyleV(PopCount, Flag, Input); }

namespace CE
{
   namespace Style
   {
      typedef struct StyleBase
      {
         NO_DEFAULT_RO5(StyleBase)

         virtual void PushStyle() = 0;
         virtual void PopStyle() { while (--PopCount >= 0) { ImGui::PopStyleVar(); } }
         virtual std::string SerializeToJson() = 0;
         
         void WrapStyle(const std::function<void(void)>& Scoped) { PushStyle(); Scoped(); PopStyle(); }
         int PopCount = 0;
      }style_base_t;
      typedef struct SAlpha final : StyleBase
      {
         NO_DEFAULT_RO5(SAlpha)
         
         float Enabled = -1.0f;
         float Disabled = -1.0f;
         
         virtual void PushStyle() final
         {
            PushIf(IsValidFloat(Enabled), PopCount, ImGuiStyleVar_Alpha, Enabled)
            PushIf(IsValidFloat(Disabled), PopCount, ImGuiStyleVar_DisabledAlpha, Disabled)
         }
         
         // @todo
         virtual std::string SerializeToJson() final;
      }alpha_style_t;

      typedef struct SPadding final : StyleBase
      {
         NO_DEFAULT_RO5(SPadding)
         
         ImVec2 CellPadding = {-1.0f, -1.0f};
         ImVec2 FramePadding = {-1.0f, -1.0f};
         ImVec2 WindowPadding = {-1.0f, -1.0f};
         ImVec2 SeparatorTextPadding = {-1.0f, -1.0f};
         float Disabled = -1.0f;
         
         virtual void PushStyle() final
         {
            PushIf(IsValidVec2(CellPadding), PopCount, ImGuiStyleVar_CellPadding, CellPadding)
            PushIf(IsValidVec2(FramePadding), PopCount, ImGuiStyleVar_FramePadding, FramePadding)
            PushIf(IsValidVec2(WindowPadding), PopCount, ImGuiStyleVar_WindowPadding, WindowPadding)
            PushIf(IsValidVec2(SeparatorTextPadding), PopCount, ImGuiStyleVar_SeparatorTextPadding, SeparatorTextPadding)
         }
         
         // @todo
         virtual std::string SerializeToJson() final;
      }padding_style_t;      
      typedef struct SRounding final : StyleBase
      {
         NO_DEFAULT_RO5(SRounding)
         
         float ChildRounding = -1.0f;
         float FrameRounding = -1.0f;
         float GrabRounding = -1.0f;
         float PopupRounding = -1.0f;
         float ScrollbarRounding = -1.0f;
         float TabRounding = -1.0f;
         float WindowRounding = -1.0f;

         virtual void PushStyle() final
         {
            PushIf(IsValidFloat(ChildRounding), PopCount, ImGuiStyleVar_ChildRounding, ChildRounding)
            PushIf(IsValidFloat(FrameRounding), PopCount, ImGuiStyleVar_FrameRounding, FrameRounding)
            PushIf(IsValidFloat(GrabRounding), PopCount, ImGuiStyleVar_GrabRounding, GrabRounding)
            PushIf(IsValidFloat(PopupRounding), PopCount, ImGuiStyleVar_PopupRounding, PopupRounding)
            PushIf(IsValidFloat(ScrollbarRounding), PopCount, ImGuiStyleVar_ScrollbarRounding, ScrollbarRounding)
            PushIf(IsValidFloat(TabRounding), PopCount, ImGuiStyleVar_TabRounding, TabRounding)
            PushIf(IsValidFloat(WindowRounding), PopCount, ImGuiStyleVar_WindowRounding, WindowRounding)
         }
         // @todo
         virtual std::string SerializeToJson() final;
      }rounding_style_t;
      typedef struct SIndentation final : StyleBase
      {
         NO_DEFAULT_RO5(SIndentation)
         
         float IndentationOffset = -1.0f;

         virtual void PushStyle() final
         {
            PushIf(IsValidFloat(IndentationOffset), PopCount, ImGuiStyleVar_IndentSpacing, IndentationOffset)
         }
         // @todo
         virtual std::string SerializeToJson() final;
      }indent_style_t;
      typedef struct SSpacing final : StyleBase
      {
         NO_DEFAULT_RO5(SSpacing)
         
         ImVec2 ItemSpacing = {-1.0,-1.0};
         ImVec2 ItemInnerSpacing = {-1.0,-1.0};
         virtual void PushStyle() final
         {
            PushIf(IsValidVec2(ItemSpacing), PopCount, ImGuiStyleVar_ItemSpacing, ItemSpacing)
            PushIf(IsValidVec2(ItemInnerSpacing), PopCount, ImGuiStyleVar_ItemInnerSpacing, ItemInnerSpacing)
         }
         // @todo
         virtual std::string SerializeToJson() final;
      }spacing_style_t;       
      typedef struct SSizing final : StyleBase
      {
         NO_DEFAULT_RO5(SSizing)
         
         float ScrollbarSize = -1.0;
         float ChildBorderSize = -1.0;
         float FrameBorderSize = -1.0;
         float GrabMinSize = -1.0;
         float PopupBorderSize = -1.0;
         float TabBorderSize = -1.0;
         float WindowBorderSize = -1.0;
         float SeparatorTextBorderSize = -1.0;
         float TabBarBorderSize = -1.0;
         float WindowMinSize = -1.0;

         ImVec2 ItemInnerSpacing = {-1.0,-1.0};
         virtual void PushStyle() final
         {
            PushIf(IsValidFloat(ScrollbarSize), PopCount, ImGuiStyleVar_ScrollbarSize, ScrollbarSize)
            PushIf(IsValidFloat(ChildBorderSize), PopCount, ImGuiStyleVar_ChildBorderSize, ChildBorderSize)
            PushIf(IsValidFloat(FrameBorderSize), PopCount, ImGuiStyleVar_FrameBorderSize, FrameBorderSize)
            PushIf(IsValidFloat(GrabMinSize), PopCount, ImGuiStyleVar_GrabMinSize, GrabMinSize)
            PushIf(IsValidFloat(PopupBorderSize), PopCount, ImGuiStyleVar_PopupBorderSize, PopupBorderSize)
            PushIf(IsValidFloat(TabBorderSize), PopCount, ImGuiStyleVar_TabBorderSize, TabBorderSize)
            PushIf(IsValidFloat(WindowBorderSize), PopCount, ImGuiStyleVar_WindowBorderSize, WindowBorderSize)
            PushIf(IsValidFloat(SeparatorTextBorderSize), PopCount, ImGuiStyleVar_SeparatorTextBorderSize, SeparatorTextBorderSize)
            PushIf(IsValidFloat(TabBarBorderSize), PopCount, ImGuiStyleVar_TabBarBorderSize, TabBarBorderSize)
            PushIf(IsValidFloat(WindowMinSize), PopCount, ImGuiStyleVar_WindowMinSize, WindowMinSize)
         }
         // @todo
         virtual std::string SerializeToJson() final;
      }sizing_style_t;       
      typedef struct SAngling final : StyleBase
      {
         NO_DEFAULT_RO5(SAngling)
         
         float TableAngledHeadersAngle = -1.0;
         ImVec2 ItemInnerSpacing = {-1.0,-1.0};
         virtual void PushStyle() final
         {
            PushIf(IsValidFloat(TableAngledHeadersAngle), PopCount, ImGuiStyleVar_TableAngledHeadersAngle, TableAngledHeadersAngle)
         }
         // @todo
         virtual std::string SerializeToJson() final;
      }angling_style_t;
      typedef struct SAlignment final : StyleBase
      {
         NO_DEFAULT_RO5(SAlignment)
       
         ImVec2 ButtonTextAlign = {-1.0,-1.0};
         ImVec2 SelectableTextAlign = {-1.0,-1.0};
         ImVec2 SeparatorTextAlign = {-1.0,-1.0};
         ImVec2 WindowTitleAlign = {-1.0,-1.0};
         ImVec2 TableAngledHeadersTextAlign = {-1.0,-1.0};
         virtual void PushStyle() final
         {
            PushIf(IsValidVec2(ButtonTextAlign), PopCount, ImGuiStyleVar_TableAngledHeadersAngle, ButtonTextAlign)
            PushIf(IsValidVec2(SelectableTextAlign), PopCount, ImGuiStyleVar_TableAngledHeadersAngle, SelectableTextAlign)
            PushIf(IsValidVec2(SeparatorTextAlign), PopCount, ImGuiStyleVar_TableAngledHeadersAngle, SeparatorTextAlign)
            PushIf(IsValidVec2(WindowTitleAlign), PopCount, ImGuiStyleVar_TableAngledHeadersAngle, WindowTitleAlign)
            PushIf(IsValidVec2(TableAngledHeadersTextAlign), PopCount, ImGuiStyleVar_TableAngledHeadersAngle, TableAngledHeadersTextAlign)
         }
         // @todo
         virtual std::string SerializeToJson() final;
      }align_style_t;

      typedef struct Compound final : StyleBase
      {
         NO_DEFAULT_RO5(Compound)
         
         // This just gives us some lee-way in ctors to choose from when requesting to create a Compound style
         DefineShuffledCtors(Compound, Alpha, Padding, Rounding, Indentation, Spacing, Sizing, Angling, Alignment );

         
         SAlpha Alpha;
         SPadding Padding;
         SRounding Rounding;
         SIndentation Indentation;
         SSpacing Spacing;
         SSizing Sizing;
         SAngling Angling;
         SAlignment Alignment;
         virtual void PushStyle() final
         {
            Alpha.PushStyle();
            Padding.PushStyle();
            Rounding.PushStyle();
            Indentation.PushStyle();
            Spacing.PushStyle();
            Sizing.PushStyle();
            Angling.PushStyle();
            Alignment.PushStyle();
         }
         virtual void PopStyle() final
         {
            Alpha.PopStyle();
            Padding.PopStyle();
            Rounding.PopStyle();
            Indentation.PopStyle();
            Spacing.PopStyle();
            Sizing.PopStyle();
            Angling.PopStyle();
            Alignment.PopStyle();
         }
         
         // @todo
         virtual std::string SerializeToJson() final;
      }compund_style_t;
      
   }
}


#endif // STYLINGUI_H
