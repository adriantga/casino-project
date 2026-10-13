#include "OddOrEven.h"

#include <iostream>

#include "Utilities.h"
#include "MenuController.h"

void OddOrEven::Initialize()
{
	mySettings.rewardMultiplier = 4;
	mySettings.winLimit = 1500;
}

void OddOrEven::Reset()
{
	myBase.winAmount = 0;
	myBase.lossAmount = 0;
	myBase.cantPlay = false;
}

void OddOrEven::ShowRules(Casino& aCasino)
{
	WriteLine(
            "Here you must guess if the dice are even or odd. If both\ndice don't show even or odd numbers, or if you guess\nincorrectly(i.e. guessing odd but both dice show even),\nthe house wins.");
	
	Pause();
    ClearConsole();
    EnterGameMenu(aCasino);
}

void OddOrEven::EnterGameMenu(Casino& aCasino)
{
    ClearConsole();
    
    int aInput;
    DrawBreakerLine();
    WriteLine("Welcome to Even or Odd!");
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

void OddOrEven::PlayTable(Casino& aCasino)
{
	if (HasExceeded(myBase.winAmount, mySettings.winLimit))
    {
        RefuseGame(myBase.cantPlay, aCasino);
        return;
    }
	
	bool aIsPlayingHighStakes = false;
	myBet = GetBetAmount(aCasino, Minigames::MINIGAME_ODD_OR_EVEN, aIsPlayingHighStakes);
	
	TauntOrImpress(aCasino, myBase.winAmount, myBase.lossAmount, mySettings.winImpressAmount,
                   mySettings.lossTauntAmount);
	
	OnPlay(aCasino);
}

void OddOrEven::OnPlay(Casino& aCasino)
{
	char aEven = 'E';
	char aOdd = 'O';
	
	RollDice(aCasino);
	
	TauntOrImpress(aCasino, myBase.winAmount, myBase.lossAmount, mySettings.winImpressAmount,
                       mySettings.lossTauntAmount);
	
	WriteLine("Take your pick! Type 'e' for even and 'o' for odd!");
	
	char aPlayerPick;
	std::cin >> aPlayerPick;
	
	bool aPlayerPickedEven = IsCharacter(aPlayerPick, aEven);
	bool aPlayerPickedOdd = IsCharacter(aPlayerPick, aOdd);

	const bool aIsEven = IsEven(aCasino.dice.die1) && IsEven(aCasino.dice.die2);
	const bool aIsOdd = !IsEven(aCasino.dice.die1) && !IsEven(aCasino.dice.die2);

	while (!aPlayerPickedEven && !aPlayerPickedOdd)
	{
		while (std::cin.fail())
		{
			ClearInput();
			std::cin >> aPlayerPick;
		}
		
		std::cin >> aPlayerPick;
		
		aPlayerPickedEven = IsCharacter(aPlayerPick, aEven);
		aPlayerPickedOdd = IsCharacter(aPlayerPick, aOdd);
	}
	
	bool aIsWinner = (aPlayerPickedEven && aIsEven) || (aPlayerPickedOdd && aIsOdd);
	BroadcastDiceResult(aCasino, false);
	
	int aWinAmount = myBet * mySettings.rewardMultiplier;
	BroadcastWinOrLoss(aCasino, aIsWinner, aWinAmount, myBet, aIsWinner ? myBase.winAmount : myBase.lossAmount);
}