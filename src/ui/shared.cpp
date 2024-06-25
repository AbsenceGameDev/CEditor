#include "imgui.h"
#include "imgui_internal.h"

#include "../../inc/ui/shared.h"
#include "../../inc/ui/widgets.h"
#include "../../inc/ui/handler.h"

//
// Behaviours (via delegate)
// Selection callback behaviour base
bool UI::Callbacks::SelectInclusive(ButtonPayload& Payload)
{
   return Payload.CallingVisualElement->IsSelected = Payload.CallingVisualElement->IsSelected == false;
}

void UI::Callbacks::SelectExclusive(ButtonPayload& Payload)
{
   // Only allow one selected at a time
   Payload.CallingVisualElement->IsSelected = true;

   TemplateElementValueIt CurrentSelectionTemplateKeyIt = UIHandler::CurrentSelectedElementPerTemplate_BackMapping().find(
      Payload.CallingVisualElement);
   const bool FoundCurrentTemplate = CurrentSelectionTemplateKeyIt != UIHandler::CurrentSelectedElementPerTemplate_BackMapping().end() &&
      CurrentSelectionTemplateKeyIt->second != nullptr;
   if (FoundCurrentTemplate == false) { return; }

   TemplateElementKeyIt LastSelectionTemplateKeyIt = UIHandler::LastSelectedElementPerTemplate().find(CurrentSelectionTemplateKeyIt->second);
   const bool FoundLastTemplate = LastSelectionTemplateKeyIt != UIHandler::LastSelectedElementPerTemplate().end() &&
      LastSelectionTemplateKeyIt->second != nullptr;
   if (FoundLastTemplate == false) { return; }

   LastSelectionTemplateKeyIt->second->IsSelected = false;
}

//
// Selection callback behaviours -- Class Editor
void UI::Callbacks::Class::SelectPrimeStat(ButtonPayload& Payload)
{
   SelectExclusive(Payload);

   if (Payload.CallingVisualElement->IsSelected)
   {
      UIHandler::EditableClass.PrimeStat = static_cast<Spec::Character::StatType>(Payload.CallingVisualElement->EnumSelectorValue);
   }
   else
   {
      UIHandler::EditableClass.PrimeStat = Spec::Character::StatType::Charisma;
   }
}

void UI::Callbacks::Class::SelectHitDie(ButtonPayload& Payload)
{
   SelectExclusive(Payload);

   if (Payload.CallingVisualElement->IsSelected)
   {
      UIHandler::EditableClass.HitDieShape = static_cast<Platonic::Dice>(Payload.CallingVisualElement->EnumSelectorValue);
   }
   else
   {
      UIHandler::EditableClass.HitDieShape = Platonic::Dice::E6D;
   }
}

//
// Selection callback behaviours -- Race Editor
void UI::Callbacks::Race::SelectMinimumStat(ButtonPayload& Payload)
{
   SelectInclusive(Payload);

   if (Payload.CallingVisualElement->IsSelected)
   {
      const Spec::Character::StatType StatType = static_cast<Spec::Character::StatType>(Payload.CallingVisualElement->EnumSelectorValue);
      UIHandler::EditableRace.MinimumStats[StatType] = Payload.CallingVisualElement->CurrentValue;
   }
   else
   {
      const Spec::Character::StatType StatType = static_cast<Spec::Character::StatType>(Payload.CallingVisualElement->EnumSelectorValue);
      UIHandler::EditableRace.MinimumStats[StatType] = 0;
   }
}


void UI::Callbacks::Race::SelectModifier(ButtonPayload& Payload)
{
   SelectInclusive(Payload);

   if (Payload.CallingVisualElement->IsSelected)
   {
      Spec::Character::StatType StatType = static_cast<Spec::Character::StatType>(Payload.CallingVisualElement->EnumSelectorValue);
      UIHandler::EditableRace.Modifiers[StatType] = Payload.CallingVisualElement->CurrentValue;
   }
   else
   {
      Spec::Character::StatType StatType = static_cast<Spec::Character::StatType>(Payload.CallingVisualElement->EnumSelectorValue);
      UIHandler::EditableRace.Modifiers[StatType] = 0;
   }
}

void UI::Callbacks::Race::SelectAllowedClass(ButtonPayload& Payload)
{
   SelectInclusive(Payload);


   int SelectorValue = Payload.CallingVisualElement->EnumSelectorValue;
   if (SelectorValue == INVALID_INDEX) { return; }

   if (Payload.CallingVisualElement->IsSelected)
   {
      UIHandler::EditableRace.AllowedClassIDs[SelectorValue] = Payload.CallingVisualElement->CurrentValue;
   }
   else
   {
      UIHandler::EditableRace.AllowedClassIDs[SelectorValue] = 0;
   }
}

void UI::Callbacks::Character::SelectRace(ButtonPayload& Payload)
{
   SelectExclusive(Payload);

   if (Payload.CallingVisualElement->IsSelected)
   {
      const int RaceID = Payload.CallingVisualElement->CurrentValue;
      UIHandler::EditableCharacter.Race.ID = RaceID;

      RaceTreeIt It = EditorCore::MappedRaces.find(RaceID);
      if (It == EditorCore::MappedRaces.end()) { return; }

      CharacterRace& Race = It->second;
      UIHandler::EditableCharacter.Race = Race;
   }
   else
   {
      UIHandler::EditableCharacter.Race = CharacterRace{};
   }
}

void UI::Callbacks::Character::SelectClass(ButtonPayload& Payload)
{
   SelectExclusive(Payload);

   if (Payload.CallingVisualElement->IsSelected)
   {
      const int ClassID = Payload.CallingVisualElement->CurrentValue;
      UIHandler::EditableCharacter.Class.ID = ClassID;
   }
   else
   {
      UIHandler::EditableCharacter.Class = CharacterClass{};
   }
}

//
// Selection callback behaviours -- Character Editor
void UI::Callbacks::Character::SelectAlignment(ButtonPayload& Payload)
{
   SelectExclusive(Payload);

   if (Payload.CallingVisualElement->IsSelected)
   {
      UIHandler::EditableCharacter.Alignment = static_cast<Spec::Character::Alignment>(Payload.CallingVisualElement->EnumSelectorValue);
   }
   else // nothing is selected / was deselected
   {
      UIHandler::EditableCharacter.Alignment = Spec::Character::NeutralNeutral;
   }
}

void UI::Callbacks::Character::UpdateStatsEntry(ButtonPayload& ButtonPayload)
{
   GridElement* Widget = ButtonPayload.CallingVisualElement;
   const Spec::Character::StatType StatSelector =  static_cast<Spec::Character::StatType>(Widget->EnumSelectorValue);

   Widget->CurrentValue = UIHandler::EditableCharacter.AbilityScores[StatSelector].ModifiedValue;
}

void UI::Callbacks::Character::SelectStat(ButtonPayload& ButtonPayload)
{
   SelectExclusive(ButtonPayload);

}

//
// Character picker
void UI::Callbacks::CharacterPicker::SelectCharacter(ButtonPayload& Payload)
{
   SelectExclusive(Payload);

   if (Payload.CallingVisualElement->IsSelected)
   {
      const int CharacterID = Payload.CallingVisualElement->CurrentValue;
      CharTreeIt It = EditorCore::MappedCharacters.find(CharacterID);
      if (It == EditorCore::MappedCharacters.end()) { return; }

      StowedCharacterForPicker(&It->second);
   }
   else
   {
      StowedCharacterForPicker(&UIHandler::EditableCharacter);
   }
}
