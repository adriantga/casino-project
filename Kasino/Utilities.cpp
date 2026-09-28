#include <iostream>
#include "Helpers.h"
#include <random>
#include "Utilities.h"
#include "Minigame.h"

#include "Kasino.h"
#include "StateController.h"
#include "Stats.h"

constexpr int GLOBAL_LOWER_CASE_OFFSET = 32;

// Initialize random number generator
std::random_device rnd;
std::mt19937 distributor(rnd());

int GetRandomNumber(int aMin, int aMax)
{
    std::uniform_int_distribution<int> generator(aMin, aMax);
    return generator(distributor);
}

const char* FromMinigameToChar(EMinigameType& aMinigame)
{
    switch (aMinigame)
    {
    case EMinigameType::GuessTheDiceSum:
        return "Guess The Dice Sum";
    case EMinigameType::OddOrEven:
        return "Odd or Even";
    case EMinigameType::YesOrNo:
        return "Yes or No";
    case EMinigameType::HigherOrLower:
        return "Higher or Lower";
    case EMinigameType::Roulette:
        return "Roulette";
    }

    return "";
}

void DrawTitle(const char aTitleText[])
{
    DrawMenuLine();
    WriteLine(aTitleText);
    DrawMenuLine();
}

int GetBetAmount(Casino& aCasino, int gameIndex, bool& aIsPlayingHighStakes)
{
    Minigame& selectedMinigame = aCasino.minigames[gameIndex];
    
    int playerMoney = aCasino.player.money;
    bool canPlay = !HasSubceeded(playerMoney, selectedMinigame.myMinAllowedBet);
    
    BroadcastPlayerBalance(false, aCasino);
    
    if (!canPlay && aIsPlayingHighStakes && selectedMinigame.myHasStakes)
    {
        ClearConsole();
        WriteLine("As you try to pull out money from your wallet, you notice that your wallet is emptier than you thought.");
        WriteLine("'You are low on funds. Come back when you have more', the security guard says.");
        Pause();
        selectedMinigame.EnterGameMenu(aCasino);
        return selectedMinigame.myMinAllowedBet;
    }
    
    WriteLine("How much are you betting?");

    int result;
    std::cin >> result;

    while (std::cin.fail())
    {
        std::cout << "Please pick a valid number!" << '\n';
        ClearInput();
        std::cin >> result;
    }
    
    bool isBetBiggerThanBank = HasExceeded(result, playerMoney);
    while (isBetBiggerThanBank || result < selectedMinigame.myMinAllowedBet || result > selectedMinigame.myMaxAllowedBet)
    {
        if (isBetBiggerThanBank)
        {
            std::cout << "You don't have the money you're betting!" << '\n';
        }
        else
        {
            std::cout << "Invalid bet($" << selectedMinigame.myMinAllowedBet << "-" << selectedMinigame.myMaxAllowedBet <<
                ")\n";    
            ClearInput();
        }
        
        std::cin >> result;
        isBetBiggerThanBank = HasExceeded(result, playerMoney);
    }

    ClearInput();
    return result;
}

void DrawMenu(Casino& aCasino, int& aInput, const char aTitleText[], const char aOptions[], int aNumOptions,
              const char aExtraOptions[], bool aShowBalance)
{
    DrawTitle(aTitleText);
    WriteLine(aOptions);

    if (std::strlen(aExtraOptions) > 0)
    {
        DrawBreakerLine(false);
        WriteLine(aExtraOptions);
    }

    DrawMenuLine();
    if (aShowBalance) BroadcastPlayerBalance(false, aCasino);

    ForceInput(aInput);

    aInput = Clamp(aInput, 1, aNumOptions);

    ClearInput();
    ClearConsole();
}

int RollDie()
{
    return GetRandomNumber(1, 6);
}

void DrawBreakerLine(bool aNewLine)
{
    WriteLine("--------------------------------------", aNewLine);
}

void DrawMenuLine()
{
    WriteLine("===========================================================");
}

void RollDice(Casino& aCasino)
{
    aCasino.dice.die1 = RollDie();
    aCasino.dice.die2 = RollDie();
    aCasino.dice.diceSum = aCasino.dice.die1 + aCasino.dice.die2;
}

bool IsCharacter(char aInput, char aValue)
{
    return aInput == aValue || aInput == ToLower(aValue);
}

int GetRoll()
{
    return GetRandomNumber(1, 100);
}

void ClearInput()
{
    std::cin.clear();
    std::cin.ignore(10000, '\n');
}

int Max(int aValue, int aMax)
{
    if (aValue > aMax)
    {
        return aMax;
    }

    return aValue;
}

int Min(int aValue, int aMin)
{
    if (aValue < aMin)
    {
        return aMin;
    }

    return aValue;
}

int Clamp(int aValue, int aMin, int aMax)
{
    return Max(Min(aValue, aMin), aMax);
}

bool IsInRange(int aValue, int aMin, int aMax)
{
    return aValue >= aMin && aValue <= aMax;
}

bool HasSubceeded(int aCurrent, int aTarget)
{
    return aCurrent < aTarget;
}

bool HasExceeded(int aCurrent, int aTarget)
{
    return aCurrent > aTarget;
}

bool IsEven(int aValue)
{
    return aValue % 2 == 0;
}

void ClearConsole()
{
    system("cls");
}

void Pause()
{
    system("pause");
}

void WriteLine(const char aTextToType[], bool aNewLine)
{
    std::cout << aTextToType;
    if (aNewLine) std::cout << '\n';
}

void BroadcastDiceResult(Casino& aCasino, bool aShowSum)
{
    WriteLine("--------------- RESULT ---------------");

    std::cout << "DIE 1 - " << aCasino.dice.die1 << '\n';
    std::cout << "DIE 2 - " << aCasino.dice.die2 << '\n';

    if (aShowSum)
    {
        std::cout << "SUM - " << aCasino.dice.diceSum << '\n';
    }

    DrawBreakerLine();
}

void BroadcastWinOrLoss(Casino& aCasino, bool aIsWinner, int aWinAmount, int aLoseAmount,
                        int& aMinigame)
{
    Pause();

    DrawBreakerLine();
    if (aIsWinner)
    {
        std::cout << "You won $" << aWinAmount << "!" << '\n';
        AddBalance(aWinAmount, aMinigame, aCasino);
    }
    else
    {
        WriteLine("You didn't win anything this time.");
        RemoveBalance(aLoseAmount, aMinigame, aCasino);

        if (aCasino.player.money == 0)
        {
            Pause();
            GameOver(aCasino);
            Exit(aCasino);
            return;
        }
    }


    AddPlayedGame(aCasino, aIsWinner);
    BroadcastPlayerBalance(true, aCasino);
    
    Pause();
    ClearInput();
    ClearConsole();
    
    ShowStats(aCasino);
    
    Pause();
    ClearConsole();
    aCasino.minigames[aCasino.game.currentMinigame].EnterGameMenu(aCasino);
}

std::array<int, 12> BuildRow(int aRowStart)
{
    std::array<int, 12> result;

    int num = aRowStart;

    for (int i = 0; i < result.size(); i++)
    {
        result[i] = num;
        num += 3;
    }

    return result;
}

bool IsMatching(int current[], int target[])
{
    return current == target;
}

char ToLower(char aCharacter)
{
    return aCharacter + GLOBAL_LOWER_CASE_OFFSET;
}

void ForceInput(int& aInput)
{
    std::cin >> aInput;
    while (std::cin.fail())
    {
        WriteLine("Invalid input! Please only enter numbers.");
        ClearInput();
        std::cin >> aInput;
    }
}
