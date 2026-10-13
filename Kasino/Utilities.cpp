#include <iostream>
#include "Helpers.h"
#include <random>
#include "Utilities.h"

#include "Kasino.h"
#include "StateController.h"
#include "Stats.h"

constexpr int GLOBAL_LOWER_CASE_OFFSET = 32;

// Initialize random number generator
std::random_device rnd;
std::mt19937 distributor(rnd());

int GetRandomNumber(int aMin, int aMax)
{
    std::uniform_int_distribution<int> aGenerator(aMin, aMax);
    return aGenerator(distributor);
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

void GetBetLimits(Casino& aCasino, int aGameIndex, int& aMinBet, int& aMaxBet, bool& aHasStakes)
{
    switch (aGameIndex)
    {
    case Minigames::MINIGAME_GUESS_THE_DICE_SUM:
        aMinBet = aCasino.guessTheDiceSumTable.GetMinAllowedBet();
        aMaxBet = aCasino.guessTheDiceSumTable.GetMaxAllowedBet();
        aHasStakes = true;
        break;
    case Minigames::MINIGAME_HIGHER_OR_LOWER:
        aMinBet = aCasino.higherOrLowerTable.GetMinAllowedBet();
        aMaxBet = aCasino.higherOrLowerTable.GetMaxAllowedBet();
        aHasStakes = true;
        break;
    case Minigames::MINIGAME_ODD_OR_EVEN:
    case Minigames::MINIGAME_YES_OR_NO:
    case Minigames::MINIGAME_ROULETTE:
    default:
        aMinBet = aCasino.player.standardBets[0];
        aMaxBet = aCasino.player.money;
        aHasStakes = false;
        break;
    }
}

int GetBetAmount(Casino& aCasino, int aGameIndex, bool& aIsPlayingHighStakes)
{
    int aMinAllowedBet = 0;
    int aMaxAllowedBet = 0;
    bool aHasStakes = false;
    GetBetLimits(aCasino, aGameIndex, aMinAllowedBet, aMaxAllowedBet, aHasStakes);
    
    int aPlayerMoney = aCasino.player.money;
    bool aCanPlay = !HasSubceeded(aPlayerMoney, aMinAllowedBet);
    
    BroadcastPlayerBalance(false, aCasino);
    
    if (!aCanPlay && aIsPlayingHighStakes && aHasStakes)
    {
        ClearConsole();
        WriteLine("As you try to pull out money from your wallet, you notice that your wallet is emptier than you thought.");
        WriteLine("'You are low on funds. Come back when you have more', the security guard says.");
        Pause();
        aCasino.EnterGameMenu(aGameIndex);
        return aMinAllowedBet;
    }
    
    WriteLine("How much are you betting?");

    int aResult;
    std::cin >> aResult;

    while (std::cin.fail())
    {
        std::cout << "Please pick a valid number!" << '\n';
        ClearInput();
        std::cin >> aResult;
    }
    
    bool aIsBetBiggerThanBank = HasExceeded(aResult, aPlayerMoney);
    while (aIsBetBiggerThanBank || aResult < aMinAllowedBet || aResult > aMaxAllowedBet)
    {
        if (aIsBetBiggerThanBank)
        {
            std::cout << "You don't have the money you're betting!" << '\n';
        }
        else
        {
            std::cout << "Invalid bet($" << aMinAllowedBet << "-" << aMaxAllowedBet <<
                ")\n";    
            ClearInput();
        }
        
        std::cin >> aResult;
        aIsBetBiggerThanBank = HasExceeded(aResult, aPlayerMoney);
    }

    ClearInput();
    return aResult;
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
    aCasino.EnterGameMenu(aCasino.game.currentMinigame);
}

std::array<int, 12> BuildRow(int aRowStart)
{
    std::array<int, 12> aResult;

    int aNum = aRowStart;

    for (int aI = 0; aI < aResult.size(); aI++)
    {
        aResult[aI] = aNum;
        aNum += 3;
    }

    return aResult;
}

bool IsMatching(int aCurrent[], int aTarget[])
{
    return aCurrent == aTarget;
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
