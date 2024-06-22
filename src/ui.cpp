#include "imgui.h"
#include "imgui_internal.h"

#include <vector>

#include "../inc/dataformat.h"
#include "../inc/ui.h"


#define PDDEBUG

ImFont* UIHandler::GlobalFont = nullptr;

void UIHandler::DrawTemplates(Template* Category)
{
   const ImVec2& CursorScreenPos = ImGui::GetCursorScreenPos();
   const ImVec2& Origin = ImGui::GetCursorPos();

   float X = 0.f;
   float Y = 0.f;

   for (size_t i = 0; i < Category->Inner.size(); i++)
   {
      GridElement& Element = Category->Inner[i];
      const ElementStyle& Style = Element.Style;
      
      ImGui::PushID(static_cast<int>(i));
      const float HalfPaddingX = Style.Padding.x / 2;
      const float HalfPaddingY = Style.Padding.y / 2;

      const float SizeX = Style.ElementSizes.x;
      const float SizeY = Style.ElementSizes.y;
      

      //----
      ImGuiWindow* Window = ImGui::GetCurrentWindow();
      const ImGuiID id = Window->GetID(Category);
      
      bool OutHovered;
      bool OutHeld;
      ImGui::ButtonBehavior(ImRect(X, Y, X + SizeX, Y + SizeY), id, &OutHovered, &OutHeld, ImGuiButtonFlags_PressedOnClick);

      ImVec2 MousePos = ImGui::GetMousePos();
      MousePos.x -= CursorScreenPos.x;
      MousePos.y -= CursorScreenPos.y;
      OutHovered = (MousePos.x > X && MousePos.x < X + SizeX && MousePos.y > Y && MousePos.y < Y + SizeY);

      //---- Selection
      if (OutHovered || Element.IsSelected)
      {
         const float OffsetX = CursorScreenPos.x + HalfPaddingX;
         const float OffsetY = CursorScreenPos.y + HalfPaddingY;
         
         const ImU32 Column = ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered]);
         X += OffsetX;
         Y += OffsetY;
         ImGui::RenderFrame(ImVec2(X, Y), ImVec2(X + SizeX, Y + SizeY), Column, true, 3.f);
         X -= OffsetX;
         Y -= OffsetY;
      }

      ImGui::SetCursorPosX(X + Origin.x + HalfPaddingX);
      ImGui::SetCursorPosY(Y + Origin.y + HalfPaddingY);
      // CreateTexture(Element);
      // ImGui::Image(Element.ImageID, ImVec2(50, 50));


      // hide long names?
      // std::string Name = Element.Label;
      // if (Name.length() > 8) { Name = Name.substr(0, 8) + "..."; }
      const bool DidPressButton = ImGui::Button(Element.Label.c_str(), Style.ElementSizes);
      if (DidPressButton)
      {
         Category->LastSelectedElement = Category->CurrentSelectedElement; 
         Category->CurrentSelectedElement = static_cast<int>(i);
      }      
      
      if (OutHovered)
      {
         const bool SelectNewItem = DidPressButton && Category->LastSelectedElement != Category->CurrentSelectedElement;
         if (Element.IsSelected == false)
         {
            Element.IsSelected = SelectNewItem; // select new

            // deselect old
            if (Element.IsSelected && Category->LastSelectedElement != INVALID_INDEX)
            {
               GridElement& LastSelectedElement = Category->Inner[Category->LastSelectedElement];
               LastSelectedElement.IsSelected = false;
            }
         }         
      }
      
      //---- Text
      ImGui::SetCursorPosX(X + Origin.x + HalfPaddingX);
      ImGui::SetCursorPosY(Y + Origin.y + SizeY);
      ImGui::PushFont(GlobalFont);
      
      if (Element.CurrentValue >= 0)
      {
         ImGui::Text("%02d", Element.CurrentValue); 
      }

      ImGui::PopFont();

      //---- Next position
      X += SizeX + HalfPaddingX * 2;
      if (X + SizeX > ImGui::GetWindowWidth())
      {
         X = 0;
         Y += SizeY + HalfPaddingY * 2;
      }

      ImGui::PopID();
   }
}

void UIHandler::OnPaint(ImGuiIO& io, ImVec4 ClearColor)
{
#ifdef PDDEBUG      
   ImGui::ShowIDStackToolWindow();   // @imgui debug tools 
#endif // PDDEBUG
   
   // Create a Class editor window
   {
      static char buf[Data::Format::NameSize];
      ImGui::Begin("CEditor Class Creator");
      ImGui::PushFont(GlobalFont);

      const char* Input = "Class Base"; 
      ImGui::Text(Input);
      ImGui::InputText("{Class Name}", buf, Data::Format::NameSize);

      ImGui::NewLine();
      ImGui::Text("{Prime Stat}");
      DrawTemplates(&UI::Class::PrimeStatTemplates);
      
      ImGui::NewLine();
      ImGui::Text("{Hit-die}");
      DrawTemplates(&UI::Class::PlatonicDiceTemplates);

      //---- Save class buttons
      ImGui::NewLine();
      ImGui::NewLine();
      DrawTemplates(&UI::Menu::MenuControlTemplates);
      
      ImGui::PopFont();
      ImGui::End();
   }

   
   // Create a Race editor window
   {
      static char buf[Data::Format::NameSize];
      ImGui::Begin("CEditor Race Creator");
      ImGui::PushFont(GlobalFont);

      const char* Input = "Class Base"; 
      ImGui::Text(Input);
      ImGui::InputText("{Class Name}", buf, Data::Format::NameSize);

      ImGui::Selectable("{Prime Stat}");

      ImGui::NewLine();
      ImGui::Text("Hit-die");
      DrawTemplates(&UI::Class::PlatonicDiceTemplates);
      
      ImGui::PopFont();
      ImGui::End();
   }
   
   // Create a Character editor window
   {
      static char buf[Data::Format::NameSize];
      ImGui::Begin("CEditor Character Creator");
      ImGui::PushFont(GlobalFont);

      const char* Input = "Character Base"; 
      ImGui::Text(Input);
      ImGui::InputText("{Character Name}", buf, Data::Format::NameSize);

      // @todo Select class
      // @todo Select race
      
      ImGui::PopFont();
      ImGui::End();
   }
}

ImFont* UIHandler::LoadFonts(ImGuiIO& io)
{
   io.Fonts->AddFontDefault();
   io.Fonts->AddFontFromFileTTF("../../rsc/Fonts/Besley/static/Besley-BlackItalic.ttf", 15.0f);
   ImFont* font = io.Fonts->AddFontFromFileTTF("../../rsc/Fonts/Besley/static/Besley-Black.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
   IM_ASSERT(font != nullptr);

   return font; 
}