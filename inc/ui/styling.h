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

namespace CE
{
   namespace Style
   {
      typedef struct StyleBase
      {
         NO_DEFAULT_RO5(StyleBase)

         virtual void PushStyle() = 0;
         virtual void PopStyle() { while (PopCount > 0) { --PopCount; ImGui::PopStyleVar(); } }
         virtual std::string SerializeToJson(const std::string& ObjectName  = {}) = 0;
         
         void WrapStyle(const std::function<void(void)>& Scoped) { PushStyle(); Scoped(); PopStyle(); }
         int PopCount = 0;
      }style_base_t;

      // @note @bug Ctor_N_UniformParams(SIndentation, ...); expands improperly by teh compilers preprocessor, but expanded properly by resharper,
      // @note @bug until this is resolved somehow, the macro can still be used as a shorthand to generate A widget style group via telling the editor to expand it fully, in rider this would be right-clicking or pressing 'alt' + '.' and then select to expand in the context menu 
      
      typedef struct SAlpha final : StyleBase
      {
         // expanded in editor from 'Ctor_N_UniformParams(SAlpha, -1.0f, float, 2, Disabled, Enabled)'
_Pragma("EXPANDED_MACRO")
         SAlpha() { }
         virtual ~SAlpha() override = default;
         SAlpha(const SAlpha& Other) : StyleBase(Other) { *this = Other; }
         SAlpha(const SAlpha&& Other) noexcept { *this = Other; }
         SAlpha(float InEnabled, float InDisabled) : Enabled(InEnabled), Disabled(InDisabled) { }

         SAlpha& operator=(const SAlpha& Other)
         {
            this->Enabled = Other.Enabled;
            this->Disabled = Other.Disabled;
            return *this;
         }

         SAlpha& operator=(SAlpha&& Other) noexcept
         {
            this->Enabled = Other.Enabled;
            this->Disabled = Other.Disabled;
            return *this;
         }

         float Enabled = {-1.0f};
         float Disabled = {-1.0f}; 
_Pragma("EXPANDED_MACRO")

         virtual void PushStyle() final
         {
            PushIf(IsValidFloat(Enabled), PopCount, ImGuiStyleVar_Alpha, Enabled)
            PushIf(IsValidFloat(Disabled), PopCount, ImGuiStyleVar_DisabledAlpha, Disabled)
         }
         
         virtual std::string SerializeToJson(const std::string& ObjectName  = {}) override;
      }alpha_style_t;
      

      typedef struct SPadding final : StyleBase
      {
         // expanded in editor from 'Ctor_N_UniformParams(SPadding, -1.0f, PDVec2, 4, CellPadding, FramePadding, WindowPadding, SeparatorTextPadding)'
_Pragma("EXPANDED_MACRO")
         SPadding() { }
         virtual ~SPadding() override = default;
         SPadding(const SPadding& Other) : StyleBase(Other) { *this = Other; }
         SPadding(const SPadding&& Other) noexcept { *this = Other; }

         SPadding(PDVec2 InSeparatorTextPadding, PDVec2 InWindowPadding, PDVec2 InFramePadding, PDVec2 InCellPadding) :
            SeparatorTextPadding(InSeparatorTextPadding), WindowPadding(InWindowPadding), FramePadding(InFramePadding),
            CellPadding(InCellPadding) {}

         SPadding& operator=(const SPadding& Other)
         {
            this->SeparatorTextPadding = Other.SeparatorTextPadding;
            this->WindowPadding = Other.WindowPadding;
            this->FramePadding = Other.FramePadding;
            this->CellPadding = Other.CellPadding;
            return *this;
         }

         SPadding& operator=(SPadding&& Other) noexcept
         {
            this->SeparatorTextPadding = Other.SeparatorTextPadding;
            this->WindowPadding = Other.WindowPadding;
            this->FramePadding = Other.FramePadding;
            this->CellPadding = Other.CellPadding;
            return *this;
         }

         PDVec2 SeparatorTextPadding = {-1.0f};
         PDVec2 WindowPadding = {-1.0f};
         PDVec2 FramePadding = {-1.0f};
         PDVec2 CellPadding = {-1.0f};
_Pragma("EXPANDED_MACRO")

         virtual void PushStyle() final
         {
            PushIf(IsValidVec2(CellPadding), PopCount, ImGuiStyleVar_CellPadding, CellPadding)
            PushIf(IsValidVec2(FramePadding), PopCount, ImGuiStyleVar_FramePadding, FramePadding)
            PushIf(IsValidVec2(WindowPadding), PopCount, ImGuiStyleVar_WindowPadding, WindowPadding)
            PushIf(IsValidVec2(SeparatorTextPadding), PopCount, ImGuiStyleVar_SeparatorTextPadding, SeparatorTextPadding)
         }
         
         virtual std::string SerializeToJson(const std::string& ObjectName  = {}) override;
      }padding_style_t;
      
      typedef struct SRounding final : StyleBase
      {
         // expanded in editor from ' Ctor_N_UniformParams(SRounding, -1.0f, float, 7, ChildRounding, FrameRounding, GrabRounding, PopupRounding, ScrollbarRounding, TabRounding, WindowRounding)
_Pragma("EXPANDED_MACRO")
         SRounding() { }
         virtual ~SRounding() override = default;
         SRounding(const SRounding& Other) : StyleBase(Other) { *this = Other; }
         SRounding(const SRounding&& Other) noexcept { *this = Other; }

         SRounding(float InWindowRounding, float InTabRounding, float InScrollbarRounding, float InPopupRounding, float InGrabRounding,
                   float InFrameRounding, float InChildRounding) : WindowRounding(InWindowRounding), TabRounding(InTabRounding),
                                                                   ScrollbarRounding(InScrollbarRounding), PopupRounding(InPopupRounding),
                                                                   GrabRounding(InGrabRounding), FrameRounding(InFrameRounding),
                                                                   ChildRounding(InChildRounding) {}
         SRounding& operator=(const SRounding& Other)
         {
            this->WindowRounding = Other.WindowRounding;
            this->TabRounding = Other.TabRounding;
            this->ScrollbarRounding = Other.ScrollbarRounding;
            this->PopupRounding = Other.PopupRounding;
            this->GrabRounding = Other.GrabRounding;
            this->FrameRounding = Other.FrameRounding;
            this->ChildRounding = Other.ChildRounding;
            return *this;
         }

         SRounding& operator=(SRounding&& Other) noexcept
         {
            this->WindowRounding = Other.WindowRounding;
            this->TabRounding = Other.TabRounding;
            this->ScrollbarRounding = Other.ScrollbarRounding;
            this->PopupRounding = Other.PopupRounding;
            this->GrabRounding = Other.GrabRounding;
            this->FrameRounding = Other.FrameRounding;
            this->ChildRounding = Other.ChildRounding;
            return *this;
         }

         float WindowRounding = {-1.0f};
         float TabRounding = {-1.0f};
         float ScrollbarRounding = {-1.0f};
         float PopupRounding = {-1.0f};
         float GrabRounding = {-1.0f};
         float FrameRounding = {-1.0f};
         float ChildRounding = {-1.0f};
_Pragma("EXPANDED_MACRO")
         
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
         virtual std::string SerializeToJson(const std::string& ObjectName  = {}) override;
      }rounding_style_t;
      
      typedef struct SIndentation final : StyleBase
      {
         // expanded in editor from ' Ctor_N_UniformParams(SIndentation, -1.0f, float, 1, IndentationOffset); '
_Pragma("EXPANDED_MACRO")
         SIndentation() {}
         virtual ~SIndentation() override = default;
         SIndentation(const SIndentation& Other) : StyleBase(Other) { *this = Other; }
         SIndentation(const SIndentation&& Other) noexcept { *this = Other; }
         SIndentation(float InIndentationOffset) : IndentationOffset(InIndentationOffset) {}

         SIndentation& operator=(const SIndentation& Other)
         {
            this->IndentationOffset = Other.IndentationOffset;
            return *this;
         }

         SIndentation& operator=(SIndentation&& Other) noexcept
         {
            this->IndentationOffset = Other.IndentationOffset;
            return *this;
         }

         float IndentationOffset = {-1.0f};
_Pragma("EXPANDED_MACRO");

         virtual void PushStyle() final
         {
            PushIf(IsValidFloat(IndentationOffset), PopCount, ImGuiStyleVar_IndentSpacing, IndentationOffset)
         }
         virtual std::string SerializeToJson(const std::string& ObjectName  = {}) override;
      }indent_style_t;
      
      typedef struct SSpacing final : StyleBase
      {
         // expanded in editor from ' Ctor_N_UniformParams(SSpacing, -1.0f, PDVec2, 2, ItemSpacing, ItemInnerSpacing); '
_Pragma("EXPANDED_MACRO")
         SSpacing(){}
         virtual ~SSpacing() override = default;
         SSpacing(const SSpacing& Other) : StyleBase(Other) { *this = Other; }
         SSpacing(const SSpacing&& Other) noexcept { *this = Other; }
         SSpacing(PDVec2 InItemInnerSpacing, PDVec2 InItemSpacing) : ItemInnerSpacing(InItemInnerSpacing), ItemSpacing(InItemSpacing){}

         SSpacing& operator=(const SSpacing& Other)
         {
            this->ItemInnerSpacing = Other.ItemInnerSpacing;
            this->ItemSpacing = Other.ItemSpacing;
            return *this;
         }

         SSpacing& operator=(SSpacing&& Other) noexcept
         {
            this->ItemInnerSpacing = Other.ItemInnerSpacing;
            this->ItemSpacing = Other.ItemSpacing;
            return *this;
         }

         PDVec2 ItemInnerSpacing = {-1.0f};
         PDVec2 ItemSpacing = {-1.0f};
_Pragma("EXPANDED_MACRO");
         
         virtual void PushStyle() final
         {
            PushIf(IsValidVec2(ItemSpacing), PopCount, ImGuiStyleVar_ItemSpacing, ItemSpacing)
            PushIf(IsValidVec2(ItemInnerSpacing), PopCount, ImGuiStyleVar_ItemInnerSpacing, ItemInnerSpacing)
         }
         virtual std::string SerializeToJson(const std::string& ObjectName  = {}) override;
      }spacing_style_t;
      
      typedef struct SSizing1D final : StyleBase
      {
         // expanded in editor from 'Ctor_N_UniformParams(SSizingFloats, -1.0f, float, 10, ScrollbarSize, ChildBorderSize, FrameBorderSize, GrabMinSize, PopupBorderSize,TabBorderSize, WindowBorderSize, SeparatorTextBorderSize, TabBarBorderSize);'
_Pragma("EXPANDED_MACRO")
         SSizing1D() {}
         virtual ~SSizing1D() override = default;
         SSizing1D(const SSizing1D& Other) : StyleBase(Other) { *this = Other; }
         SSizing1D(const SSizing1D&& Other) noexcept { *this = Other; }

         SSizing1D(float InTabBarBorderSize, float InSeparatorTextBorderSize, float InWindowBorderSize,
                 float InTabBorderSize, float InPopupBorderSize, float InGrabMinSize, float InFrameBorderSize, float InChildBorderSize,
                 float InScrollbarSize) : TabBarBorderSize(InTabBarBorderSize),
                                          SeparatorTextBorderSize(InSeparatorTextBorderSize), WindowBorderSize(InWindowBorderSize),
                                          TabBorderSize(InTabBorderSize), PopupBorderSize(InPopupBorderSize), GrabMinSize(InGrabMinSize),
                                          FrameBorderSize(InFrameBorderSize), ChildBorderSize(InChildBorderSize),
                                          ScrollbarSize(InScrollbarSize) { }

         SSizing1D& operator=(const SSizing1D& Other)
         {
            this->TabBarBorderSize = Other.TabBarBorderSize;
            this->SeparatorTextBorderSize = Other.SeparatorTextBorderSize;
            this->WindowBorderSize = Other.WindowBorderSize;
            this->TabBorderSize = Other.TabBorderSize;
            this->PopupBorderSize = Other.PopupBorderSize;
            this->GrabMinSize = Other.GrabMinSize;
            this->FrameBorderSize = Other.FrameBorderSize;
            this->ChildBorderSize = Other.ChildBorderSize;
            this->ScrollbarSize = Other.ScrollbarSize;
            return *this;
         }

         SSizing1D& operator=(SSizing1D&& Other) noexcept
         {
            this->TabBarBorderSize = Other.TabBarBorderSize;
            this->SeparatorTextBorderSize = Other.SeparatorTextBorderSize;
            this->WindowBorderSize = Other.WindowBorderSize;
            this->TabBorderSize = Other.TabBorderSize;
            this->PopupBorderSize = Other.PopupBorderSize;
            this->GrabMinSize = Other.GrabMinSize;
            this->FrameBorderSize = Other.FrameBorderSize;
            this->ChildBorderSize = Other.ChildBorderSize;
            this->ScrollbarSize = Other.ScrollbarSize;
            return *this;
         }

         float TabBarBorderSize = {-1.0f};
         float SeparatorTextBorderSize = {-1.0f};
         float WindowBorderSize = {-1.0f};
         float TabBorderSize = {-1.0f};
         float PopupBorderSize = {-1.0f};
         float GrabMinSize = {-1.0f};
         float FrameBorderSize = {-1.0f};
         float ChildBorderSize = {-1.0f};
         float ScrollbarSize = {-1.0f};
_Pragma("EXPANDED_MACRO");
         
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
         }
         virtual std::string SerializeToJson(const std::string& ObjectName  = {}) override;
      }sizing_float_style_t;

      typedef struct SSizing2D final : StyleBase
      {
         // expanded in editor from 'Ctor_N_UniformParams(SSizingFloats, -1.0f, PDVec, 1, WindowMinSize);'
_Pragma("EXPANDED_MACRO")
         SSizing2D() {}
         virtual ~SSizing2D() override = default;
         SSizing2D(const SSizing2D& Other) : StyleBase(Other) { *this = Other; }
         SSizing2D(const SSizing2D&& Other) noexcept { *this = Other; }

         SSizing2D(PDVec2 InWindowMinSize) : WindowMinSize(InWindowMinSize){ }

         SSizing2D& operator=(const SSizing2D& Other)
         {
            this->WindowMinSize = Other.WindowMinSize;
            return *this;
         }

         SSizing2D& operator=(SSizing2D&& Other) noexcept
         {
            this->WindowMinSize = Other.WindowMinSize;
            return *this;
         }

         PDVec2 WindowMinSize = {-1.0f};
_Pragma("EXPANDED_MACRO");
         
         virtual void PushStyle() final
         {
            PushIf(IsValidVec2(WindowMinSize), PopCount, ImGuiStyleVar_WindowMinSize, WindowMinSize)
         }
         virtual std::string SerializeToJson(const std::string& ObjectName  = {}) override;
      }sizing_vector2_style_t;
      
      
      typedef struct SAngling final : StyleBase
      {
         // expanded in editor from ' Ctor_N_UniformParams(SAngling, -1.0f, float, 1, TableAngledHeadersAngle);'
_Pragma("EXPANDED_MACRO")
         SAngling() {}
         virtual ~SAngling() override = default;
         SAngling(const SAngling& Other) : StyleBase(Other) { *this = Other; }
         SAngling(const SAngling&& Other) noexcept { *this = Other; }
         SAngling(float InTableAngledHeadersAngle) : TableAngledHeadersAngle(InTableAngledHeadersAngle) {}

         SAngling& operator=(const SAngling& Other)
         {
            this->TableAngledHeadersAngle = Other.TableAngledHeadersAngle;
            return *this;
         }

         SAngling& operator=(SAngling&& Other) noexcept
         {
            this->TableAngledHeadersAngle = Other.TableAngledHeadersAngle;
            return *this;
         }

         float TableAngledHeadersAngle = {-1.0f};
_Pragma("EXPANDED_MACRO");
         
         virtual void PushStyle() final
         {
            PushIf(IsValidFloat(TableAngledHeadersAngle), PopCount, ImGuiStyleVar_TableAngledHeadersAngle, TableAngledHeadersAngle)
         }
         virtual std::string SerializeToJson(const std::string& ObjectName  = {}) override;
      }angling_style_t;
      
      typedef struct SAlignment final : StyleBase
      {
         // expanded in editor from 'Ctor_N_UniformParams(SAlignment, -1.0f, ImVec2, 5, ButtonTextAlign, SelectableTextAlign, SeparatorTextAlign, WindowTitleAlign, TableAngledHeadersTextAlign);'
_Pragma("EXPANDED_MACRO")
         SAlignment(){}
         virtual ~SAlignment() override = default;
         SAlignment(const SAlignment& Other) : StyleBase(Other) { *this = Other; }
         SAlignment(const SAlignment&& Other) noexcept { *this = Other; }

         SAlignment(PDVec2 InTableAngledHeadersTextAlign, PDVec2 InWindowTitleAlign, PDVec2 InSeparatorTextAlign,
                    PDVec2 InSelectableTextAlign, PDVec2 InButtonTextAlign) : TableAngledHeadersTextAlign(InTableAngledHeadersTextAlign),
                                                                              WindowTitleAlign(InWindowTitleAlign),
                                                                              SeparatorTextAlign(InSeparatorTextAlign),
                                                                              SelectableTextAlign(InSelectableTextAlign),
                                                                              ButtonTextAlign(InButtonTextAlign) {}

         SAlignment& operator=(const SAlignment& Other)
         {
            this->TableAngledHeadersTextAlign = Other.TableAngledHeadersTextAlign;
            this->WindowTitleAlign = Other.WindowTitleAlign;
            this->SeparatorTextAlign = Other.SeparatorTextAlign;
            this->SelectableTextAlign = Other.SelectableTextAlign;
            this->ButtonTextAlign = Other.ButtonTextAlign;
            return *this;
         }

         SAlignment& operator=(SAlignment&& Other) noexcept
         {
            this->TableAngledHeadersTextAlign = Other.TableAngledHeadersTextAlign;
            this->WindowTitleAlign = Other.WindowTitleAlign;
            this->SeparatorTextAlign = Other.SeparatorTextAlign;
            this->SelectableTextAlign = Other.SelectableTextAlign;
            this->ButtonTextAlign = Other.ButtonTextAlign;
            return *this;
         }

         PDVec2 TableAngledHeadersTextAlign = {-1.0f};
         PDVec2 WindowTitleAlign = {-1.0f};
         PDVec2 SeparatorTextAlign = {-1.0f};
         PDVec2 SelectableTextAlign = {-1.0f};
         PDVec2 ButtonTextAlign = {-1.0f};
_Pragma("EXPANDED_MACRO");
         

         virtual void PushStyle() final
         {
            PushIf(IsValidVec2(ButtonTextAlign), PopCount, ImGuiStyleVar_ButtonTextAlign, ButtonTextAlign)
            PushIf(IsValidVec2(SelectableTextAlign), PopCount, ImGuiStyleVar_SelectableTextAlign, SelectableTextAlign)
            PushIf(IsValidVec2(SeparatorTextAlign), PopCount, ImGuiStyleVar_SeparatorTextAlign, SeparatorTextAlign)
            PushIf(IsValidVec2(WindowTitleAlign), PopCount, ImGuiStyleVar_WindowTitleAlign, WindowTitleAlign)
            PushIf(IsValidVec2(TableAngledHeadersTextAlign), PopCount, ImGuiStyleVar_TableAngledHeadersTextAlign, TableAngledHeadersTextAlign)
         }
         virtual std::string SerializeToJson(const std::string& ObjectName  = {}) override;
      }align_style_t;

      typedef struct Compound final : StyleBase
      {
         Compound(){};
         Compound(const Compound& Other) : StyleBase(Other) { *this = Other; }
         Compound(const Compound&& Other) noexcept { *this = Other; }

         Compound& operator=(const Compound& Other)
         {
            this->Alpha = Other.Alpha; 
            this->Padding = Other.Padding; 
            this->Rounding = Other.Rounding; 
            this->Indentation = Other.Indentation; 
            this->Spacing = Other.Spacing; 
            this->Sizing1D = Other.Sizing1D; 
            this->Sizing2D = Other.Sizing2D; 
            this->Angling = Other.Angling; 
            this->Alignment = Other.Alignment; 
            return *this;
         }

         Compound& operator=(Compound&& Other) noexcept
         {
            this->Alpha = Other.Alpha; 
            this->Padding = Other.Padding; 
            this->Rounding = Other.Rounding; 
            this->Indentation = Other.Indentation; 
            this->Spacing = Other.Spacing; 
            this->Sizing1D = Other.Sizing1D; 
            this->Sizing2D = Other.Sizing2D; 
            this->Angling = Other.Angling; 
            this->Alignment = Other.Alignment; 
            return *this;
         }
         
         //
         // This just gives us some lee-way in ctors to choose from when requesting to create a Compound style
         DefineShuffledCtors(Compound, Alpha, Padding, Rounding, Indentation, Spacing, Sizing1D, Sizing2D, Angling, Alignment );
         
         SAlpha Alpha;
         SPadding Padding;
         SRounding Rounding;
         SIndentation Indentation;
         SSpacing Spacing;
         SSizing1D Sizing1D;
         SSizing2D Sizing2D;
         SAngling Angling;
         SAlignment Alignment;
         virtual void PushStyle() final
         {
            Alpha.PushStyle();
            Padding.PushStyle();
            Rounding.PushStyle();
            Indentation.PushStyle();
            Spacing.PushStyle();
            Sizing1D.PushStyle();
            Sizing2D.PushStyle();
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
            Sizing1D.PopStyle();
            Sizing2D.PopStyle();
            Angling.PopStyle();
            Alignment.PopStyle();
         }
         
         virtual std::string SerializeToJson(const std::string& ObjectName = {}) override;
      }compound_style_t;
      
      enum StylingSelector { Global = 0, MainMenuBar, CharacterEditor, CharacterSelector, RaceEditor, ClassEditor, End};
      
   }
}


#endif // STYLINGUI_H
