#pragma once
#include "Helpers.h"

class OddOrEven
{
    MinigameSettings mySettings;
    Minigame myBase = {};

    int GetRewardMultiplier() const { return mySettings.rewardMultiplier; }
    int GetWinLimit() const { return mySettings.winLimit; }
    
    void PlayTable(Casino& aCasino);
    void ShowRules(Casino& aCasino);
    void OnPlay(Casino& aCasino);
public:
    int myBet = 0;
    
    void Initialize();
    void Reset();
    void EnterGameMenu(Casino& aCasino);

    bool CantPlay() const { return myBase.cantPlay; }
    int GetWinAmount() const { return myBase.winAmount; }
    int GetLossAmount() const { return myBase.lossAmount; }
};