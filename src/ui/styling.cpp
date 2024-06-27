#include "imgui.h"
#include "imgui_internal.h"

#include <string>

#include "../../inc/ui/styling.h"
#include "../../inc/ui/handler.h"
#include "../../inc/jsonhelper.h"


std::string CE::Style::SAlpha::SerializeToJson(const std::string& ObjectName)
{
   std::string Serialized
      = JHelper::PoorMansSerializer::StartSerialize( ObjectName.empty() == false ? ObjectName : R"("Alpha" : )" )
      + JHelper::PoorMansSerializer::SerializeGenericNumber<true,true,float>(PRM(Enabled))
      + JHelper::PoorMansSerializer::SerializeGenericNumber<false,true,float>(PRM(Disabled))
      + JHelper::PoorMansSerializer::EndSerialize();
   return Serialized;
}


std::string CE::Style::SPadding::SerializeToJson(const std::string& ObjectName)
{
   std::string Serialized
      = JHelper::PoorMansSerializer::StartSerialize(ObjectName.empty() == false ? ObjectName : R"("Padding" : )")
      + JHelper::PoorMansSerializer::SerializeVec2<true,true>(PRM(CellPadding))
      + JHelper::PoorMansSerializer::SerializeVec2<false,true>(PRM(FramePadding))
      + JHelper::PoorMansSerializer::SerializeVec2<false,true>(PRM(WindowPadding))
      + JHelper::PoorMansSerializer::SerializeVec2<false,true>(PRM(SeparatorTextPadding))
      + JHelper::PoorMansSerializer::EndSerialize();
   return Serialized;
}

std::string CE::Style::SRounding::SerializeToJson(const std::string& ObjectName)
{
   std::string Serialized
      = JHelper::PoorMansSerializer::StartSerialize(ObjectName.empty() == false ? ObjectName : R"("Rounding" : )")
      + JHelper::PoorMansSerializer::SerializeGenericNumber<true,true,float>(PRM(ChildRounding))
      + JHelper::PoorMansSerializer::SerializeGenericNumber<false,true,float>(PRM(FrameRounding))
      + JHelper::PoorMansSerializer::SerializeGenericNumber<false,true,float>(PRM(GrabRounding))
      + JHelper::PoorMansSerializer::SerializeGenericNumber<false,true,float>(PRM(PopupRounding))
      + JHelper::PoorMansSerializer::SerializeGenericNumber<false,true,float>(PRM(ScrollbarRounding))
      + JHelper::PoorMansSerializer::SerializeGenericNumber<false,true,float>(PRM(TabRounding))
      + JHelper::PoorMansSerializer::SerializeGenericNumber<false,true,float>(PRM(WindowRounding))
      + JHelper::PoorMansSerializer::EndSerialize();
   return Serialized;
}

std::string CE::Style::SIndentation::SerializeToJson(const std::string& ObjectName)
{
   std::string Serialized
      = JHelper::PoorMansSerializer::StartSerialize(ObjectName.empty() == false ? ObjectName : R"("Indentation" : )")
      + JHelper::PoorMansSerializer::SerializeGenericNumber<true,true,float>(PRM(IndentationOffset))
      + JHelper::PoorMansSerializer::EndSerialize();
   return Serialized;
}

std::string CE::Style::SSpacing::SerializeToJson(const std::string& ObjectName)
{
   std::string Serialized
      = JHelper::PoorMansSerializer::StartSerialize(ObjectName.empty() == false ? ObjectName : R"("Spacing" : )")
      + JHelper::PoorMansSerializer::SerializeVec2<true,true>(PRM(ItemSpacing))
      + JHelper::PoorMansSerializer::SerializeVec2<false,true>(PRM(ItemInnerSpacing))
      + JHelper::PoorMansSerializer::EndSerialize();
   return Serialized;
}

std::string CE::Style::SSizing1D::SerializeToJson(const std::string& ObjectName)
{
   std::string Serialized
      = JHelper::PoorMansSerializer::StartSerialize(ObjectName.empty() == false ? ObjectName : R"("SizingF" : )"  )
      + JHelper::PoorMansSerializer::SerializeGenericNumber<true,true,float>(PRM(ScrollbarSize))
      + JHelper::PoorMansSerializer::SerializeGenericNumber<false,true,float>(PRM(ChildBorderSize))
      + JHelper::PoorMansSerializer::SerializeGenericNumber<false,true,float>(PRM(FrameBorderSize))
      + JHelper::PoorMansSerializer::SerializeGenericNumber<false,true,float>(PRM(GrabMinSize))
      + JHelper::PoorMansSerializer::SerializeGenericNumber<false,true,float>(PRM(PopupBorderSize))
      + JHelper::PoorMansSerializer::SerializeGenericNumber<false,true,float>(PRM(TabBorderSize))
      + JHelper::PoorMansSerializer::SerializeGenericNumber<false,true,float>(PRM(WindowBorderSize))
      + JHelper::PoorMansSerializer::SerializeGenericNumber<false,true,float>(PRM(SeparatorTextBorderSize))
      + JHelper::PoorMansSerializer::SerializeGenericNumber<false,true,float>(PRM(TabBarBorderSize))
      + JHelper::PoorMansSerializer::EndSerialize();
   return Serialized;
}

std::string CE::Style::SSizing2D::SerializeToJson(const std::string& ObjectName)
{
   std::string Serialized
      = JHelper::PoorMansSerializer::StartSerialize(ObjectName.empty() == false ? ObjectName : R"("SizingV" : )")
      + JHelper::PoorMansSerializer::SerializeVec2<false,true>(PRM(WindowMinSize))
      + JHelper::PoorMansSerializer::EndSerialize();
   return Serialized;
}

std::string CE::Style::SAngling::SerializeToJson(const std::string& ObjectName)
{
   std::string Serialized
      = JHelper::PoorMansSerializer::StartSerialize(ObjectName.empty() == false ? ObjectName : R"("Angling" : )")
      + JHelper::PoorMansSerializer::SerializeGenericNumber<true,true,float>(PRM(TableAngledHeadersAngle))
      + JHelper::PoorMansSerializer::EndSerialize();
   return Serialized;
}

std::string CE::Style::SAlignment::SerializeToJson(const std::string& ObjectName)
{
   std::string Serialized
      = JHelper::PoorMansSerializer::StartSerialize(ObjectName.empty() == false ? ObjectName : R"("Angling" : )")
      + JHelper::PoorMansSerializer::SerializeVec2<true,true>(PRM(ButtonTextAlign))
      + JHelper::PoorMansSerializer::SerializeVec2<false,true>(PRM(SelectableTextAlign))
      + JHelper::PoorMansSerializer::SerializeVec2<false,true>(PRM(SeparatorTextAlign))
      + JHelper::PoorMansSerializer::SerializeVec2<false,true>(PRM(WindowTitleAlign))
      + JHelper::PoorMansSerializer::SerializeVec2<false,true>(PRM(TableAngledHeadersTextAlign))
      + JHelper::PoorMansSerializer::EndSerialize();
   return Serialized;
}

std::string CE::Style::Compound::SerializeToJson(const std::string& ObjectName)
{
   std::string Serialized
      = JHelper::PoorMansSerializer::StartSerialize(ObjectName.empty() == false ? ObjectName : R"("StyleCompound" : )")
      + Alpha.SerializeToJson()
      + Padding.SerializeToJson()
      + Rounding.SerializeToJson()
      + Indentation.SerializeToJson()
      + Spacing.SerializeToJson()
      + Sizing1D.SerializeToJson()
      + Sizing2D.SerializeToJson()
      + Angling.SerializeToJson()
      + Alignment.SerializeToJson()
      + JHelper::PoorMansSerializer::EndSerialize();
   return Serialized;
   
   return std::string(); // @todo
}

CE::Style::Compound UIHandler::Stylings[CE::Style::StylingSelector::End] =
   {
      //Global
      CE::Style::Compound
      {
         CE::Style::SPadding{}, CE::Style::SRounding{},
         CE::Style::SIndentation{}, CE::Style::SSpacing{},
         CE::Style::SSizing1D{}, CE::Style::SSizing2D{}, CE::Style::SAngling{},
         CE::Style::SAlignment{}, CE::Style::SAlpha{}
      },
      
      //MainMenu
      CE::Style::Compound
      {
         CE::Style::SPadding{UNIFORM_ARGS_ANY(2, -1), PDVec2{20,20}, UNIFORM_ARGS_ANY(1, -1)},
         CE::Style::SRounding{UNIFORM_ARGS_ANY(3, -1) , UNIFORM_ARGS_ANY(1, 20.0f), UNIFORM_ARGS_ANY(3, -1)},
         CE::Style::SIndentation{UNIFORM_ARGS_ANY(1, -1)},
         CE::Style::SSpacing{UNIFORM_ARGS_ANY(1, -1), PDVec2{10, 10}},
         CE::Style::SSizing1D{50.f, UNIFORM_ARGS_ANY(8, -1)},
         CE::Style::SSizing2D{},
         CE::Style::SAngling{UNIFORM_ARGS_ANY(1, -1)},
         CE::Style::SAlignment{UNIFORM_ARGS_ANY(5, -1)},
         CE::Style::SAlpha{UNIFORM_ARGS_ANY(2, -1)}
      },


      //CharEd
      CE::Style::Compound
      {
         CE::Style::SPadding{UNIFORM_ARGS_ANY(4, -1)},
         CE::Style::SRounding{UNIFORM_ARGS_ANY(7, -1)},
         CE::Style::SIndentation{UNIFORM_ARGS_ANY(1, -1)},
         CE::Style::SSpacing{UNIFORM_ARGS_ANY(2, -1)},
         CE::Style::SSizing1D{UNIFORM_ARGS_ANY(9, -1)},
         CE::Style::SSizing2D{PDVec2(600, 500)},
         CE::Style::SAngling{UNIFORM_ARGS_ANY(1, -1)},
         CE::Style::SAlignment{UNIFORM_ARGS_ANY(5, -1)},
         CE::Style::SAlpha{UNIFORM_ARGS_ANY(2, -1)}         
      },

      //CharSlct
      CE::Style::Compound
      {
         CE::Style::SPadding{UNIFORM_ARGS_ANY(4, -1)},
         CE::Style::SRounding{UNIFORM_ARGS_ANY(7, -1)},
         CE::Style::SIndentation{UNIFORM_ARGS_ANY(1, -1)},
         CE::Style::SSpacing{UNIFORM_ARGS_ANY(2, -1)},
         CE::Style::SSizing1D{UNIFORM_ARGS_ANY(9, -1)},
         CE::Style::SSizing2D{UNIFORM_ARGS_ANY(1, -1)},
         CE::Style::SAngling{UNIFORM_ARGS_ANY(1, -1)},
         CE::Style::SAlignment{UNIFORM_ARGS_ANY(5, -1)},
         CE::Style::SAlpha{UNIFORM_ARGS_ANY(2, -1)}  
      },

      //RaceEd
      CE::Style::Compound
      {

         CE::Style::SPadding{UNIFORM_ARGS_ANY(4, -1)},
         CE::Style::SRounding{UNIFORM_ARGS_ANY(7, -1)},
         CE::Style::SIndentation{UNIFORM_ARGS_ANY(1, -1)},
         CE::Style::SSpacing{UNIFORM_ARGS_ANY(2, -1)},
         CE::Style::SSizing1D{UNIFORM_ARGS_ANY(9, -1)},
         CE::Style::SSizing2D{PDVec2(500, 600)},
         CE::Style::SAngling{UNIFORM_ARGS_ANY(1, -1)},
         CE::Style::SAlignment{UNIFORM_ARGS_ANY(5, -1)},
         CE::Style::SAlpha{UNIFORM_ARGS_ANY(2, -1)}    
      },

      //ClassEd
      CE::Style::Compound
      {
         CE::Style::SPadding{UNIFORM_ARGS_ANY(4, -1)},
         CE::Style::SRounding{UNIFORM_ARGS_ANY(7, -1)},
         CE::Style::SIndentation{UNIFORM_ARGS_ANY(1, -1)},
         CE::Style::SSpacing{UNIFORM_ARGS_ANY(2, -1)},
         CE::Style::SSizing1D{UNIFORM_ARGS_ANY(9, -1)},
         CE::Style::SSizing2D{PDVec2(600, 450)},
         CE::Style::SAngling{UNIFORM_ARGS_ANY(1, -1)},
         CE::Style::SAlignment{UNIFORM_ARGS_ANY(5, -1)},
         CE::Style::SAlpha{UNIFORM_ARGS_ANY(2, -1)}   
      }
      
   };  