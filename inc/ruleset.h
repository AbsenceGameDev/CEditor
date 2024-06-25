#pragma once
#ifndef RULESET_H
#define RULESET_H

//
// Changlog : 0 
// Starting project with a purely independent ruleset header file & processor header/src file pair

constexpr int DEFAULT_START_STATE = 0x12345;
constexpr int MAX_ALLOWED_CLASSES = 20;
constexpr int INVALID_INDEX = -1;
constexpr int MAX_ID = 0x1000000; // Arbitrary high number, 16 to the power of 6
constexpr int ConModifiers[20] = {-3, -3, -3, -3, -2, -2, -1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3};

constexpr int NameSize = 52;
using GenericName = char[NameSize];

inline void PoorMansStringCopy(GenericName& __restrict Target, const GenericName& __restrict Src)
{
   // This is my own fault for insisting to not use any includes or dependencies in ruleset.h,
   for (int Step = 0; Step < NameSize;)
   {
      Target[Step] = Src[Step];
      if (Src[Step] == '\0') { break; }

      Step++;
   }
}

typedef char*(StrcpyFP)(char* dest, const char* src);

inline void PassThroughStrCpy(StrcpyFP StrCpy, GenericName& __restrict Target, const GenericName& __restrict Src)
{
   StrCpy(&Target[0], &Src[0]);
}


/** @brief max */
template<typename  TType>
static constexpr TType MaxV(const TType CompareA, const TType CompareB)
{
   return (CompareB < CompareA) ? CompareA : CompareB;
}

/** @brief min */
template<typename  TType>
static constexpr TType MinV(const TType compareA, const TType compareB)
{
   return (compareA < compareB) ? compareA : compareB;
}

/** @brief Find Max of minimum value vs. result of Min(x Max), keeps result in range [Min <= X <= Max] */
template<typename  TType>
static constexpr TType ClampData(const TType inValue, const TType minValue, const TType maxValue)
{
   return MaxV(MinV(inValue, maxValue), minValue);
};

namespace Platonic
{
   enum Dice : char { E4D = 4, E6D = 6, E8D = 8, E12D = 12, E20D = 20 };
   enum class DiceIdx : char { E4D = 0, E6D, E8D, E12D, E20D, EMax};
}

constexpr int PLATONIC_SOLIDS[5] = {
   Platonic::Dice::E4D, Platonic::Dice::E6D, Platonic::Dice::E8D, Platonic::Dice::E12D, Platonic::Dice::E20D
};

// Park Miller linear congruential generator
inline int LCGPM(int* State)
{
   constexpr int A = 48271;
   int Low = (*State & 0x7fff) * A; // max: 32,767 * 48,271 = 1,581,695,857 = 0x5e46c371
   int High = (*State >> 15) * A; // max: 65,535 * 48,271 = 3,163,439,985 = 0xbc8e4371
   int X = Low + ((High & 0xffff) << 15) + (High >> 16); // max: 0x5e46c371 + 0x7fff8000 + 0xbc8e = 0xde46ffff
   *State = (X & 0x7fffffff) + (X >> 31);
   return *State;
}

//
// Dice roller
template <int TDiceCount, int TUniformSides>
struct TDiceRoller
{
   TDiceRoller(int OverrideStartState = DEFAULT_START_STATE) : State(OverrideStartState)
   {
   }

   void Roll(int* Results)
   {
      for (int Step = 0; Step < TDiceCount;)
      {
         Results[Step++] = LCGPM(&State) % TUniformSides;
      }
   }

   int RollAndAdd()
   {
      int Result = 0;

      int Results[TDiceCount];
      Roll(&Results[0]);

      for (int Step = 0; Step < TDiceCount;)
      {
         const int StepResult = Results[Step++];
         Result += StepResult != 0 ? StepResult : 1; // Die's can't roll 0
      }

      return Result;
   }

   int RollAddAndMultiply(int Factor = 1)
   {
      return RollAndAdd() * Factor;
   }

   int Count = TDiceCount;
   int SidesPerDie = TUniformSides;

   int State = DEFAULT_START_STATE;
};

#define MakeRoller(DiceCount, UniformSides) using Roller1d4 = TDiceRoller<1, 6>;
// This is dumb, I know
using DRoller1d4 = TDiceRoller<1, PLATONIC_SOLIDS[0]>;
using DRoller2d4 = TDiceRoller<2, PLATONIC_SOLIDS[0]>;
using DRoller3d4 = TDiceRoller<3, PLATONIC_SOLIDS[0]>;

using DRoller1d6 = TDiceRoller<1, PLATONIC_SOLIDS[1]>;
using DRoller2d6 = TDiceRoller<2, PLATONIC_SOLIDS[1]>;
using DRoller3d6 = TDiceRoller<3, PLATONIC_SOLIDS[1]>;

using DRoller1d8 = TDiceRoller<1, PLATONIC_SOLIDS[2]>;
using DRoller2d8 = TDiceRoller<2, PLATONIC_SOLIDS[2]>;
using DRoller3d8 = TDiceRoller<3, PLATONIC_SOLIDS[2]>;

using DRoller1d12 = TDiceRoller<1, PLATONIC_SOLIDS[3]>;
using DRoller2d12 = TDiceRoller<2, PLATONIC_SOLIDS[3]>;
using DRoller3d12 = TDiceRoller<3, PLATONIC_SOLIDS[3]>;

using DRoller1d20 = TDiceRoller<1, PLATONIC_SOLIDS[4]>;
using DRoller2d20 = TDiceRoller<2, PLATONIC_SOLIDS[4]>;
using DRoller3d20 = TDiceRoller<3, PLATONIC_SOLIDS[4]>;

union UHitDice
{
   UHitDice(Platonic::Dice HitDieShape, int Seed = DEFAULT_START_STATE)
   {
      switch (HitDieShape)
      {
      case Platonic::Dice::E4D: Roller1d4 = DRoller1d4{Seed};
         break;
      case Platonic::Dice::E6D: Roller1d6 = DRoller1d6{Seed};
         break;
      case Platonic::Dice::E8D: Roller1d8 = DRoller1d8{Seed};
         break;
      case Platonic::Dice::E12D: Roller1d12 = DRoller1d12{Seed};
         break;
      case Platonic::Dice::E20D: Roller1d20 = DRoller1d20{Seed};
         break;
      }
   }

   DRoller1d4 Roller1d4;
   DRoller1d6 Roller1d6;
   DRoller1d8 Roller1d8;
   DRoller1d12 Roller1d12;
   DRoller1d20 Roller1d20;
};

//
// Character definitions
namespace Spec
{
   namespace Character
   {
      // @todo make use of this if I have time, Chaotic good, etc..
      enum Alignment : unsigned char
      {
         /* Lawful          Neutral            Chaotic */
         /* Good    */     LawGood=0,      NeutralGood,       ChaosGood,
         /* Neutral */     LawNeutral,     NeutralNeutral,    ChaosNeutral,
         /* Evil    */     LawEvil,        NeutralEvil,       ChaosEvil
      };

      enum StatType : unsigned char
      {
         Strength = 0,
         Intelligence,
         Wisdom,
         Dexterity,
         Constitution,
         Charisma
      };

      struct StatConstruct
      {
         StatType Type = StatType::Charisma;
         int Value = 0;
      };

      constexpr int SpecSize = 6;
   }
};

typedef struct BaseCharacterEditorClass
{
   
};

static int IncrementalCID = 0; // @todo remove from being a global, had not time to think on this before

struct CharacterClass : public BaseCharacterEditorClass
{
   CharacterClass(): Name{}, PrimeStat(), _HitDice(Platonic::Dice::E4D, -1)
   {
   }

   CharacterClass(const CharacterClass& Other, int Seed = DEFAULT_START_STATE)
      : PrimeStat(Other.PrimeStat), HitDieShape(Other.HitDieShape), _HitDice(Other.HitDieShape, Seed)
   {
      PoorMansStringCopy(Name, Other.Name);

      if (Other.ID != INVALID_INDEX)
      {
         ID = Other.ID;
         return;
      }
      NewID();
   }

   CharacterClass(const GenericName& InName, Spec::Character::StatType InPrimeStat, Platonic::Dice InHitDieShape, int Seed = DEFAULT_START_STATE)
      : PrimeStat(InPrimeStat), HitDieShape(InHitDieShape), _HitDice(InHitDieShape, Seed)
   {
      PoorMansStringCopy(Name, InName);
      NewID();
   }

   CharacterClass& operator=(CharacterClass const& Other)
   {
      PoorMansStringCopy(this->Name, Other.Name);

      this->ID = Other.ID;
      this->PrimeStat = Other.PrimeStat;
      this->HitDieShape = Other.HitDieShape;
      
      return *this;
      
   }

   void NewID() { ID = IncrementalCID++ % MAX_ID; }

public:
   int GetID() const { return ID; }

   /** @brief Class Name. */
   GenericName Name; // @todo
   /** @brief Class's Prime Stat'. */
   Spec::Character::StatType PrimeStat;
   /** @brief Platonic solid (shape of die)*/
   Platonic::Dice HitDieShape = Platonic::Dice::E4D;

   /** @brief Class ID.
    * @note Needed to move it out of private so nlohmanns library can serialize it */
   int ID = INVALID_INDEX;
private:
   UHitDice _HitDice;
   friend struct DnDCharacter;
};

static int IncrementalRID = 0;

struct CharacterRace : public BaseCharacterEditorClass
{
   CharacterRace(): Name{"Default\0"}, AbilityName{"Default\0"} { }
   
   CharacterRace(const CharacterRace& Other)
   {
      PoorMansStringCopy(this->Name, Other.Name);
      PoorMansStringCopy(this->AbilityName, Other.AbilityName);
      if (Other.ID != INVALID_INDEX)
      {
         this->ID = Other.ID;
         return;
      }
      
      NewID();
   }

   CharacterRace& operator=(CharacterRace const& Other)
   {
      PoorMansStringCopy(this->Name, Other.Name);
      PoorMansStringCopy(AbilityName, Other.AbilityName);

      this->ID = Other.ID;
      for (int Step = 0; Step < 6; Step++)
      {
         this->Modifiers[Step] = Other.Modifiers[Step];
         this->MinimumStats[Step] = Other.MinimumStats[Step];
      }

      for (int Step = 0; Step < 20; Step++)
      {
         this->AllowedClassIDs[Step] = Other.AllowedClassIDs[Step];
      }      
      
      return *this;
      
   }
   
private:
   CharacterRace(const GenericName& InName, const GenericName& InRaceAbilityName)
   {
      PoorMansStringCopy(Name, InName);
      PoorMansStringCopy(AbilityName, InRaceAbilityName);

      NewID();
   }

   CharacterRace(StrcpyFP StrCpy, const GenericName& InName, const GenericName& InRaceAbilityName)
   {
      StrCpy(Name, InName);
      StrCpy(AbilityName, InRaceAbilityName);

      NewID();
   }

public:
   void NewID() { ID = ++IncrementalRID % MAX_ID; }

   static CharacterRace ConstructOnStack() { return CharacterRace{}; }

   /** @brief Gets a copy of the ID. */
   int GetID() const { return ID; }

   /** @brief Race Name. */
   GenericName Name; // @todo
   /** @brief Minimum stats for race. */
   int MinimumStats[Spec::Character::SpecSize]{};
   /** @brief Stat modifiers for race. */
   int Modifiers[Spec::Character::SpecSize]{};

   /** @brief Race ability Name. */
   GenericName AbilityName; // @todo
   /** @brief Granted classes for this race */
   int AllowedClassIDs[MAX_ALLOWED_CLASSES]{INVALID_INDEX};

   /** @brief Race ID.
    * @note Needed to move it out of private so nlohmanns library can serialize it */
   int ID = INVALID_INDEX;
};

struct StatRollReturn
{
   int RollValue = 0;
   int ModifiedValue = 0;
};


static int IncrementalID = 0;

typedef struct DnDCharacter : public BaseCharacterEditorClass
{
   DnDCharacter(): Name{} { }

   DnDCharacter(int InID) : Name{}, ID(InID) {}

private:
   DnDCharacter(
      const GenericName& InName,
      const CharacterRace& InitRace,
      const CharacterClass& InClass,
      int Seed,
      int StartingGoldFactor = 10)
      : Race(InitRace), Class(InClass, Seed)
   {
      PoorMansStringCopy(Name, InName);
      NewID();

      DRoller3d6 Roller(Seed);

      // Roll stats
      Spec::Character::StatType Step = Spec::Character::StatType::Strength;
      for (; Step <= Spec::Character::StatType::Charisma;)
      {
         AbilityScores[Step] = RollStat(Roller, Step);
         Step = static_cast<Spec::Character::StatType>(Step + 1);
      }

      RollHPAndGold(StartingGoldFactor, Roller);

      CachedSeed = Roller.State;
   }



   /** @brief Rolls a value for the requested stat and then modifies it based on the races minimum stat value for said stat and the races stat modifiers for said stat */
   StatRollReturn RollStat(DRoller3d6& Roller, Spec::Character::StatType SelectedStat)
   {
      const int MinimumStat = Race.MinimumStats[SelectedStat];
      const int StatMod = Race.Modifiers[SelectedStat];

      StatRollReturn Return;

      Return.RollValue = Roller.RollAndAdd();
      Return.ModifiedValue = ClampData(Return.RollValue + StatMod, MinimumStat, 30);
      return Return;
   }

public:
   void NewID() { ID = IncrementalRID++ % MAX_ID; }
   int GetID() const { return ID; }

   /** @brief Create a character base on the stack. Calls ctro which rolls all stats, rolls starting gold and lastly rolls our hitpoints */
   static DnDCharacter ConstructOnStack(const GenericName& InName, bool& RaceAndClassWasCompatible, const CharacterRace& InitRace, const CharacterClass& InitClass, int Seed)
   {
      RaceAndClassWasCompatible = false;
      for (int Step = 0; Step < MAX_ALLOWED_CLASSES;)
      {
         if (InitRace.AllowedClassIDs[Step++] != InitClass.GetID())
         {
            continue;
         }

         RaceAndClassWasCompatible = true;
         break;
      }

      return RaceAndClassWasCompatible
                ? DnDCharacter{{InName}, InitRace, InitClass, Seed}
                : DnDCharacter{{InName}, CharacterRace::ConstructOnStack(), CharacterClass{}, -1};
   }

   int RollHitDie()
   {
      switch (Class.HitDieShape)
      {
      case Platonic::Dice::E4D: return Class._HitDice.Roller1d4.RollAndAdd();
      case Platonic::Dice::E6D: return Class._HitDice.Roller1d6.RollAndAdd();
      case Platonic::Dice::E8D: return Class._HitDice.Roller1d8.RollAndAdd();
      case Platonic::Dice::E12D: return Class._HitDice.Roller1d12.RollAndAdd();
      case Platonic::Dice::E20D: return Class._HitDice.Roller1d20.RollAndAdd();
      }
      return 0;
   }

   /** @brief  Recalculates the hitpoints */
   int RecalculateHitPoints()
   {
      const int RaceModifierConstitution = Race.Modifiers[Spec::Character::StatType::Constitution];
      const int FinalConstitution = AbilityScores[Spec::Character::StatType::Constitution].RollValue + RaceModifierConstitution;

      int HitPointsModifier = ConModifiers[FinalConstitution];
      return RollHitDie() + HitPointsModifier;
   }

   void RollHPAndGold(int StartingGoldFactor, DRoller3d6 Roller)
   {
      // Roll starting gold
      Gold = Roller.RollAddAndMultiply(StartingGoldFactor); // 3d6 * StartingGoldFactor : defaults to 3d6 * 10

      Hitpoints = RecalculateHitPoints();
   }

   void RerollStats(int Seed)
   {
      // Roll stats
      DRoller3d6 Roller(Seed);

      Spec::Character::StatType Step = Spec::Character::StatType::Strength;
      for (; Step <= Spec::Character::StatType::Charisma;)
      {
         AbilityScores[Step] = RollStat(Roller, Step);
         Step = static_cast<Spec::Character::StatType>(Step + 1);
      }      
   }

   /** @brief Character Name. */
   GenericName Name; // @todo

   /** @brief Characted Race */
   CharacterRace Race = CharacterRace::ConstructOnStack();

   /** @brief Character Class, possible selections determined by the given race */
   CharacterClass Class{{}, Spec::Character::StatType::Intelligence, Platonic::Dice::E6D};

   /** @brief Ability Scores */
   StatRollReturn AbilityScores[Spec::Character::SpecSize]{};

   /** @brief Has no bearing to anything as of yet */
   Spec::Character::Alignment Alignment = Spec::Character::Alignment::ChaosGood;

   /** @brief Characters gold */
   int Gold = 0;

   /** @brief Characters hitpoints */
   int Hitpoints = 0;

   /** @brief Character ID, suggest to use hash but for now incremental static ID.
    * @note Needed to move it out of private so nlohmanns library can serialize it */
   int ID = 0;


   int CachedSeed = DEFAULT_START_STATE;
} DnDCharacter_t;

#endif // RULESET_H
