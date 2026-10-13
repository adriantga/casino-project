#include "GuessTheDiceSum.h"

#include <iostream>

#include "Helpers.h"
#include "Utilities.h"
#include "MenuController.h"

GuessTheDiceSum::GuessTheDiceSum(const Player& aPlayer, const std::array<int, 3>& aAllowedBets)
{
    this->myBettingSettings.allowedBets = aAllowedBets;
    UpdateBets(aPlayer);
}

void GuessTheDiceSum::Initialize()
{
    mySettings.rewardMultiplier = 5;
    mySettings.winLimit = 2500;
}

void GuessTheDiceSum::UpdateBets(const Player& aPlayer)
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

void GuessTheDiceSum::Reset()
{
    myBase.winAmount = 0;
    myBase.lossAmount = 0;
    myBase.cantPlay = false;
}

void GuessTheDiceSum::EnterGameMenu(Casino& aCasino)
{
    ClearConsole();
    
    int aInput;
    DrawBreakerLine();
    WriteLine("Welcome to Guess The Dice Sum!");
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

void GuessTheDiceSum::ShowRules(Casino& aCasino)
{
    DrawMenuLine();
    WriteLine("GUESS THE DICE SUM RULES");
    DrawMenuLine();
    WriteLine("I'll roll two dice and your only goal from that point\nonwards is to guess the sum produced the aforementioned\nrolled dice. It's important to note: Your guess should\n**NOT** exceed 12 or subceed 2!");
    Pause();
    ClearConsole();
    EnterGameMenu(aCasino);
}

void GuessTheDiceSum::PlayTable(Casino& aCasino)
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
    myBet = GetBetAmount(aCasino, Minigames::MINIGAME_GUESS_THE_DICE_SUM, mySettings.isPlayingHighStakes);

    TauntOrImpress(aCasino, myBase.winAmount, myBase.lossAmount, mySettings.winImpressAmount,
                   mySettings.lossTauntAmount);
    
    RollDice(aCasino);
    
    WriteLine("Guess a number (2-12)");
    OnPlay(aCasino);
}

void GuessTheDiceSum::OnPlay(Casino& aCasino)
{
    int aPlayerGuess;

    ForceInput(aPlayerGuess);

    while (aPlayerGuess < aCasino.dice.diceSumMin || aPlayerGuess > aCasino.dice.diceSumMax)
    {
        std::cout << "Please enter a valid guess!" << '\n';
        std::cin >> aPlayerGuess;
    }

    BroadcastDiceResult(aCasino, true);
    bool aIsWinner = aPlayerGuess == aCasino.dice.diceSum;
    int aWinAmount = 0;
    
    if (aPlayerGuess == aCasino.dice.diceSum)
    {
        aWinAmount = myBet * mySettings.rewardMultiplier;
    }
    
    BroadcastWinOrLoss(aCasino, aIsWinner, aWinAmount, myBet,
                       aIsWinner ? myBase.winAmount : myBase.lossAmount);
}