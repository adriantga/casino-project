#pragma once

#include "Helpers.h"

struct Constants;
struct Casino;
enum class EMinigameType;

class INeedToRemoveThisLater
{
    void PlayGame(Casino& aCasino);
    void ShowRules(Casino& aCasino);
    void OnPlay(Casino& aCasino);

public:
    int myWinAmount = 0;
    int myLossAmount = 0;
    int myWinLimit = 0;

    int myBet = 0;
    
    // I store 3 values because 2 are for low-stakes/no stakes, and 1 is for high stakes.
    std::array<int, 3> myAllowedBets = {0, 0};
    int myMinAllowedBet = 0;
    int myMaxAllowedBet = 0;

    bool myHasStakes = false;

    int myGameIndex = 0;

    bool myCantPlay = false;

    INeedToRemoveThisLater(Player& aPlayer, std::array<int, 3>& aAllowedBets, bool aHasStakes);
    
    bool IsPlayingHighStakes() const { return myIsPlayingHighStakes; }
    void Initialize(const EMinigameType& aMinigameType);
    void EnterGameMenu(Casino& aCasino);
    void UpdateBets(Player& aPlayer);

    void Reset();
};
