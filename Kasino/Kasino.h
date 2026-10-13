#pragma once
#include "Helpers.h"

#include "GuessTheDiceSum.h"
#include "HigherOrLower.h"
#include "OddOrEven.h"
#include "Roulette.h"
#include "YesOrNo.h"

struct Casino
{
    Dice& dice;
    Player& player;
    PlayerStats& playerStats;
    Game& game;
    std::array<EMinigameType, Constants::minigameCount>& minigameTypes;
    
    GuessTheDiceSum guessTheDiceSumTable;
    OddOrEven oddOrEvenTable;
    YesOrNo yesOrNoTable;
    HigherOrLower higherOrLowerTable;
    Roulette rouletteTable;

    Casino(Dice& aDice, Player& aPlayer, PlayerStats& aPlayerStats, Game& aGame,
           std::array<EMinigameType, Constants::minigameCount>& aMinigameTypes)
        : dice(aDice), player(aPlayer), playerStats(aPlayerStats), game(aGame), minigameTypes(aMinigameTypes),
          guessTheDiceSumTable(aPlayer, aPlayer.stakeBets),
          higherOrLowerTable(aPlayer, aPlayer.stakeBets)
    {
    }
    
    void Initialize()
    {
        guessTheDiceSumTable.Initialize();
        oddOrEvenTable.Initialize();
        yesOrNoTable.Initialize();
        higherOrLowerTable.Initialize();
        rouletteTable.Initialize();
    }
    
    void Reset()
    {
        guessTheDiceSumTable.Reset();
        oddOrEvenTable.Reset();
        yesOrNoTable.Reset();
        higherOrLowerTable.Reset();
        rouletteTable.Reset();
    }

    void EnterGameMenu(int aGameIndex)
    {
        switch (aGameIndex)
        {
        case Minigames::MINIGAME_GUESS_THE_DICE_SUM:
            guessTheDiceSumTable.EnterGameMenu(*this);
            break;
        case Minigames::MINIGAME_ODD_OR_EVEN:
            oddOrEvenTable.EnterGameMenu(*this);
            break;
        case Minigames::MINIGAME_YES_OR_NO:
            yesOrNoTable.EnterGameMenu(*this);
            break;
        case Minigames::MINIGAME_HIGHER_OR_LOWER:
            higherOrLowerTable.EnterGameMenu(*this);
            break;
        case Minigames::MINIGAME_ROULETTE:
            rouletteTable.EnterGameMenu(*this);
            break;
        }
    }
};

void InitializeStakeBets(Player& aPlayer, std::array<int, 3>& aStandardBets, std::array<int, 3>& aStakeBets);
void ResetBalance(Casino& aCasino);
bool IsValidName(char* aS);
void About(Casino& aCasino);
void Exit(Casino& aCasino);
void CashOut(Casino& aCasino);
void ResetGame(Casino& aCasino);
void BroadcastPlayerBalance(bool aStylize, Casino& aCasino);
void AddBalance(int aAmount, int& aMinigame, Casino& aCasino);
void RemoveBalance(int aAmount, int& aMinigame, Casino& aCasino);
void GameOver(Casino& aCasino);
bool AskPlayerAgain(bool aIsInGame, Casino& aCasino);
void RefuseGame(bool& aCantPlay, Casino& aCasino);
void TauntOrImpress(Casino& aCasino, int aWinAmount, int aLossAmount, int aImpressWinAmt, int aTauntLossAmt);
void SwitchTo(EOptions aOption, Casino& aCasino);
