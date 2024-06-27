#include "../../inc/ui/widgets.h"


#include "imgui_internal.h"
#include "../../inc/ui/settings.h"
#include "../../inc/ui/handler.h"

Template UI::Class::PrimeStatTemplates    = {PrimeStatConstructorList()};
Template UI::Class::PlatonicDiceTemplates = {PlatonicDiceConstructorList()};
Template UI::Race::ClassTemplates          = {};
Template UI::Race::MinimumStatsTemplates  = {MinimumStatsConstructorList()};
Template UI::Race::ModifierTemplates      = {ModifierConstructorList()};

ImVec2   UI::Character::MaxSizeLimited3_3      = ImVec2{3 * (CharacterEditorStyle.ElementSizes.x + CharacterEditorStyle.Padding.x), 3 * (CharacterEditorStyle.ElementSizes.y + CharacterEditorStyle.Padding.y)};
Template UI::Character::CharacterStatTemplates = {CharacterStatConstructorList()};
Template UI::Character::AlignmentTemplates     = {AlignmentConstructorList(), MaxSizeLimited3_3};
Template UI::Character::LoadedCharacterTemplates = {};      
Template UI::Character::ClassTemplates           = {};
Template UI::Character::RaceTemplates            = {};

Template UI::Menu::ClassMenuControlTemplates         = {ClassMenuControlConstructorList()};
Template UI::Menu::RaceMenuControlTemplates          = {RaceMenuControlConstructorList()};
Template UI::Menu::CharacterMenuControlTemplates     = {CharacterMenuControlConstructorList()};
Template UI::Menu::CharacterSelectorControlTemplates = {CharacterSelectorControlConstructorList()};


std::set<GridElement> UI::Class::PrimeStatConstructorList()
{
   return
   {
      GridElement{ClassStatLabels[Spec::Character::StatType::Strength], DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Strength, false, true, false, DummyStateDelegate, &UI::Callbacks::Class::SelectPrimeStat , DummyCallbackTarget},
      GridElement{ClassStatLabels[Spec::Character::StatType::Intelligence], DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Intelligence, false, true, false, DummyStateDelegate, &UI::Callbacks::Class::SelectPrimeStat, DummyCallbackTarget},
      GridElement{ClassStatLabels[Spec::Character::StatType::Wisdom], DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Wisdom, false, true, false, DummyStateDelegate, &UI::Callbacks::Class::SelectPrimeStat, DummyCallbackTarget},
      GridElement{ClassStatLabels[Spec::Character::StatType::Dexterity], DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Dexterity, false, true, false, DummyStateDelegate, &UI::Callbacks::Class::SelectPrimeStat, DummyCallbackTarget},
      GridElement{ClassStatLabels[Spec::Character::StatType::Constitution], DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Constitution, false, true, false, DummyStateDelegate, &UI::Callbacks::Class::SelectPrimeStat, DummyCallbackTarget},
      GridElement{ClassStatLabels[Spec::Character::StatType::Charisma], DefaultStyle, INVALID_INDEX, Spec::Character::StatType::Charisma, false, true, false, DummyStateDelegate, &UI::Callbacks::Class::SelectPrimeStat, DummyCallbackTarget}
   };   
}

std::set<GridElement> UI::Class::PlatonicDiceConstructorList()
{
   return
   {
      GridElement{ClassDieLabels[static_cast<uint8_t>(Platonic::DiceIdx::E4D)], DefaultStyle, INVALID_INDEX, Platonic::Dice::E4D,  false, true, false, DummyStateDelegate, &UI::Callbacks::Class::SelectHitDie, DummyCallbackTarget},
      GridElement{ClassDieLabels[static_cast<uint8_t>(Platonic::DiceIdx::E6D)], DefaultStyle, INVALID_INDEX, Platonic::Dice::E6D,  false, true, false, DummyStateDelegate, &UI::Callbacks::Class::SelectHitDie, DummyCallbackTarget},
      GridElement{ClassDieLabels[static_cast<uint8_t>(Platonic::DiceIdx::E8D)], DefaultStyle, INVALID_INDEX, Platonic::Dice::E8D,  false, true, false, DummyStateDelegate, &UI::Callbacks::Class::SelectHitDie, DummyCallbackTarget},
      GridElement{ClassDieLabels[static_cast<uint8_t>(Platonic::DiceIdx::E12D)], DefaultStyle, INVALID_INDEX, Platonic::Dice::E12D, false, true, false, DummyStateDelegate, &UI::Callbacks::Class::SelectHitDie, DummyCallbackTarget},
      GridElement{ClassDieLabels[static_cast<uint8_t>(Platonic::DiceIdx::E20D)], DefaultStyle, INVALID_INDEX,  Platonic::Dice::E20D, false, true, false, DummyStateDelegate, &UI::Callbacks::Class::SelectHitDie, DummyCallbackTarget}
   };   
}

void WText::Draw(CE::Style::Compound& StaticWidgetStyling, const char* fmt, ...)
{
   int result;
   va_list list{};
   va_start(list, fmt);
   StaticWidgetStyling.WrapStyle(
      [&]()
      {
         ImGui::TextV(fmt, list);
      });      
   va_end(list);
}

void Template::Draw()
{
   const ImVec2& CursorScreenPos = ImGui::GetCursorScreenPos();
   const ImVec2& Origin = ImGui::GetCursorPos();

   ImVec2 CalcPos = {0, 0};

   int StepID = 0;
   for (const GridElement& ImmutableElement : Inner)
   {
      Step(ImmutableElement, StepID, CursorScreenPos, Origin, CalcPos);
   }
}

void Template::Step(const GridElement& StepElement, int& StepIdx, const ImVec2& CursorScreenPos, const ImVec2& Origin, ImVec2& CalcPos)
{
      GridElement& MutableElement = const_cast<GridElement&>(StepElement);
      const ElementStyle& Style = MutableElement.Style;

      ImGui::PushID(static_cast<int>(StepIdx++));
      const float HalfPaddingX = Style.Padding.x / 2;
      const float HalfPaddingY = Style.Padding.y / 2;

      const float SizeX = Style.ElementSizes.x;
      const float SizeY = Style.ElementSizes.y;


      //----
      ImGuiWindow* Window = ImGui::GetCurrentWindow();
      const ImGuiID id = Window->GetID(this);

      bool OutHovered = false;
      bool OutHeld = false;
      ImGui::ButtonBehavior(ImRect(CalcPos.x, CalcPos.y, CalcPos.x + SizeX, CalcPos.y + SizeY), id, &OutHovered, &OutHeld,
                            ImGuiButtonFlags_PressedOnClick);
      ImVec2 MousePos = ImGui::GetMousePos();
      MousePos.x -= CursorScreenPos.x;
      MousePos.y -= CursorScreenPos.y;

      
      const bool AllowSelectionWasOverridden = Override::Behaviour::IsOverriddenBehaviour<Override::Behaviour::Category::AllowSelection>(SharedSettingsFlags);
      const bool AllowSelectionOverrideState = Override::Behaviour::GetOverriddenState<Override::Behaviour::Category::AllowSelection>(SharedSettingsFlags);
      const bool AllowSelection = AllowSelectionWasOverridden ? AllowSelectionOverrideState : MutableElement.AllowSelection;
      
      //
      // const bool AllowNegativeValuesWasOverridden = Override::Behaviour::IsOverriddenBehaviour<Override::Behaviour::Category::AllowNegativeValues>(Category->SharedSettings);
      // const bool AllowNegativeValuesOverrideState = Override::Behaviour::GetOverriddenState<Override::Behaviour::Category::AllowNegativeValues>(Category->SharedSettings);
      // bool AllowNegativeValues = AllowSelectionWasOverridden ? AllowSelectionOverrideState : MutableElement.AllowNegativeValues;      

      const bool DisplayValueWasOverridden = Override::Behaviour::IsOverriddenBehaviour<Override::Behaviour::Category::DisplayValue>(SharedSettingsFlags);
      const bool DisplayValueOverrideState = Override::Behaviour::GetOverriddenState<Override::Behaviour::Category::DisplayValue>(SharedSettingsFlags);
      const bool DisplayValue = DisplayValueWasOverridden ? DisplayValueOverrideState : MutableElement.DisplayValue;            

      if (AllowSelection)
      {
         OutHovered = (MousePos.x > CalcPos.x && MousePos.x < CalcPos.x + SizeX && MousePos.y > CalcPos.y && MousePos.y < CalcPos.y +
            SizeY);
         //---- Visualize Selection
         if (OutHovered || MutableElement.IsSelected)
         {
            const float OffsetX = CursorScreenPos.x + HalfPaddingX;
            const float OffsetY = CursorScreenPos.y + HalfPaddingY;

            const ImU32 Column = ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered]);
            CalcPos.x += OffsetX;
            CalcPos.y += OffsetY;
            ImGui::RenderFrame(ImVec2(CalcPos.x, CalcPos.y), ImVec2(CalcPos.x + SizeX, CalcPos.y + SizeY), Column, true, 3.f);
            CalcPos.x -= OffsetX;
            CalcPos.y -= OffsetY;
         }
      }

      ImGui::SetCursorPosX(CalcPos.x + Origin.x + HalfPaddingX);
      ImGui::SetCursorPosY(CalcPos.y + Origin.y + HalfPaddingY);
      // CreateTexture(Element);
      // ImGui::Image(Element.ImageID, ImVec2(50, 50));


      std::string Name = MutableElement.Label;
      Name = Name.substr(4, Name.length() - 4);

      if (AllowSelection)
      {
         const bool DidPressButton = ImGui::Button(Name.c_str(), Style.ElementSizes);
         if (DidPressButton)
         {
            ProcessGridElement(MutableElement);
         }

         // Check if mouse button is still active and if this is an element with a visible value, ignore those with INVALID_INDEX, they are not meant to have values
         if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && DisplayValue)
         {
            const float YAcc = -ImGui::GetIO().MouseDelta.y / 2;
            MutableElement.CurrentValue = ClampData(MutableElement.CurrentValue + static_cast<int>(YAcc),
                                                    MutableElement.AllowNegativeValues ? -20 : 0, 20);
         }
      }
      else
      {
         const float OffsetX = CursorScreenPos.x + HalfPaddingX;
         const float OffsetY = CursorScreenPos.y + HalfPaddingY;

         const ImU32 Column = ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]);
         CalcPos.x += OffsetX;
         CalcPos.y += OffsetY;
         ImGui::RenderFrame(ImVec2(CalcPos.x, CalcPos.y), ImVec2(CalcPos.x + SizeX, CalcPos.y + SizeY), Column, true, 3.f);

         ImGui::PushFont(UIHandler::GlobalLargeFont);
         ImGui::RenderText(ImVec2(CalcPos.x + HalfPaddingX * 0.5f, CalcPos.y + HalfPaddingY * 0.5f), Name.c_str(),
                           Name.c_str() + Name.size());
         ImGui::PopFont();
         CalcPos.x -= OffsetX;
         CalcPos.y -= OffsetY;
      }

      ButtonPayload Payload = {MutableElement};
      MutableElement.OnRefresh(Payload);

      
      //---- Text
      ImGui::SetCursorPosX(CalcPos.x + Origin.x + HalfPaddingX);
      ImGui::SetCursorPosY(CalcPos.y + Origin.y + SizeY);
      ImGui::PushFont(UIHandler::GlobalSmallFont);

      if (DisplayValue)
      {
         ImGui::Text("%02d", MutableElement.CurrentValue);
      }

      ImGui::PopFont();

      //---- Next position
      CalcPos.x += SizeX + HalfPaddingX * 2;

      const float WindowWidth = ImGui::GetWindowWidth();
      const float MaxWidth = MaxSize.x > 0 ? MinV(MaxSize.x, WindowWidth) : WindowWidth;
      if (CalcPos.x + SizeX > MaxWidth)
      {
         CalcPos.x = 0;
         CalcPos.y += SizeY + HalfPaddingY * 2;
      }

      ImGui::PopID();   
}

void Template::ProcessGridElement(GridElement& Element)
{
   TemplateElementKeyIt CurrentSelectionTemplateKeyIt = UIHandler::CurrentSelectedElementPerTemplate().find(this);
   const bool FoundCurrentTemplate = CurrentSelectionTemplateKeyIt != UIHandler::CurrentSelectedElementPerTemplate().end() &&
      CurrentSelectionTemplateKeyIt->first != nullptr;

   TemplateElementKeyIt LastSelectionTemplateKeyIt = UIHandler::LastSelectedElementPerTemplate().find(this);
   const bool FoundLastTemplate = LastSelectionTemplateKeyIt != UIHandler::LastSelectedElementPerTemplate().end() &&
      LastSelectionTemplateKeyIt->first != nullptr;

   {
      GridElement* SelectedGridElement = FoundCurrentTemplate ? UIHandler::CurrentSelectedElementPerTemplate()[this] : nullptr;
      TemplateElementValueIt OverwriteElementIt = UIHandler::LastSelectedElementPerTemplate_BackMapping().find(SelectedGridElement);
      if (FoundLastTemplate == false)
      {
         UIHandler::LastSelectedElementPerTemplate().emplace(this, SelectedGridElement);
         UIHandler::LastSelectedElementPerTemplate_BackMapping().emplace(SelectedGridElement, this);
      }
      else
      {
         LastSelectionTemplateKeyIt->second = SelectedGridElement;
         if (OverwriteElementIt != UIHandler::LastSelectedElementPerTemplate_BackMapping().end())
         {
            UIHandler::LastSelectedElementPerTemplate_BackMapping()[SelectedGridElement] = this;
         }
         else
         {
            UIHandler::LastSelectedElementPerTemplate_BackMapping().emplace(SelectedGridElement, this);
         }
      }
   }

   TemplateElementValueIt OverwriteElementIt = UIHandler::CurrentSelectedElementPerTemplate_BackMapping().find(&Element);
   if (FoundCurrentTemplate == false)
   {
      UIHandler::CurrentSelectedElementPerTemplate().emplace(this, &Element);
      UIHandler::CurrentSelectedElementPerTemplate_BackMapping().emplace(&Element, this);
   }
   else
   {
      CurrentSelectionTemplateKeyIt->second = &Element;
      if (OverwriteElementIt != UIHandler::CurrentSelectedElementPerTemplate_BackMapping().end())
      {
         UIHandler::CurrentSelectedElementPerTemplate_BackMapping()[&Element] = this;
      }
      else
      {
         UIHandler::CurrentSelectedElementPerTemplate_BackMapping().emplace(&Element, this);
      }
   }

   // Always call after making sure we've progressed our states

   ButtonPayload InnerPayload{Element};
   ButtonPayload Payload{Element, nullptr, Element.GetStateDelegate(InnerPayload) == false};
   Element.CallbackTarget(Payload); //Flip state
}


void Template::RemoveGridElement(const GridElement* Consider) 
{
   Template::TemplateIt SelfIt = Inner.find(*Consider);
   const bool IsValidElement = SelfIt != Inner.end();

   bool ElementFound = false;

   if (
      IsValidElement
      && UIHandler::LastSelectedElementPerTemplate().empty() == false
      && UIHandler::CurrentSelectedElementPerTemplate().empty() == false)
   {
      TemplateElementKeyIt LastSelectionTemplateKeyIt = UIHandler::LastSelectedElementPerTemplate().find(this);
      const bool FoundLastTemplate = LastSelectionTemplateKeyIt != UIHandler::LastSelectedElementPerTemplate().end();
      if (FoundLastTemplate)
      {
         TemplateElementValueIt LastElement = UIHandler::LastSelectedElementPerTemplate_BackMapping().find(
            LastSelectionTemplateKeyIt->second);
         const bool FoundLastElement = LastElement != UIHandler::LastSelectedElementPerTemplate_BackMapping().end();
         if (FoundLastElement)
         {
            UIHandler::LastSelectedElementPerTemplate_BackMapping().erase(LastElement);
            ElementFound = true;
         }
         UIHandler::LastSelectedElementPerTemplate().erase(LastSelectionTemplateKeyIt);
      }

      TemplateElementKeyIt CurrentSelectionTemplateKeyIt = UIHandler::CurrentSelectedElementPerTemplate().find(this);
      const bool CurrentLastTemplate = CurrentSelectionTemplateKeyIt != UIHandler::CurrentSelectedElementPerTemplate().end();
      if (CurrentLastTemplate)
      {
         TemplateElementValueIt CurrentElement = UIHandler::CurrentSelectedElementPerTemplate_BackMapping().find(
            CurrentSelectionTemplateKeyIt->second);
         const bool FoundCurrentElement = CurrentElement != UIHandler::CurrentSelectedElementPerTemplate_BackMapping().end();
         if (FoundCurrentElement)
         {
            UIHandler::CurrentSelectedElementPerTemplate_BackMapping().erase(CurrentElement);
            ElementFound = true;
         }
         UIHandler::CurrentSelectedElementPerTemplate().erase(CurrentSelectionTemplateKeyIt);
      }

      if (ElementFound) { Remove(*Consider); }
   }
}


void LinkedStatCategory::operator()()
{
   WidgetStyling.WrapStyle(
      [&]()
      {
         UIHandler::DrawTemplates(InnerTemplate);
      });  
}

void LinkedStatCategory::OnStateUpdate(bool OldState)
{
   InnerTemplate->SharedSettingsFlags = 
   AltState == false
      ? Override::Behaviour::OverrideBehaviour<Override::Behaviour::Category::AllowSelection, true>(InnerTemplate->SharedSettingsFlags)
      : Override::Behaviour::OverrideBehaviour<Override::Behaviour::Category::AllowSelection, false>(InnerTemplate->SharedSettingsFlags);
}

std::set<GridElement> UI::Race::MinimumStatsConstructorList()
{
   return
   {
      GridElement{RaceMinStatLabels[Spec::Character::StatType::Strength], DefaultStyle, 0, Spec::Character::StatType::Strength, false, true, true, DummyStateDelegate, &UI::Callbacks::Race::SelectMinimumStat, DummyCallbackTarget},
      GridElement{RaceMinStatLabels[Spec::Character::StatType::Intelligence], DefaultStyle, 0, Spec::Character::StatType::Intelligence, false, true, true, DummyStateDelegate, &UI::Callbacks::Race::SelectMinimumStat, DummyCallbackTarget},
      GridElement{RaceMinStatLabels[Spec::Character::StatType::Wisdom], DefaultStyle, 0, Spec::Character::StatType::Wisdom, false, true, true, DummyStateDelegate, &UI::Callbacks::Race::SelectMinimumStat, DummyCallbackTarget},
      GridElement{RaceMinStatLabels[Spec::Character::StatType::Dexterity], DefaultStyle, 0, Spec::Character::StatType::Dexterity, false, true, true, DummyStateDelegate, &UI::Callbacks::Race::SelectMinimumStat, DummyCallbackTarget},
      GridElement{RaceMinStatLabels[Spec::Character::StatType::Constitution], DefaultStyle, 0, Spec::Character::StatType::Constitution, false, true, true, DummyStateDelegate, &UI::Callbacks::Race::SelectMinimumStat, DummyCallbackTarget},
      GridElement{RaceMinStatLabels[Spec::Character::StatType::Charisma], DefaultStyle, 0, Spec::Character::StatType::Charisma, false, true, true, DummyStateDelegate, &UI::Callbacks::Race::SelectMinimumStat, DummyCallbackTarget}
   };   
}

std::set<GridElement> UI::Race::ModifierConstructorList()
{
   return
   {
      GridElement{RaceModStatLabels[Spec::Character::StatType::Strength], DefaultStyle, 0, Spec::Character::StatType::Strength, true, true, true, DummyStateDelegate, &UI::Callbacks::Race::SelectModifier, DummyCallbackTarget},
      GridElement{RaceModStatLabels[Spec::Character::StatType::Intelligence], DefaultStyle, 0, Spec::Character::StatType::Intelligence, true, true, true, DummyStateDelegate, &UI::Callbacks::Race::SelectModifier, DummyCallbackTarget},
      GridElement{RaceModStatLabels[Spec::Character::StatType::Wisdom], DefaultStyle, 0, Spec::Character::StatType::Wisdom, true, true, true, DummyStateDelegate, &UI::Callbacks::Race::SelectModifier, DummyCallbackTarget},
      GridElement{RaceModStatLabels[Spec::Character::StatType::Dexterity], DefaultStyle, 0, Spec::Character::StatType::Dexterity, true, true, true, DummyStateDelegate, &UI::Callbacks::Race::SelectModifier, DummyCallbackTarget},
      GridElement{RaceModStatLabels[Spec::Character::StatType::Constitution], DefaultStyle, 0, Spec::Character::StatType::Constitution, true, true, true, DummyStateDelegate, &UI::Callbacks::Race::SelectModifier, DummyCallbackTarget},
      GridElement{RaceModStatLabels[Spec::Character::StatType::Charisma], DefaultStyle, 0, Spec::Character::StatType::Charisma, true, true, true, DummyStateDelegate, &UI::Callbacks::Race::SelectModifier, DummyCallbackTarget}
   };   
}

std::set<GridElement> UI::Character::CharacterStatConstructorList()
{
   return
   {
      GridElement{CharacterStatLabels[Spec::Character::StatType::Strength], DefaultStyle, 0, Spec::Character::StatType::Strength, true, false, true, DummyStateDelegate,&UI::Callbacks::Character::SelectStat, &UI::Callbacks::Character::UpdateStatsEntry},
      GridElement{CharacterStatLabels[Spec::Character::StatType::Intelligence], DefaultStyle, 0, Spec::Character::StatType::Intelligence, true, false, true, DummyStateDelegate,&UI::Callbacks::Character::SelectStat, &UI::Callbacks::Character::UpdateStatsEntry},
      GridElement{CharacterStatLabels[Spec::Character::StatType::Wisdom], DefaultStyle, 0, Spec::Character::StatType::Wisdom, true, false, true, DummyStateDelegate,&UI::Callbacks::Character::SelectStat, &UI::Callbacks::Character::UpdateStatsEntry},
      GridElement{CharacterStatLabels[Spec::Character::StatType::Dexterity], DefaultStyle, 0, Spec::Character::StatType::Dexterity, true, false, true, DummyStateDelegate,&UI::Callbacks::Character::SelectStat, &UI::Callbacks::Character::UpdateStatsEntry},
      GridElement{CharacterStatLabels[Spec::Character::StatType::Constitution], DefaultStyle, 0, Spec::Character::StatType::Constitution, true, false, true, DummyStateDelegate,&UI::Callbacks::Character::SelectStat, &UI::Callbacks::Character::UpdateStatsEntry},
      GridElement{CharacterStatLabels[Spec::Character::StatType::Charisma], DefaultStyle, 0, Spec::Character::StatType::Charisma, true, false, true, DummyStateDelegate,&UI::Callbacks::Character::SelectStat, &UI::Callbacks::Character::UpdateStatsEntry}
   };
}

std::set<GridElement> UI::Character::AlignmentConstructorList()
{
   return 
   {
      GridElement{CharacterAlignmentLabels[Spec::Character::Alignment::LawGood],        CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::LawGood, false, true, false, DummyStateDelegate, &UI::Callbacks::Character::SelectAlignment, DummyCallbackTarget},
      GridElement{CharacterAlignmentLabels[Spec::Character::Alignment::LawNeutral],     CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::LawNeutral, false, true, false, DummyStateDelegate, &UI::Callbacks::Character::SelectAlignment, DummyCallbackTarget},
      GridElement{CharacterAlignmentLabels[Spec::Character::Alignment::LawEvil],        CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::LawEvil, false, true, false, DummyStateDelegate, &UI::Callbacks::Character::SelectAlignment, DummyCallbackTarget},
      GridElement{CharacterAlignmentLabels[Spec::Character::Alignment::NeutralGood],    CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::NeutralGood, false, true, false, DummyStateDelegate, &UI::Callbacks::Character::SelectAlignment, DummyCallbackTarget},
      GridElement{CharacterAlignmentLabels[Spec::Character::Alignment::NeutralNeutral], CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::NeutralNeutral, false, true, false, DummyStateDelegate, &UI::Callbacks::Character::SelectAlignment, DummyCallbackTarget},
      GridElement{CharacterAlignmentLabels[Spec::Character::Alignment::NeutralEvil],    CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::NeutralEvil, false, true, false, DummyStateDelegate, &UI::Callbacks::Character::SelectAlignment, DummyCallbackTarget},
      GridElement{CharacterAlignmentLabels[Spec::Character::Alignment::ChaosGood],      CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::ChaosGood, false, true, false, DummyStateDelegate, &UI::Callbacks::Character::SelectAlignment, DummyCallbackTarget},
      GridElement{CharacterAlignmentLabels[Spec::Character::Alignment::ChaosNeutral],   CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::ChaosNeutral, false, true, false, DummyStateDelegate, &UI::Callbacks::Character::SelectAlignment, DummyCallbackTarget},
      GridElement{CharacterAlignmentLabels[Spec::Character::Alignment::ChaosEvil],      CharacterEditorStyle, INVALID_INDEX, Spec::Character::Alignment::ChaosEvil, false, true, false, DummyStateDelegate, &UI::Callbacks::Character::SelectAlignment, DummyCallbackTarget}
   };
}


std::set<GridElement> UI::Menu::ClassMenuControlConstructorList()
{
   return
   {
      GridElement{MenuSaveLabels[Control::Menu::Save::Class], MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, DummyStateDelegate, &UIHandler::SaveClass, DummyCallbackTarget},
      GridElement{MenuCloseLabels[Control::Menu::Close::ClassEditor], MenuStyle, INVALID_INDEX, INVALID_INDEX,  false, true, false, &UIHandler::GetIsClassEditorOpen, &UIHandler::SetClassEditorOpen, DummyCallbackTarget} 
   };
}


std::set<GridElement> UI::Menu::RaceMenuControlConstructorList()
{
   return 
   {
      GridElement{MenuSaveLabels[Control::Menu::Save::Race], MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, DummyStateDelegate, &UIHandler::SaveRace, DummyCallbackTarget},
      GridElement{MenuCloseLabels[Control::Menu::Close::RaceEditor], MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, &UIHandler::GetIsRaceEditorOpen, &UIHandler::SetRaceEditorOpen, DummyCallbackTarget}
   };
}

std::set<GridElement> UI::Menu::CharacterMenuControlConstructorList()
{
   return 
   {
      GridElement{MenuSaveLabels[Control::Menu::Save::Character], MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, DummyStateDelegate, &UIHandler::SaveCharacter, DummyCallbackTarget} ,
      GridElement{MenuCloseLabels[Control::Menu::Close::CharacterEditor], MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, &UIHandler::GetIsCharacterEditorOpen, &UIHandler::SetCharacterEditorOpen, DummyCallbackTarget}
   };
}

std::set<GridElement> UI::Menu::CharacterSelectorControlConstructorList()
{
   return
   {
      GridElement{MenuLoadLabels[Control::Menu::Load::Character], MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, DummyStateDelegate, &UIHandler::LoadCharacter, DummyCallbackTarget},
      GridElement{MenuCloseLabels[Control::Menu::Close::CharacterSelector], MenuStyle, INVALID_INDEX, INVALID_INDEX, false, true, false, &UIHandler::GetIsCharacterSelectorOpen, &UIHandler::SetCharacterSelectorOpen, DummyCallbackTarget}      
   };
}
