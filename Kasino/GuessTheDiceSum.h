#pragma once
#include "Helpers.h"

class GuessTheDiceSum
{
    MinigameSettings mySettings;
    BettingSettings myBettingSettings = {};
    Minigame myBase = {};
    
    int GetRewardMultiplier() const { return mySettings.rewardMultiplier; }
    int GetWinLimit() const { return mySettings.winLimit; }
    
    void PlayTable(Casino& aCasino);
    void ShowRules(Casino& aCasino);
    void OnPlay(Casino& aCasino);
    
public:
    int myBet = 0;
    
    GuessTheDiceSum() = default;
    GuessTheDiceSum(const Player& aPlayer, const std::array<int, 3>& aAllowedBets);
    
    bool IsPlayingHighStakes() const { return mySettings.isPlayingHighStakes; }
    void Initialize();
    void EnterGameMenu(Casino& aCasino);
    void UpdateBets(const Player& aPlayer);

    void Reset();

    int GetMinAllowedBet() const { return myBettingSettings.minAllowedBet; }
    int GetMaxAllowedBet() const { return myBettingSettings.maxAllowedBet; }
    bool CantPlay() const { return myBase.cantPlay; }
    int GetWinAmount() const { return myBase.winAmount; }
    int GetLossAmount() const { return myBase.lossAmount; }
};