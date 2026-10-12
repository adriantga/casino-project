#include "GuessTheDiceSum.h"

#include <iostream>

#include "Helpers.h"
#include "Utilities.h"

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
    
    int input;
    DrawBreakerLine();
    WriteLine("Welcome to Guess The Dice Sum!");
    DrawBreakerLine();
    
    WriteLine("What would you like to do?\n[1] Play Game\n[2] Show Rules\n[3] Leave Table");
    DrawBreakerLine();
    BroadcastPlayerBalance(false, aCasino);
    DrawBreakerLine();
    
    ForceInput(input);
    ClearConsole();

    constexpr int PLAY_GAME = 1;
    constexpr int SHOW_INSTRUCTIONS = 2;
    constexpr int LEAVE_TABLE = 3;
    
    input = Max(input, LEAVE_TABLE);

    switch (input)
    {
    case PLAY_GAME:
        PlayTable(aCasino);
        break;
    case SHOW_INSTRUCTIONS:
        ShowRules(aCasino);
        break;
    case LEAVE_TABLE:
        EnterGamePicker(aCasino);
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

    char playerHighLowInput;
    std::cin >> playerHighLowInput;

    mySettings.hasPlayerPickedFirst = IsCharacter(playerHighLowInput, mySettings.firstChoice);
    mySettings.hasPlayerPickedSecond = IsCharacter(playerHighLowInput, mySettings.secondChoice);

    myBettingSettings.maxAllowedBet = aCasino.player.money;
        
    while (!mySettings.hasPlayerPickedFirst && !mySettings.hasPlayerPickedSecond)
    {
        std::cout << mySettings.isPlayingHighStakes << '\n';
        WriteLine("Please write 'h' for high stakes or 'l' for low stakes");

        while (std::cin.fail())
        {
            ClearInput();
            std::cin >> playerHighLowInput;
        }

        std::cin >> playerHighLowInput;

        mySettings.hasPlayerPickedFirst = IsCharacter(playerHighLowInput, mySettings.firstChoice);
        mySettings.hasPlayerPickedSecond = IsCharacter(playerHighLowInput, mySettings.secondChoice);
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
    int playerGuess;

    ForceInput(playerGuess);

    while (playerGuess < aCasino.dice.diceSumMin || playerGuess > aCasino.dice.diceSumMax)
    {
        std::cout << "Please enter a valid guess!" << '\n';
        std::cin >> playerGuess;
    }

    BroadcastDiceResult(aCasino, true);
    bool isWinner = playerGuess == aCasino.dice.diceSum;
    int winAmount = 0;
    
    if (playerGuess == aCasino.dice.diceSum)
    {
        winAmount = myBet * mySettings.rewardMultiplier;
    }
    
    BroadcastWinOrLoss(aCasino, isWinner, winAmount, myBet,
                       isWinner ? myBase.winAmount : myBase.lossAmount);
}