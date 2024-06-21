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


/** @brief max */
static constexpr int MaxV(const int CompareA, const int CompareB)
{
   return (CompareB < CompareA) ? CompareA : CompareB;
}

/** @brief min */
static constexpr int MinV(const int compareA, const int compareB)
{
   return (compareA < compareB) ? compareA : compareB;
}

/** @brief Find Max of minimum value vs. result of Min(x Max), keeps result in range [Min <= X <= Max] */
static constexpr int ClampData(const int inValue, const int minValue, const int maxValue)
{
   return MaxV(MinV(inValue, maxValue), minValue);
};

namespace Platonic
{
   enum Dice : char { E4D = 4, E6D = 6, E8D = 8, E12D = 12, E20D = 20 };
}

constexpr int PLATONIC_SOLIDS[5] = {
   Platonic::Dice::E4D, Platonic::Dice::E6D, Platonic::Dice::E8D, Platonic::Dice::E12D, Platonic::Dice::E20D
};

// Park Miller linear congruential generator
int LCGPM(int* State)
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
   };

   void Roll(int& Results[TDiceCount])
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
      Roll(Results);

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

private:
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

namespace Character
{
   namespace Spec
   {
   // @todo make use of this if I have time, Chaotic good, etc..
   enum Alignment : unsigned char
   {
                     /* Lawful          Neutral            Chaotic */
      /* Good    */     LawGood=0,      NeutralGood,       ChaosGood,
      /* Neutral */     LawNeutral,     NeutralNeutral,    ChaosNeutral,
      /* Evil    */     LawEvil,        NeutralEvil,       ChaosEvil
   };
   }
};

namespace Character
{
   namespace Spec
   {
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


struct CharacterClass
{
   CharacterClass(int ClassID, Character::Spec::StatType InPrimeStat, Platonic::Dice InHitDieShape, int Seed = DEFAULT_START_STATE)
      : ID(ClassID), PrimeStat(InPrimeStat), HitDieShape(InHitDieShape), _HitDice(InHitDieShape, Seed)
   {
   };

public:
   /** @brief Class Name. */
   GenericName Name; // @todo
   /** @brief Class ID. */
   int ID = INVALID_INDEX;
   /** @brief Class's Prime Stat'. */
   Character::Spec::StatType PrimeStat;
   /** @brief Platonic solid (shape of die)*/
   Platonic::Dice HitDieShape = Platonic::Dice::E4D;

private:
   UHitDice _HitDice;
   friend struct DnDCharacter;
};

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

typedef char*(StrcpyFP)(char *dest, const char *src);

inline void PassThroughStrCpy(StrcpyFP StrCpy, GenericName& __restrict Target, const GenericName& __restrict Src)
{
   StrCpy(&Target[0], &Src[0]);
}

static int IncrementalID = 0;
struct CharacterRace
{
private:
   CharacterRace(const GenericName& InName, const GenericName& InRaceAbilityName )
   {
      PoorMansStringCopy(Name, InName);
      PoorMansStringCopy(AbilityName, InRaceAbilityName);

      NewID();
   }
   CharacterRace(StrcpyFP StrCpy, const GenericName& InName, const GenericName& InRaceAbilityName )
   {
      StrCpy(Name, InName);
      StrCpy(AbilityName, InRaceAbilityName);

      NewID();
   }
   void NewID() { ID = IncrementalID++ % MAX_ID; }
public:
   
   /** @brief Race Name. */
   GenericName Name; // @todo
   /** @brief Minimum stats for race. */
   int MinimumStats[Character::Spec::SpecSize]{};
   /** @brief Stat modifiers for race. */
   int Modifiers[Character::Spec::SpecSize]{};

   /** @brief Race ability Name. */
   GenericName AbilityName; // @todo
   /** @brief Granted classes for this race */
   int AllowedClassIDs[MAX_ALLOWED_CLASSES]{INVALID_INDEX};

private:
   /** @brief Race ID. */
   int ID = INVALID_INDEX;
};

struct StatRollReturn
{
   int RollValue = 0;
   int ModifiedValue = 0;
};


typedef struct DnDCharacter
{
private:
   DnDCharacter(const CharacterRace& InitRace, int ClassID, Character::Spec::StatType ClassPrimaryStat, Platonic::Dice HitDieShape,
                int Seed,
                int StartingGoldFactor = 10)
      : Race(InitRace), Class(CharacterClass{ClassID, ClassPrimaryStat, HitDieShape, Seed})
   {
      DRoller3d6 Roller(Seed);

      // Roll stats
      Character::Spec::StatType Step = Character::Spec::StatType::Strength;
      for (; Step <= Character::Spec::StatType::Charisma;)
      {
         AbilityScores[Step] = RollStat(Roller, Step);
         Step = Character::Spec::StatType{Step + 1};
      }

      // Roll starting gold
      Gold = Roller.RollAddAndMultiply(StartingGoldFactor); // 3d6 * StartingGoldFactor : defaults to 3d6 * 10

      Hitpoints = RecalculateHitPoints();
   }


   /** @brief Rolls a value for the requested stat and then modifies it based on the races minimum stat value for said stat and the races stat modifiers for said stat */
   StatRollReturn RollStat(DRoller3d6& Roller, Character::Spec::StatType SelectedStat)
   {
      const int MinimumStat = Race.MinimumStats[SelectedStat];
      const int StatMod = Race.Modifiers[SelectedStat];

      StatRollReturn Return;

      Return.RollValue = Roller.RollAndAdd();
      Return.ModifiedValue = ClampData(Return.RollValue + StatMod, MinimumStat, 30);
   }

public:
   /** @brief Create a character base on the stack. Calls ctro which rolls all stats, rolls starting gold and lastly rolls our hitpoints */
   static DnDCharacter ConstructOnStack(bool& RaceAndClassWasCompatible, const CharacterRace& InitRace, int ClassID,
                                        Character::Spec::StatType ClassPrimaryStat, Platonic::Dice HitDieShape, int Seed)
   {
      RaceAndClassWasCompatible = false;
      for (int Step = 0; Step < MAX_ALLOWED_CLASSES;)
      {
         if (InitRace.AllowedClassIDs[Step++] != ClassID)
         {
            continue;
         }

         RaceAndClassWasCompatible = true;
         break;
      }

      return RaceAndClassWasCompatible
                ? DnDCharacter{InitRace, ClassID, ClassPrimaryStat, HitDieShape, Seed}
                : DnDCharacter{CharacterRace{}, -1, Character::Spec::StatType{}, Platonic::Dice{}, -1};
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
      const int RaceModifierConstitution = Race.Modifiers[Character::Spec::StatType::Constitution];
      const int FinalConstitution = AbilityScores[Character::Spec::StatType::Constitution].RollValue + RaceModifierConstitution;

      int HitPointsModifier = ConModifiers[FinalConstitution];
      return RollHitDie() + HitPointsModifier;
   }

   /** @brief Character Name. */
   GenericName Name; // @todo
   
   /** @brief Character ID, suggest to use hash. */
   int CharacterID = 0;

   /** @brief Characted Race */
   CharacterRace Race{};

   /** @brief Character Class, possible selections determined by the given race */
   CharacterClass Class{INVALID_INDEX, Character::Spec::StatType::Intelligence, Platonic::Dice::E6D};

   /** @brief Ability Scores */
   StatRollReturn AbilityScores[Character::Spec::SpecSize]{};

   /** @brief Has no bearing to anything as of yet */
   Character::Spec::Alignment Alignment = Character::Spec::Alignment::ChaosGood;

   /** @brief Characters gold */
   int Gold = 0;

   /** @brief Characters hitpoints */
   int Hitpoints = 0;
} DnDCharacter_t;

#endif // RULESET_H
