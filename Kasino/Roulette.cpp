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
    
    int input;
    DrawBreakerLine();
    WriteLine("Welcome to Roulette!");
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
	char correctPick = ' ';
	int correctRow;
	int landedCol;
	
	int landing = GetRandomNumber(aCasino.game.rouletteStraightMin, aCasino.game.rouletteStraightMax);
	std::array<int, 12> correctRowArray;
	bool isWinner;
	
	int playerDecision;
	std::cin >> playerDecision;

	while (std::cin.fail())
	{
		WriteLine("Please enter a valid number!");
		ClearInput();
		std::cin >> playerDecision;
	}

	while (HasSubceeded(playerDecision, aCasino.game.rouletteBettingTypeMin) || HasExceeded(playerDecision, aCasino.game.rouletteBettingTypeMax))
	{
		std::cout << "Please enter a valid number in range(" << aCasino.game.rouletteBettingTypeMin << " - " << aCasino.game.rouletteBettingTypeMax << ")\n";
		std::cin >> playerDecision;
	}
	
	if (playerDecision == aCasino.game.rouletteStraight)
	{
		WriteLine("What's your guess? (0-36)");

		int playerInput;
        std::cin >> playerInput;

        while (std::cin.fail())
        {
            WriteLine("Please make a valid pick!");

            ClearInput();
            std::cin >> playerInput;
        }

        playerInput = Clamp(playerInput, aCasino.game.rouletteStraightMin,
                             aCasino.game.rouletteStraightMax);

        isWinner = playerInput == landing;
	}
	else if (playerDecision == aCasino.game.rouletteRedBlack)
	{
		WriteLine("What's your guess? ('r' for red, 'b' for black)");
		
		char red = 'R';
		char black = 'B';
		
		int cachedRoll = GetRoll();

		bool isRedPicked = HasSubceeded(cachedRoll, 50) && cachedRoll > 0;

		correctPick = isRedPicked ? red : black;

		char playerPick;
		std::cin >> playerPick;
		
		bool hasPickedRed = IsCharacter(playerPick, red);
		bool hasPickedBlack = IsCharacter(playerPick, black);
		
		bool isCorrectPick = IsCharacter(playerPick, correctPick);

		while (!hasPickedRed && !hasPickedBlack)
		{
			WriteLine("Please make a valid pick!");

            while (std::cin.fail())
            {
                ClearInput();
                std::cin >> playerPick;
            }

            std::cin >> playerPick;

            hasPickedRed = IsCharacter(playerPick, red);
            hasPickedBlack = IsCharacter(playerPick, black);
		}
		
		isWinner = isCorrectPick;
	}
	else if (playerDecision == aCasino.game.rouletteOddEven)
	{
		char odd = 'O';
		char even = 'E';
		
		bool isLandingEven = IsEven(landing);
		
		WriteLine("What's your guess? ('e' for even, 'o' for odd)");
		
		char playerPick;
		std::cin >> playerPick;
		
		bool hasPickedOdd = IsCharacter(playerPick, odd);
		bool hasPickedEven = IsCharacter(playerPick, even);

		while (!hasPickedOdd && !hasPickedEven)
		{
			WriteLine("Please make a valid pick!");
			
			while (std::cin.fail())
            {
                ClearInput();
                std::cin >> playerPick;
            }

            std::cin >> playerPick;
			
			hasPickedOdd = IsCharacter(playerPick, odd);
			hasPickedEven = IsCharacter(playerPick, even);
		}
		
		isWinner = (hasPickedOdd && !isLandingEven) || (hasPickedEven && isLandingEven);
	}
	else
	{
		int correctRowIndex = GetRandomNumber(0, 3);
		correctRowArray = BuildRow(correctRowIndex);
		correctRow = correctRowIndex;
		
		landedCol = GetRandomNumber(aCasino.game.rouletteRowMin, aCasino.game.rouletteRowMax);
		
		WriteLine("Which row are you picking? (0-3)");

        int pickedRow;
        std::cin >> pickedRow;

		while (std::cin.fail())
		{
			WriteLine("Please make a valid pick!");
			ClearInput();
			std::cin >> pickedRow;
		}

		while (HasSubceeded(pickedRow, 0) || HasExceeded(pickedRow, aCasino.game.rouletteRowMax))
		{
			std::cout << "Please pick a valid row! (0 - " << aCasino.game.rouletteRowMax << ")\n";
			std::cin >> pickedRow;
		}
		
		isWinner = pickedRow == correctRowIndex;
		
		bool hasNumbers = playerDecision == aCasino.game.rouletteStraight || playerDecision == aCasino.game.rouletteOddEven;
		bool hasRow = playerDecision == aCasino.game.rouletteColumnRow;

		if (hasRow)
		{
			bool isAboveZero = correctRow > 0;

			std::cout << (isWinner ? "Congratulations! It landed on" : "Nice try! It landed on") << " " <<
				correctRow;
			if (isAboveZero) std::cout << "(" << (isAboveZero ? correctRowArray[landedCol] : 0) << ")" << '\n';
			else
			{
				 std::cout << '\n';
			}
		}
		else if (hasNumbers)
		{
			std::cout << (isWinner ? "Well done! The ball landed on" : "Nice try! The ball landed on") << " " <<
				landing <<
				'\n';
		}
		else
		{
			std::cout << "It landed on a " << correctPick << " rouge... so" << (isWinner
				                                                                    ? " congratulations!"
				                                                                    : " nice try! Better luck next time") << '\n';
		}
	}
	
	int winAmount = landing == 0 ? myBet * mySettings.alternativeRewardMultiplier : myBet * mySettings.rewardMultiplier;
	BroadcastWinOrLoss(aCasino, isWinner, winAmount, myBet, isWinner ? myBase.winAmount : myBase.lossAmount);
}