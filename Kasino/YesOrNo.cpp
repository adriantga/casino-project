#include "YesOrNo.h"

#include <iostream>

#include "Utilities.h"
#include "MenuController.h"

void YesOrNo::Initialize()
{
	mySettings.baseReward = 25;
	mySettings.winLimit = 4000;
	mySettings.rewardIncreaseThreshold = 3;
}

void YesOrNo::Reset()
{
    myBase.winAmount = 0;
    myBase.lossAmount = 0;
    myBase.cantPlay = false;
}

void YesOrNo::EnterGameMenu(Casino& aCasino)
{
    ClearConsole();
    
    int input;
    DrawBreakerLine();
    WriteLine("Welcome to Yes or No!");
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

void YesOrNo::PlayTable(Casino& aCasino)
{
    if (HasExceeded(myBase.winAmount, mySettings.winLimit))
    {
        RefuseGame(myBase.cantPlay, aCasino);
        return;
    }
	
	bool isPlayingHighStakes = false;
	myBet = GetBetAmount(aCasino, Minigames::MINIGAME_YES_OR_NO, isPlayingHighStakes);
	
    ForceRange();
    
    TauntOrImpress(aCasino, myBase.winAmount, myBase.lossAmount, mySettings.winImpressAmount,
                   mySettings.lossTauntAmount);
    
    OnPlay(aCasino);
}

void YesOrNo::ForceRange()
{
    myRangeStart = GetRoll();
    myRangeEnd = GetRoll();
    
    if (myRangeStart > myRangeEnd)
    {
        const int temp = myRangeEnd;
        myRangeStart = myRangeEnd;
        myRangeEnd = temp;
    }

    if (myRangeStart == myRangeEnd)
    {
        myRangeEnd++;
    }
}

void YesOrNo::ShowRules(Casino& aCasino)
{
    DrawMenuLine();
    WriteLine("YES OR NO RULES");
    DrawMenuLine();
    WriteLine("Your sole objective is to guess whether the number picked\nis within the given range. ", false);
    WriteLine("Use 'y' if you think the number\nis in range and 'n' to guess if it is not. ", false);
    std::cout << "Every time you\nhave " << mySettings.rewardIncreaseThreshold << " consecutive correct guesses in a row, your reward\nmight increase!" << '\n';
    WriteLine("\nNOTE: If you quit before guessing, you'll automatically\nlose!");
    
    Pause();
    ClearConsole();
    EnterGameMenu(aCasino);
}

void YesOrNo::OnPlay(Casino& aCasino)
{
    mySettings.rewardMultiplier = 1;
    
    char yes = 'Y';
    char no = 'N';
    char quit = 'Q';
    
    int actualRoll = GetRoll();
    
    int numConsecutiveCorrectGuesses = 0;
    
    std::cout << "Is the number between " << myRangeStart << " and " << myRangeEnd << "?" << '\n';
    
    char playerPick;
    std::cin >> playerPick;
    
    bool hasPickedYes = IsCharacter(playerPick, yes);
    bool hasPickedNo = IsCharacter(playerPick, no);
    bool hasQuit = IsCharacter(playerPick, quit);

    while (!hasQuit)
    {
        if (!hasPickedYes && !hasPickedNo)
        {
            WriteLine("Please write 'y' for yes or 'n' for no");
        }
        
        while (std::cin.fail())
        {
            ClearInput();
            std::cin >> playerPick;
        }
        
        if (hasPickedYes || hasPickedNo)
        {
            const bool isCorrectPick = (hasPickedYes && IsInRange(actualRoll, myRangeStart, myRangeEnd)) || (hasPickedNo && !IsInRange(actualRoll, myRangeStart, myRangeEnd));
            
            if (isCorrectPick)
            {
                numConsecutiveCorrectGuesses++;
                if (numConsecutiveCorrectGuesses >= mySettings.rewardIncreaseThreshold)
                {
                    numConsecutiveCorrectGuesses = 0;
                    mySettings.rewardMultiplier++;
                    std::cout << "Your reward multiplier has increased. Current multiplier: " << mySettings.rewardMultiplier << '\n';
                }
             
                actualRoll = GetRoll();
                ForceRange();
                
                std::cout << "Is the number between " << myRangeStart << " and " << myRangeEnd << "?\n";
                mySettings.cachedReward += mySettings.baseReward;
            }
            else
            {
                mySettings.cachedReward = 0;
                std::cout << "You guessed wrong! The number was " << actualRoll << " which means the number was " << (IsInRange(actualRoll, myRangeStart, myRangeEnd) ? "in" : "not in") << " range.\n";
                break;
            }
        }
        
        std::cin >> playerPick;
        hasPickedYes = IsCharacter(playerPick, yes);
        hasPickedNo = IsCharacter(playerPick, no);
        hasQuit = IsCharacter(playerPick, quit);
    }
    
    int winAmount = 0;
    
    if (mySettings.cachedReward > 0)
    {
        winAmount = myBet + (mySettings.cachedReward * mySettings.rewardMultiplier);
    }
    
    bool isWinner = winAmount > 0 && hasQuit;
    BroadcastWinOrLoss(aCasino, isWinner, winAmount, myBet, isWinner ? myBase.winAmount : myBase.lossAmount);
}