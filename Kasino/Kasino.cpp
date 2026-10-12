#include <iostream>
#include <random>
#include "Helpers.h"
#include "Utilities.h"
#include "Kasino.h"

#include "INeedToRemoveThisLater.h"
#include "StateController.h"
#include "Stats.h"

/*
 * GOALS:
 * - Create separate classes for each game mode/table (can't inherit which I don't agree with)
 * - Optimize the game (eventually)
 */
void InitializeStakeBets(Player& aPlayer, std::array<int, 3>& standardBets, std::array<int, 3>& stakeBets)
{
    constexpr int LOW_STAKES_MIN_BET = 1;
    constexpr int LOW_STAKES_MAX_BET = 30;
    constexpr int HIGH_STAKES_MIN_BET = 50;

    int highestBet = aPlayer.money;

    standardBets = {LOW_STAKES_MIN_BET, highestBet};
    stakeBets = {LOW_STAKES_MIN_BET, LOW_STAKES_MAX_BET, HIGH_STAKES_MIN_BET};
}

int main()
{
    Dice dice;

    bool playerHasEnteredName = false;
    const char* playerName = "N/A";
    Player player = {Constants::startingBalance, playerName, playerHasEnteredName};

    PlayerStats playerStats;
    Game game;

    std::array<EMinigameType, Constants::minigameCount> minigameTypes = {
        EMinigameType::GuessTheDiceSum, EMinigameType::OddOrEven, EMinigameType::YesOrNo, EMinigameType::HigherOrLower,
        EMinigameType::Roulette
    };
    
    InitializeStakeBets(player, player.standardBets, player.stakeBets);
    
    std::array<INeedToRemoveThisLater, minigameTypes.size()> minigames = {
        INeedToRemoveThisLater{player, player.stakeBets, true}, INeedToRemoveThisLater{player, player.standardBets, false}, INeedToRemoveThisLater{player, player.stakeBets, true},
        INeedToRemoveThisLater{player, player.standardBets, false}, INeedToRemoveThisLater{player, player.standardBets, false}
    };
    for (int i = 0; i < minigames.size(); i++)
    {
        minigames[i].Initialize(minigameTypes[i]);
    }

    Casino aCasino = {dice, player, playerStats, game, minigames, minigameTypes};

    ResetStats(aCasino);
    ResetBalance(aCasino);
    if (!aCasino.game.isQuitting)
    {
        ChangeState(EStates::MainMenu, aCasino);
    }

    return 0;
}

void RefuseGame(bool& aCantPlay, Casino& casino)
{
    aCantPlay = true;
    WriteLine("As you walk over to the table, the guards immediately escort you away.");
    WriteLine("You start to wonder why. You eventually land on the question 'Maybe I've won too much?'.");
    Pause();
    ClearConsole();
    ChangeState(EStates::Game, casino);
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
    for (INeedToRemoveThisLater& minigame : aCasino.minigames)
    {
        minigame.Reset();
    }

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
    int winLossDifference = aWinAmount - aLossAmount;

    bool shouldBeImpressed = HasExceeded(winLossDifference, aImpressWinAmt);
    bool shouldTaunt = HasSubceeded(winLossDifference, -aTauntLossAmt);
    
    if (shouldBeImpressed)
    {
        std::cout << "Well, " << aCasino.player.name << ". Seems like you're on a winning streak! Keep it up!\n";
    }
    else if (shouldTaunt)
    {
        std::cout << "Sad to see you struggling, " << aCasino.player.name <<
            ". Can't tell if you're naive or just plain unlucky!\n";
    }
    else if (!shouldBeImpressed && !shouldTaunt)
    {
        std::cout << "Welcome " << aCasino.player.name << '\n';
    }
}

// TODO: REPURPOSE
bool AskPlayerAgain(bool aIsInGame, Casino& aCasino)
{
    ClearConsole();

    WriteLine("Would you like to play again? (y/n) ");
    char input;
    std::cin >> input;

    bool isYes = IsCharacter(input, 'Y');

    while (!isYes)
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
                aCasino.minigames[aCasino.game.currentMinigame].EnterGameMenu(aCasino);
            }
            break;
        }

        std::cin >> input;
        isYes = IsCharacter(input, 'Y');
    }

    if (isYes)
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
    ChangeState(EStates::MainMenu, aCasino);
}

void CashOut(Casino& aCasino)
{
    int cantPlayAmount = 0;
    for (INeedToRemoveThisLater minigame : aCasino.minigames)
    {
        if (minigame.myCantPlay)
        {
            cantPlayAmount++;
        }
    }

    // Um... alright.
    if (cantPlayAmount != aCasino.minigames.size())
    {
        WriteLine("'It's too early to cash out' is what your inner voice is telling you.");
        WriteLine("Maybe your inside voice is right...");
        WriteLine("(Get kicked out from every game to cash out!)");
        Pause();
        ClearConsole();
        ChangeState(EStates::Game, aCasino);
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
    ChangeState(EStates::MainMenu, aCasino);
}

void EnterNamePicker(Casino& aCasino)
{
    if (aCasino.player.hasEnteredName)
    {
        EnterGamePicker(aCasino);
        return;
    }

    char confirmName = 'Y';
    char denyName = 'N';

    WriteLine("What's your name? (Must be between 2-16 characters)");

    char myPlayerNameInput[17];
    std::cin >> myPlayerNameInput;

    while (std::cin.fail() || !IsValidName(myPlayerNameInput))
    {
        WriteLine("Please enter letters only! / Ensure the name is between 2-16 characters.");
        ClearInput();
        std::cin >> myPlayerNameInput;
    }

    ClearInput();

    std::cout << "Your name is: " << myPlayerNameInput << ". Continue? (y to confirm, n to deny)" << '\n';

    char confirmInput;
    std::cin >> confirmInput;
    while (std::cin.fail())
    {
        ClearInput();
        std::cin >> confirmInput;
        WriteLine("Please confirm or deny!");
    }

    bool hasConfirmedName = IsCharacter(confirmInput, confirmName);
    bool hasDeniedName = IsCharacter(confirmInput, denyName);

    if (!hasConfirmedName && !hasDeniedName)
    {
        std::cin >> confirmInput;

        hasConfirmedName = IsCharacter(confirmInput, confirmName);
        hasDeniedName = IsCharacter(confirmInput, denyName);
    }

    if (hasConfirmedName)
    {
        aCasino.player.hasEnteredName = true;
        aCasino.player.name = myPlayerNameInput;

        ClearConsole();
        EnterGamePicker(aCasino);
    }
    else if (hasDeniedName)
    {
        ClearConsole();
        EnterNamePicker(aCasino);
    }
}

void EnterGamePicker(Casino& aCasino)
{
    const int OPTIONS = int(aCasino.minigameTypes.size()) + 2;

    int input;

    // Sooner or later I'll have to change this based on 
    DrawTitle("GAME PICKER");

    int minigameCount = int(aCasino.minigameTypes.size());

    for (int i = 0; i < minigameCount; i++)
    {
        EMinigameType minigameType = aCasino.minigameTypes[i];
        std::cout << "[" << (i + 1) << "] " << FromMinigameToChar(minigameType) << '\n';
    }

    DrawBreakerLine();

    std::cout << "[" << (minigameCount + 1) << "] " << "Cash Out" << '\n';
    std::cout << "[" << (minigameCount + 2) << "] " << "Back To Menu" << '\n';

    DrawMenuLine();

    BroadcastPlayerBalance(false, aCasino);

    ForceInput(input);

    input = Clamp(input, 1, OPTIONS);

    Pick(input, true, aCasino);
}

void EnterMainMenu(Casino& aCasino)
{
    int input;
    DrawMenu(aCasino, input, "THE ULTIMATE CASINO", "[1] Play Game\n[2] About\n[3] Stats\n[4] Quit", 4);
    Pick(input, false, aCasino);
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
        EnterMainMenu(aCasino);
        break;
    case EOptions::About:
        About(aCasino);
        break;
    case EOptions::Quit:
        Exit(aCasino);
        break;
    }
}
