#include "../../inc/ui/widgets.h"


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

void LinkedStatCategory::operator()()
{
   UIHandler::DrawTemplates(InnerTemplate);
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
