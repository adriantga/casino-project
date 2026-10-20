#include "HigherOrLower.h"

#include <iostream>

#include "Utilities.h"
#include "MenuController.h"

HigherOrLower::HigherOrLower(const Player& aPlayer, const std::array<int, 3>& aAllowedBets)
{
    this->myBettingSettings.allowedBets = aAllowedBets;
    UpdateBets(aPlayer);
}

void HigherOrLower::Initialize()
{
    mySettings.winLimit = 3500;
    mySettings.rewardMultiplier = 2;
    mySettings.baseReward = 50;
}

void HigherOrLower::Reset()
{
    myBase.winAmount = 0;
    myBase.lossAmount = 0;
    myBase.cantPlay = false;
}

void HigherOrLower::UpdateBets(const Player& aPlayer)
{
    if (myBettingSettings.allowedBets[Minigames::LOW_NO_STAKES_MIN] == 0 &&
        myBettingSettings.allowedBets[Minigames::LOW_NO_STAKES_MAX] == 0)
    {
        myBettingSettings.allowedBets = aPlayer.stakeBets;
    }

    if (mySettings.isPlayingHighStakes)
    {
        myBettingSettings.minAllowedBet = myBettingSettings.allowedBets[Minigames::HIGH_STAKES_MIN];
        myBettingSettings.maxAllowedBet = aPlayer.money;
    }
    else
    {
        myBettingSettings.minAllowedBet = myBettingSettings.allowedBets[Minigames::LOW_NO_STAKES_MIN];
        myBettingSettings.maxAllowedBet = myBettingSettings.allowedBets[Minigames::LOW_NO_STAKES_MAX];
    }
}

void HigherOrLower::ShowRules(Casino& aCasino)
{
    DrawMenuLine();
    WriteLine("HIGHER OR LOWER RULES");
    DrawMenuLine();
    WriteLine("The goal is simple: Guess whether or not the number visible\non the screen is lower than the next one. ", false);
    WriteLine("Guess correctly\nand you'll get rewarded. However, one wrong guess will\nturn your fortune into demise!\n");
    WriteLine("(By quitting without any guesses, you'll forfeit\nyour profit)");
    
    Pause();
    ClearConsole();
    EnterGameMenu(aCasino);
}

void HigherOrLower::EnterGameMenu(Casino& aCasino)
{
    ClearConsole();
    
    int input;
    DrawBreakerLine();
    WriteLine("Welcome to Higher or Lower!");
    DrawBreakerLine();
    
    WriteLine("What would you like to do?\n[1] Play Game\n[2] Show Rules\n[3] Leave Table");
    DrawBreakerLine();
    BroadcastPlayerBalance(false, aCasino);
    DrawBreakerLine();
    
    ForceInput(input);
    ClearConsole();

    constexpr int PLAY_TABLE = 1;
    constexpr int SHOW_INSTRUCTIONS = 2;
    constexpr int LEAVE_TABLE = 3;
    
    input = Max(input, LEAVE_TABLE);

    switch (input)
    {
    case PLAY_TABLE:
        PlayTable(aCasino);
        break;
    case SHOW_INSTRUCTIONS:
        ShowRules(aCasino);
        break;
    case LEAVE_TABLE:
        Menu::EnterGamePicker(aCasino);
        break;
    }
}

void HigherOrLower::PlayTable(Casino& aCasino)
{
    if (HasExceeded(myBase.winAmount, mySettings.winLimit))
    {
        RefuseGame(myBase.cantPlay, aCasino);
        return;
    }
    
    UpdateBets(aCasino.player);
    
    WriteLine("Alright, buddy. Wanna play high stakes or low stakes?('h' for high stakes, 'l' for low stakes)");

    mySettings.firstChoice = 'H';
    mySettings.secondChoice = 'L';

    char playerPick;
    std::cin >> playerPick;

    mySettings.hasPlayerPickedFirst = IsCharacter(playerPick, mySettings.firstChoice);
    mySettings.hasPlayerPickedSecond = IsCharacter(playerPick, mySettings.secondChoice);

    myBettingSettings.maxAllowedBet = aCasino.player.money;
        
    while (!mySettings.hasPlayerPickedFirst && !mySettings.hasPlayerPickedSecond)
    {
        std::cout << mySettings.isPlayingHighStakes << '\n';
        WriteLine("Please write 'h' for high stakes or 'l' for low stakes");

        while (std::cin.fail())
        {
            ClearInput();
            std::cin >> playerPick;
        }

        std::cin >> playerPick;

        mySettings.hasPlayerPickedFirst = IsCharacter(playerPick, mySettings.firstChoice);
        mySettings.hasPlayerPickedSecond = IsCharacter(playerPick, mySettings.secondChoice);
        mySettings.isPlayingHighStakes = mySettings.hasPlayerPickedFirst;
    }

    ClearInput();

    if (mySettings.hasPlayerPickedFirst)
    {
        mySettings.isPlayingHighStakes = true;
        myBettingSettings.minAllowedBet = myBettingSettings.allowedBets[Minigames::HIGH_STAKES_MIN];
        myBettingSettings.maxAllowedBet = aCasino.player.money;
    }
    else
    {
        mySettings.isPlayingHighStakes = false;

        myBettingSettings.minAllowedBet = myBettingSettings.allowedBets[Minigames::LOW_NO_STAKES_MIN];
        myBettingSettings.maxAllowedBet = myBettingSettings.allowedBets[Minigames::LOW_NO_STAKES_MAX];
    }
    
    UpdateBets(aCasino.player);
    myBet = GetBetAmount(aCasino, Minigames::MINIGAME_HIGHER_OR_LOWER, mySettings.isPlayingHighStakes);

    TauntOrImpress(aCasino, myBase.winAmount, myBase.lossAmount, mySettings.winImpressAmount,
                   mySettings.lossTauntAmount);
    OnPlay(aCasino);
}

void HigherOrLower::OnPlay(Casino& aCasino)
{
    int pickedNumber = GetRoll();
    int nextPickedNumber = GetRoll();

    while (pickedNumber == nextPickedNumber)
    {
        pickedNumber = GetRoll();
    }
    
    bool isHigher = pickedNumber > nextPickedNumber;
    
    char higher = 'H';
    char lower = 'L';
    char quit = 'Q';
    
    std::cout << "Do you think " << pickedNumber << " is higher or lower than the next one?\n";
    
    char playerGuess;
    std::cin >> playerGuess;
    
    bool hasPickedHigher = IsCharacter(playerGuess, higher);
    bool hasPickedLower = IsCharacter(playerGuess, lower);
    bool hasQuit = IsCharacter(playerGuess, quit);
    
    mySettings.cachedReward = 0;
    int winAmount = 0;

    while (!hasQuit)
    {
        while (std::cin.fail())
        {
            ClearInput();
            std::cin >> playerGuess;
        }
        
        if (hasPickedHigher || hasPickedLower)
        {
            bool isCorrectPick = hasPickedHigher && isHigher || hasPickedLower && !isHigher;
            if (isCorrectPick)
            {
                std::cout << "That's correct! " << pickedNumber << " is " << (isHigher ? "higher" : "lower") << " than " << nextPickedNumber << "!" << "\n";
                mySettings.cachedReward += mySettings.baseReward;
                
                int aTemp = nextPickedNumber;
                pickedNumber = aTemp;
                nextPickedNumber = GetRoll();

                while (pickedNumber == nextPickedNumber)
                {
                    pickedNumber = GetRoll();
                }
                
                isHigher = pickedNumber > nextPickedNumber;
                std::cout << "Do you think " << pickedNumber << " is higher or lower than the next number?" << '\n';
            }
            else
            {
                std::cout << "The number after " << pickedNumber << " was " << nextPickedNumber << "!\n";
                WriteLine("Better luck next time!");
                mySettings.cachedReward = 0;
                break;
            }
        }
        
        std::cin >> playerGuess;
        hasPickedHigher = IsCharacter(playerGuess, higher);
        hasPickedLower = IsCharacter(playerGuess, lower);
        hasQuit = IsCharacter(playerGuess, quit);
    }
    
    if (mySettings.cachedReward > 0)
    {
        winAmount = mySettings.baseReward + myBet * (mySettings.cachedReward);
    }
    
    bool isWinner = winAmount > 0 && hasQuit;
    BroadcastWinOrLoss(aCasino, isWinner, winAmount, myBet,
                       isWinner ? myBase.winAmount : myBase.lossAmount);
}