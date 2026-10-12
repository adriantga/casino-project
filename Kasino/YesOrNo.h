#pragma once
#include "Helpers.h"

class YesOrNo
{
	MinigameSettings mySettings;
	Minigame myBase = {};
	
	int GetRewardMultiplier() const { return mySettings.rewardMultiplier; }
    int GetWinLimit() const { return mySettings.winLimit; }
	
	void PlayTable(Casino& aCasino);
    void ShowRules(Casino& aCasino);
    void OnPlay(Casino& aCasino);
	void ForceRange();
	
	int myRangeStart = 0;
	int myRangeEnd = 0;
public:
	int myBet = 0;
	void Initialize();
    void EnterGameMenu(Casino& aCasino);
    void Reset();
};
