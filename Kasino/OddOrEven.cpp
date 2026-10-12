#include "OddOrEven.h"

#include <iostream>

#include "Utilities.h"

void OddOrEven::Initialize()
{
	mySettings.rewardMultiplier = 4;
	mySettings.winLimit = 1500;
}

void OddOrEven::Reset()
{
	myBase.winAmount = 0;
	myBase.winAmount = 0;
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
    
    int input;
    DrawBreakerLine();
    WriteLine("Welcome to Even or Odd!");
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

void OddOrEven::PlayTable(Casino& aCasino)
{
	if (HasExceeded(myBase.winAmount, mySettings.winLimit))
    {
        RefuseGame(myBase.cantPlay, aCasino);
        return;
    }
	
	bool isPlayingHighStakes = false;
	myBet = GetBetAmount(aCasino, Minigames::MINIGAME_ODD_OR_EVEN, isPlayingHighStakes);
	
	TauntOrImpress(aCasino, myBase.winAmount, myBase.lossAmount, mySettings.winImpressAmount,
                   mySettings.lossTauntAmount);
	
	OnPlay(aCasino);
}

void OddOrEven::OnPlay(Casino& aCasino)
{
	char even = 'E';
	char odd = 'O';
	
	RollDice(aCasino);
	
	TauntOrImpress(aCasino, myBase.winAmount, myBase.lossAmount, mySettings.winImpressAmount,
                       mySettings.lossTauntAmount);
	
	WriteLine("Take your pick! Type 'e' for even and 'o' for odd!");
	
	char playerPick;
	std::cin >> playerPick;
	
	bool playerPickedEven = IsCharacter(playerPick, even);
	bool playerPickedOdd = IsCharacter(playerPick, odd);

	const bool isEven = IsEven(aCasino.dice.die1) && IsEven(aCasino.dice.die2);
	const bool isOdd = !IsEven(aCasino.dice.die1) && !IsEven(aCasino.dice.die2);

	while(!playerPickedEven && !playerPickedOdd)
	{
		while(std::cin.fail())
		{
			ClearInput();
			std::cin >> playerPick;
		}
		
		std::cin >> playerPick;
		
		playerPickedEven = IsCharacter(playerPick, even);
		playerPickedOdd = IsCharacter(playerPick, odd);
	}
	
	bool isWinner = (playerPickedEven && isEven) || (playerPickedOdd && isOdd);
	BroadcastDiceResult(aCasino, false);
	
	int winAmount = myBet * mySettings.rewardMultiplier;
	BroadcastWinOrLoss(aCasino, isWinner, winAmount, myBet, isWinner ? myBase.winAmount : myBase.lossAmount);
}