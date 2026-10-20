#include <iostream>
#include <random>
#include "Helpers.h"
#include "Utilities.h"
#include "Kasino.h"

#include "StateController.h"
#include "Stats.h"
#include "MenuController.h"

void InitializeStakeBets(Player& aPlayer, std::array<int, 3>& aStandardBets, std::array<int, 3>& aStakeBets)
{
    constexpr int LOW_STAKES_MIN_BET = 1;
    constexpr int LOW_STAKES_MAX_BET = 30;
    constexpr int HIGH_STAKES_MIN_BET = 50;

    int highestBet = aPlayer.money;

    aStandardBets = {LOW_STAKES_MIN_BET, highestBet};
    aStakeBets = {LOW_STAKES_MIN_BET, LOW_STAKES_MAX_BET, HIGH_STAKES_MIN_BET};
}

int main()
{
    Dice dice;

    bool hasEnteredName = false;
    const char* playerName = "N/A";
    Player player = {Constants::startingBalance, playerName, hasEnteredName};

    PlayerStats playerStats;
    Game game;

    std::array<ETableType, Constants::tableCount> aMinigameTypes = {
        ETableType::GuessTheDiceSum, ETableType::OddOrEven, ETableType::YesOrNo, ETableType::HigherOrLower,
        ETableType::Roulette
    };
    
    InitializeStakeBets(player, player.standardBets, player.stakeBets);

    Casino aCasino = {dice, player, playerStats, game, aMinigameTypes};
    aCasino.Initialize();

    ResetStats(aCasino);
    ResetBalance(aCasino);
    if (!aCasino.game.isQuitting)
    {
        ChangeState(EGameState::MainMenu, aCasino);
    }

    return 0;
}

void RefuseGame(bool& aCantPlay, Casino& aCasino)
{
    aCantPlay = true;
    WriteLine("As you walk over to the table, the guards immediately escort you away.");
    WriteLine("You start to wonder why. You eventually land on the question 'Maybe I've won too much?'.");
    Pause();
    ClearConsole();
    ChangeState(EGameState::Game, aCasino);
}

void ResetBalance(Casino& aCasino)
{
    aCasino.player.money = Constants::startingBalance;
}

void BroadcastPlayerBalance(bool aStylize, Casino& aCasino)
{
    if (!aCasino.game.isGameOver)
    {
        if (aStylize)
        {
            DrawBreakerLine();
        }

        std::cout << aCasino.player.name << " : $" << aCasino.player.money << '\n';
    }
}

bool IsValidName(char* aS)
{
    // Time to fix this baloney
    return strlen(aS) >= Constants::playerNameMinSize && strlen(aS) <= Constants::playerNameMaxSize && strspn(
        aS, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\x8F\x86\x84\x8E\x94\x99") == strlen(aS);
}

// Resets all stats after the player has lost the entire game.
void ResetGame(Casino& aCasino)
{
    aCasino.Reset();

    aCasino.playerStats.wins = 0;
    aCasino.playerStats.losses = 0;
    aCasino.playerStats.gamesPlayed = 0;

    ResetBalance(aCasino);
    ResetStats(aCasino);
}

void AddBalance(int aAmount, int& aMinigame, Casino& aCasino)
{
    aAmount = Min(aAmount, 0);
    aCasino.player.money += aAmount;
    aMinigame += aAmount;
}

void RemoveBalance(int aAmount, int& aMinigame, Casino& aCasino)
{
    aAmount = Min(aAmount, 1);
    aCasino.player.money -= aAmount;
    aCasino.player.money = Min(aCasino.player.money, 0);
    aMinigame += aAmount;
}

void GameOver(Casino& aCasino)
{
    ClearConsole();
    aCasino.game.isGameOver = true;
    Exit(aCasino);

    WriteLine("SECURITY GUARD: 'Hey! You're completely out of cash!'");
    Pause();
    WriteLine("Two burly guards lift you by your collar and throw you out onto the street into the rain.");
    Pause();
    WriteLine("The heavy casino doors slam shut behind you!");
    Pause();
    WriteLine("Are you proud of yourself?");
    Pause();
    WriteLine("==========================================================================================");
    WriteLine("GAME OVER. You have been kicked out of the casino.\n");
    
    Pause();
    Exit(aCasino);
}

void Exit(Casino& aCasino)
{
    aCasino.game.isQuitting = true;
}

void TauntOrImpress(Casino& aCasino, int aWinAmount, int aLossAmount, int aImpressWinAmt, int aTauntLossAmt)
{
    int aWinLossDifference = aWinAmount - aLossAmount;

    bool isImpressed = HasExceeded(aWinLossDifference, aImpressWinAmt);
    bool isTaunting = HasSubceeded(aWinLossDifference, -aTauntLossAmt);
    
    if (isImpressed)
    {
        std::cout << "Well, " << aCasino.player.name << ". Seems like you're on a winning streak! Keep it up!\n";
    }
    else if (isTaunting)
    {
        std::cout << "Sad to see you struggling, " << aCasino.player.name <<
            ". Can't tell if you're naive or just plain unlucky!\n";
    }
    else if (!isImpressed && !isTaunting)
    {
        std::cout << "Welcome " << aCasino.player.name << '\n';
    }
}

bool AskPlayerAgain(bool aIsInGame, Casino& aCasino)
{
    ClearConsole();

    WriteLine("Would you like to play again? (y/n) ");
    char input;
    std::cin >> input;

    bool hasPickedYes = IsCharacter(input, 'Y');

    while (!hasPickedYes)
    {
        if (IsCharacter(input, 'N'))
        {
            if (!aIsInGame)
            {
                aCasino.game.isQuitting = true;
            }
            else
            {
                ClearInput();
                ClearConsole();
                aCasino.EnterGameMenu(aCasino.game.currentMinigame);
            }
            break;
        }

        std::cin >> input;
        hasPickedYes = IsCharacter(input, 'Y');
    }

    if (hasPickedYes)
    {
        ClearConsole();
        Pick(aCasino.game.currentMinigame + 1, true, aCasino);
    }

    return true;
}

void About(Casino& aCasino)
{
    WriteLine("STORY");
    DrawMenuLine();
    WriteLine("In this game, you play as a gambling addict.");
    WriteLine("You told everyone in your family that you are a\nrecovering addict. But you lied.");
    DrawMenuLine();
    WriteLine("HOW TO PLAY");
    DrawMenuLine();
    WriteLine("Your goal is to win as much money as possible.");
    WriteLine("However, if you run out of money, you'll lose the game.");
    WriteLine("(Stats reset once you lose the game or cash out)");

    Pause();
    ClearConsole();
    ChangeState(EGameState::MainMenu, aCasino);
}

void CashOut(Casino& aCasino)
{
    int cantPlayAmount = 0;
    if (aCasino.guessTheDiceSumTable.CantPlay()) cantPlayAmount++;
    if (aCasino.oddOrEvenTable.CantPlay()) cantPlayAmount++;
    if (aCasino.yesOrNoTable.CantPlay()) cantPlayAmount++;
    if (aCasino.higherOrLowerTable.CantPlay()) cantPlayAmount++;
    if (aCasino.rouletteTable.CantPlay()) cantPlayAmount++;

    if (cantPlayAmount != Constants::tableCount)
    {
        WriteLine("'It's too early to cash out' is what your inner voice is telling you.");
        WriteLine("Maybe your inside voice is right...");
        WriteLine("(Get kicked out from every game to cash out!)");
        Pause();
        ClearConsole();
        ChangeState(EGameState::Game, aCasino);
        return;
    }

    std::cout << "You decided to cash out $" << aCasino.player.money << '\n';
    Pause();
    WriteLine("Before you walk out the door, you turn around and the game master dead in the eye.");
    Pause();
    WriteLine("Your grin widens, as if you were to mock the casino for everything they put you through.");
    Pause();
    WriteLine("You still didn't change your ways after coming home. You decided to go to a different casino.");
    Pause();
    ClearConsole();
    ResetGame(aCasino);
    ChangeState(EGameState::MainMenu, aCasino);
}


void SwitchTo(EOptions aOption, Casino& aCasino)
{
    switch (aOption)
    {
    case EOptions::CashOut:
        CashOut(aCasino);
        break;
    case EOptions::Stats:
        ShowStats(aCasino);
        Pause();
        Menu::EnterMainMenu(aCasino);
        break;
    case EOptions::About:
        About(aCasino);
        break;
    case EOptions::Quit:
        Exit(aCasino);
        break;
    }
}