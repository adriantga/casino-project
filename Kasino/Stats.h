#pragma once

#include "Helpers.h"

void ShowStats(Casino& aCasino);
void ResetStats(Casino& aCasino);
void SetResult(Casino& aCasino, int aMatchIndex, bool aIsWin);
void AddPlayedGame(Casino& aCasino, bool aIsWinner);
void GetMinigameStats(const Casino& aCasino, int aIndex, int& aWinAmount, int& aLossAmount);