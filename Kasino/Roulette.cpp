#include "Roulette.h"

#include <iostream>

#include "Utilities.h"
#include "MenuController.h"

void Roulette::Initialize()
{
	mySettings.rewardMultiplier = 3;
	mySettings.alternativeRewardMultiplier = 36;
}

void Roulette::ShowRules(Casino& aCasino)
{
	DrawMenuLine();
    WriteLine("ROULETTE RULES");
    DrawMenuLine();
	
	WriteLine("Your main objective is to guess what color the ball will\nland on. ", false);
	WriteLine("First, you'll have to pick how you want to bet:");

	WriteLine("** STRAIGHT **: Requires you to bet on a rouge. You're\nonly able to guess 0-36");
	WriteLine("** RED/BLACK **: Picking this will require you to pick\neither red or black");
	WriteLine("** ODD/EVEN **: You'll need to guess either odd or even.");
	WriteLine("** COLUMN BET **: Allows you to bet from 0-3. In this case,\nzero is standalone.\n");

	WriteLine("If you guess correctly, your pay out stays the same.\n(If you bet on 0 and it lands, you'll win 36x your bet!");
	
	Pause();
    ClearConsole();
    EnterGameMenu(aCasino);
}

void Roulette::Reset()
{
	myBase.winAmount = 0;
	myBase.lossAmount = 0;
	myBase.cantPlay = false;
}

void Roulette::EnterGameMenu(Casino& aCasino)
{
	ClearConsole();
    
    int aInput;
    DrawBreakerLine();
    WriteLine("Welcome to Roulette!");
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

void Roulette::PlayTable(Casino& aCasino)
{
	 if (HasExceeded(myBase.winAmount, mySettings.winLimit))
    {
        RefuseGame(myBase.cantPlay, aCasino);
        return;
    }
	
	bool aIsPlayingHighStakes = false;
	myBet = GetBetAmount(aCasino, Minigames::MINIGAME_ROULETTE, aIsPlayingHighStakes);
	TauntOrImpress(aCasino, myBase.winAmount, myBase.lossAmount, mySettings.winImpressAmount,
                   mySettings.lossTauntAmount);
	
	WriteLine("What do you feel like playing?\n1. Straight\n2. Red/Black\n3. Odd/even\n4. Column Bet");
	OnPlay(aCasino);
}

void Roulette::OnPlay(Casino& aCasino)
{
	char aCorrectPick = ' ';
	int aCorrectRow = 0;
	int aLandedCol = -1;
	
	int aLanding = GetRandomNumber(aCasino.game.rouletteStraightMin, aCasino.game.rouletteStraightMax);
	std::array<int, 12> aCorrectRowArray = {};
	bool aIsWinner = false;
	
	int aPlayerDecision;
	std::cin >> aPlayerDecision;

	while (std::cin.fail())
	{
		WriteLine("Please enter a valid number!");
		ClearInput();
		std::cin >> aPlayerDecision;
	}

	while (HasSubceeded(aPlayerDecision, aCasino.game.rouletteBettingTypeMin) || HasExceeded(aPlayerDecision, aCasino.game.rouletteBettingTypeMax))
	{
		std::cout << "Please enter a valid number in range(" << aCasino.game.rouletteBettingTypeMin << " - " << aCasino.game.rouletteBettingTypeMax << ")\n";
		std::cin >> aPlayerDecision;
	}
	
	if (aPlayerDecision == aCasino.game.rouletteStraight)
	{
		WriteLine("What's your guess? (0-36)");

		int aPlayerInput;
        std::cin >> aPlayerInput;

        while (std::cin.fail())
        {
            WriteLine("Please make a valid pick!");

            ClearInput();
            std::cin >> aPlayerInput;
        }

        aPlayerInput = Clamp(aPlayerInput, aCasino.game.rouletteStraightMin,
                             aCasino.game.rouletteStraightMax);

        aIsWinner = aPlayerInput == aLanding;
	}
	else if (aPlayerDecision == aCasino.game.rouletteRedBlack)
	{
		WriteLine("What's your guess? ('r' for red, 'b' for black)");
		
		char aRed = 'R';
		char aBlack = 'B';
		
		int aCachedRoll = GetRoll();

		bool aIsRedPicked = HasSubceeded(aCachedRoll, 50) && aCachedRoll > 0;

		aCorrectPick = aIsRedPicked ? aRed : aBlack;

		char aPlayerPick;
		std::cin >> aPlayerPick;
		
		bool aPlayerPickedRed = IsCharacter(aPlayerPick, aRed);
		bool aPlayerPickedBlack = IsCharacter(aPlayerPick, aBlack);
		
		bool aIsCorrectPick = IsCharacter(aPlayerPick, aCorrectPick);

		while (!aPlayerPickedRed && !aPlayerPickedBlack)
		{
			WriteLine("Please make a valid pick!");

            while (std::cin.fail())
            {
                ClearInput();
                std::cin >> aPlayerPick;
            }

            std::cin >> aPlayerPick;

            aPlayerPickedRed = IsCharacter(aPlayerPick, aRed);
            aPlayerPickedBlack = IsCharacter(aPlayerPick, aBlack);
		}
		
		aIsWinner = aIsCorrectPick;
	}
	else if (aPlayerDecision == aCasino.game.rouletteOddEven)
	{
		char aOdd = 'O';
		char aEven = 'E';
		
		bool aIsLandingEven = IsEven(aLanding);
		
		WriteLine("What's your guess? ('e' for even, 'o' for odd)");
		
		char aPlayerPick;
		std::cin >> aPlayerPick;
		
		bool aPlayerPickedOdd = IsCharacter(aPlayerPick, aOdd);
		bool aPlayerPickedEven = IsCharacter(aPlayerPick, aEven);

		while (!aPlayerPickedOdd && !aPlayerPickedEven)
		{
			WriteLine("Please make a valid pick!");
			
			while (std::cin.fail())
            {
                ClearInput();
                std::cin >> aPlayerPick;
            }

            std::cin >> aPlayerPick;
			
			aPlayerPickedOdd = IsCharacter(aPlayerPick, aOdd);
			aPlayerPickedEven = IsCharacter(aPlayerPick, aEven);
		}
		
		aIsWinner = (aPlayerPickedOdd && !aIsLandingEven) || (aPlayerPickedEven && aIsLandingEven);
	}
	else
	{
		int aCorrectRowIndex = GetRandomNumber(0, 3);
		aCorrectRowArray = BuildRow(aCorrectRowIndex);
		aCorrectRow = aCorrectRowIndex;
		
		aLandedCol = GetRandomNumber(aCasino.game.rouletteRowMin, aCasino.game.rouletteRowMax);
		
		WriteLine("Which row are you picking? (0-3)");

        int aPlayerRowPick;
        std::cin >> aPlayerRowPick;

		while (std::cin.fail())
		{
			WriteLine("Please make a valid pick!");
			ClearInput();
			std::cin >> aPlayerRowPick;
		}

		while (HasSubceeded(aPlayerRowPick, 0) || HasExceeded(aPlayerRowPick, aCasino.game.rouletteRowMax))
		{
			std::cout << "Please pick a valid row! (0 - " << aCasino.game.rouletteRowMax << ")\n";
			std::cin >> aPlayerRowPick;
		}
		
		aIsWinner = aPlayerRowPick == aCorrectRowIndex;
		
		bool aHasNumbers = aPlayerDecision == aCasino.game.rouletteStraight || aPlayerDecision == aCasino.game.rouletteOddEven;
		bool aHasRow = aPlayerDecision == aCasino.game.rouletteColumnRow;

		if (aHasRow)
		{
			bool aIsAboveZero = aCorrectRow > 0;

			std::cout << (aIsWinner ? "Congratulations! It landed on" : "Nice try! It landed on") << " " <<
				aCorrectRow;
			if (aIsAboveZero) std::cout << "(" << (aIsAboveZero ? aCorrectRowArray[aLandedCol] : 0) << ")" << '\n';
			else std::cout << '\n';
		}
		else if (aHasNumbers)
		{
			std::cout << (aIsWinner ? "Well done! The ball landed on" : "Nice try! The ball landed on") << " " <<
				aLanding <<
				'\n';
		}
		else
		{
			std::cout << "It landed on a " << aCorrectPick << " rouge... so" << (aIsWinner
				                                                                    ? " congratulations!"
				                                                                    : " nice try! Better luck next time") << '\n';
		}
	}
	
	int aWinAmount = aLanding == 0 ? myBet * mySettings.alternativeRewardMultiplier : myBet * mySettings.rewardMultiplier;
	BroadcastWinOrLoss(aCasino, aIsWinner, aWinAmount, myBet, aIsWinner ? myBase.winAmount : myBase.lossAmount);
}