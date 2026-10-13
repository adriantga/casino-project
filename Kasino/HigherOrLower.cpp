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
    
    int aInput;
    DrawBreakerLine();
    WriteLine("Welcome to Higher or Lower!");
    DrawBreakerLine();
    
    WriteLine("What would you like to do?\n[1] Play Game\n[2] Show Rules\n[3] Leave Table");
    DrawBreakerLine();
    BroadcastPlayerBalance(false, aCasino);
    DrawBreakerLine();
    
    ForceInput(aInput);
    ClearConsole();

    constexpr int aPlayGame = 1;
    constexpr int aShowInstructions = 2;
    constexpr int aLeaveTable = 3;
    
    aInput = Max(aInput, aLeaveTable);

    switch (aInput)
    {
    case aPlayGame:
        PlayTable(aCasino);
        break;
    case aShowInstructions:
        ShowRules(aCasino);
        break;
    case aLeaveTable:
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

    char aPlayerHighLowInput;
    std::cin >> aPlayerHighLowInput;

    mySettings.hasPlayerPickedFirst = IsCharacter(aPlayerHighLowInput, mySettings.firstChoice);
    mySettings.hasPlayerPickedSecond = IsCharacter(aPlayerHighLowInput, mySettings.secondChoice);

    myBettingSettings.maxAllowedBet = aCasino.player.money;
        
    while (!mySettings.hasPlayerPickedFirst && !mySettings.hasPlayerPickedSecond)
    {
        std::cout << mySettings.isPlayingHighStakes << '\n';
        WriteLine("Please write 'h' for high stakes or 'l' for low stakes");

        while (std::cin.fail())
        {
            ClearInput();
            std::cin >> aPlayerHighLowInput;
        }

        std::cin >> aPlayerHighLowInput;

        mySettings.hasPlayerPickedFirst = IsCharacter(aPlayerHighLowInput, mySettings.firstChoice);
        mySettings.hasPlayerPickedSecond = IsCharacter(aPlayerHighLowInput, mySettings.secondChoice);
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
    int aPickedNumber = GetRoll();
    int aNextPickedNumber = GetRoll();

    while (aPickedNumber == aNextPickedNumber)
    {
        aPickedNumber = GetRoll();
    }
    
    bool aIsHigher = aPickedNumber > aNextPickedNumber;
    
    char aHigher = 'H';
    char aLower = 'L';
    char aQuit = 'Q';
    
    std::cout << "Do you think " << aPickedNumber << " is higher or lower than the next one?\n";
    
    char aPlayerPick;
    std::cin >> aPlayerPick;
    
    bool aPlayerPickedHigher = IsCharacter(aPlayerPick, aHigher);
    bool aPlayerPickedLower = IsCharacter(aPlayerPick, aLower);
    bool aPlayerIsQuitting = IsCharacter(aPlayerPick, aQuit);
    
    mySettings.cachedReward = 0;
    int aWinAmount = 0;

    while (!aPlayerIsQuitting)
    {
        while (std::cin.fail())
        {
            ClearInput();
            std::cin >> aPlayerPick;
        }
        
        if (aPlayerPickedHigher || aPlayerPickedLower)
        {
            bool aIsCorrectPick = aPlayerPickedHigher && aIsHigher || aPlayerPickedLower && !aIsHigher;
            if (aIsCorrectPick)
            {
                std::cout << "That's correct! " << aPickedNumber << " is " << (aIsHigher ? "higher" : "lower") << " than " << aNextPickedNumber << "!" << "\n";
                mySettings.cachedReward += mySettings.baseReward;
                
                int aTemp = aNextPickedNumber;
                aPickedNumber = aTemp;
                aNextPickedNumber = GetRoll();

                while (aPickedNumber == aNextPickedNumber)
                {
                    aPickedNumber = GetRoll();
                }
                
                aIsHigher = aPickedNumber > aNextPickedNumber;
                std::cout << "Do you think " << aPickedNumber << " is higher or lower than the next number?" << '\n';
            }
            else
            {
                std::cout << "The number after " << aPickedNumber << " was " << aNextPickedNumber << "!\n";
                WriteLine("Better luck next time!");
                mySettings.cachedReward = 0;
                break;
            }
        }
        
        std::cin >> aPlayerPick;
        aPlayerPickedHigher = IsCharacter(aPlayerPick, aHigher);
        aPlayerPickedLower = IsCharacter(aPlayerPick, aLower);
        aPlayerIsQuitting = IsCharacter(aPlayerPick, aQuit);
    }
    
    if (mySettings.cachedReward > 0)
    {
        aWinAmount = mySettings.baseReward + myBet * (mySettings.cachedReward);
    }
    
    bool aIsWinner = aWinAmount > 0 && aPlayerIsQuitting;
    BroadcastWinOrLoss(aCasino, aIsWinner, aWinAmount, myBet,
                       aIsWinner ? myBase.winAmount : myBase.lossAmount);
}
