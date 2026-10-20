#pragma once

#include <array>

#include "Kasino.h"

void ForceInput(int& aInput);
bool IsCharacter(char aInput, char aValue);
const char* FromMinigameToChar(ETableType& aTable);
int GetRandomNumber(int aMin, int aMax);
void ClearConsole();
void ClearInput();
void Pause();
int RollDie();
void RollDice(Casino& aCasino);
int Min(int aValue, int aMin);
int Max(int aValue, int aMax);
int Clamp(int aValue, int aMin, int aMax);
bool IsInRange(int aValue, int aMin, int aMax);
bool HasSubceeded(int aCurrent, int aTarget);
bool HasExceeded(int aCurrent, int aTarget);
bool IsEven(int aValue);
int GetRoll();
void DrawMenuLine();
void DrawBreakerLine(bool aNewLine = true);
void WriteLine(const char aTextToType[], bool aNewLine = true);
void DrawMenu(Casino& aCasino, int& aInput, const char aTitleText[], const char aOptions[], int aNumOptions = 3,
              const char aExtraOptions[] = "", bool aShowBalance = false);
void DrawTitle(const char aTitleText[]);
void GetBetLimits(Casino& aCasino, int aGameIndex, int& aMinBet, int& aMaxBet, bool& aHasStakes);
int GetBetAmount(Casino& aCasino, int aGameIndex, bool& aIsPlayingHighStakes);
void BroadcastDiceResult(Casino& aCasino, bool aShowSum = false);
void BroadcastWinOrLoss(Casino& aCasino, bool aIsWinner, int aWinAmount, int aLoseAmount,
                        int& aMinigame);
bool IsMatching(int aCurrent[], int aTarget[]);
char ToLower(char aCharacter);
std::array<int, 12> BuildRow(int aRowStart);