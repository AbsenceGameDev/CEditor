#pragma once
#ifndef SETTINGUI_H
#define SETTINGUI_H

#include "imgui.h"

#include "../editor_core.h"
#include "../../inc/dataformat.h"
#include "../../inc/ruleset.h"

namespace Settings
{
   enum GridElement
   {
      AllowSelection      = 0b0001,
      AllowNegativeValues = 0b0010,
      DisplayValue        = 0b0100,
         
      NOT_SET = 0b0111000
   };
   
}

namespace Override
{
   namespace Behaviour
   {
      using Category = Settings::GridElement;
      
      template<Override::Behaviour::Category CompareBit>
      inline bool IsOverriddenBehaviour(int Flags) { return false; };

      template<Override::Behaviour::Category CompareBit>
      inline bool GetOverriddenState(int Flags) { return false; };      
      
      template<Override::Behaviour::Category SetBit, bool TSetTrue>
      inline int OverrideBehaviour(int Flags) { return Flags; };

      template<Override::Behaviour::Category SetBit>
      inline int ResetOverrideState(int Flags) { return Flags; };            
   };
}

template<> inline bool Override::Behaviour::IsOverriddenBehaviour <Override::Behaviour::Category::AllowSelection>(int Flags)
{ return (Flags & 0b00010000) == 0; };
template<> inline bool Override::Behaviour::IsOverriddenBehaviour<Override::Behaviour::Category::AllowNegativeValues>(int Flags)
{ return (Flags & 0b00100000) == 0; };
template<> inline bool Override::Behaviour::IsOverriddenBehaviour<Override::Behaviour::Category::DisplayValue>(int Flags)
{ return (Flags & 0b01000000) == 0; };

template<> inline bool Override::Behaviour::GetOverriddenState<Override::Behaviour::Category::AllowSelection>(int Flags)
{ return (Flags & Override::Behaviour::Category::AllowSelection) > 0; };
template<> inline bool Override::Behaviour::GetOverriddenState<Override::Behaviour::Category::AllowNegativeValues>(int Flags)
{ return (Flags & Override::Behaviour::Category::AllowNegativeValues) > 0; };
template<> inline bool Override::Behaviour::GetOverriddenState<Override::Behaviour::Category::DisplayValue>(int Flags)
{ return (Flags & Override::Behaviour::Category::DisplayValue) > 0; };

template<> inline int Override::Behaviour::OverrideBehaviour<Override::Behaviour::Category::AllowSelection, true>(int Flags)
{ return  ((Flags & 0b11101111) | Category::AllowSelection); };
template<> inline int Override::Behaviour::OverrideBehaviour<Override::Behaviour::Category::AllowNegativeValues, true>(int Flags)
{ return  ((Flags & 0b11011111) | Category::AllowNegativeValues) ; };
template<> inline int Override::Behaviour::OverrideBehaviour<Override::Behaviour::Category::DisplayValue, true>(int Flags)
{ return  ((Flags & 0b10111111) | Category::DisplayValue); };   
template<> inline int Override::Behaviour::OverrideBehaviour <Override::Behaviour::Category::AllowSelection, false>(int Flags)
{ return  (Flags & 0b11101110) ; };
template<> inline int Override::Behaviour::OverrideBehaviour<Override::Behaviour::Category::AllowNegativeValues, false>(int Flags)
{ return  ((Flags & 0b11011101)) ; };
template<> inline int Override::Behaviour::OverrideBehaviour <Override::Behaviour::Category::DisplayValue, false>(int Flags)
{ return  ((Flags & 0b10111011)) ;};   

template<> inline int Override::Behaviour::ResetOverrideState<Override::Behaviour::Category::AllowSelection>(int Flags)
{ return  (Flags & 0b11101111) ;}
template<> inline int Override::Behaviour::ResetOverrideState<Override::Behaviour::Category::AllowNegativeValues>(int Flags)
{ return  (Flags & 0b11011111) ; };
template<> inline int Override::Behaviour::ResetOverrideState<Override::Behaviour::Category::DisplayValue>(int Flags)
{ return  (Flags & 0b10111111) ;};   


#endif // SETTINGUI_H
