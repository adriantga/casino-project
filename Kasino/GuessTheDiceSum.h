#pragma once
#include "Helpers.h"

class GuessTheDiceSum
{
    bool myIsPlayingHighStakes = false;
    
    void PlayTable(Casino& aCasino);
    void ShowRules(Casino& aCasino);
    void OnPlay(Casino& aCasino);
    
public:
    int myWinAmount = 0;
    int myLossAmount = 0;
    int myWinLimit = 0;

    int myBet = 0;
    
    GuessTheDiceSum(Player& aPlayer, std::array<int, 3>& aAllowedBets, bool aHasStakes);
    
    bool IsPlayingHighStakes() const { return myIsPlayingHighStakes; }
    void Initialize(const EMinigameType& aMinigameType);
    void EnterGameMenu(Casino& aCasino);
    void UpdateBets(Player& aPlayer);

    void Reset();
};
