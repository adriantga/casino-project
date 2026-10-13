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
    
    int aInput;
    DrawBreakerLine();
    WriteLine("Welcome to Yes or No!");
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

void YesOrNo::PlayTable(Casino& aCasino)
{
    if (HasExceeded(myBase.winAmount, mySettings.winLimit))
    {
        RefuseGame(myBase.cantPlay, aCasino);
        return;
    }
	
	bool aIsPlayingHighStakes = false;
	myBet = GetBetAmount(aCasino, Minigames::MINIGAME_YES_OR_NO, aIsPlayingHighStakes);
	
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
        const int aTemp = myRangeEnd;
        myRangeStart = myRangeEnd;
        myRangeEnd = aTemp;
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
    
    char aYes = 'Y';
    char aNo = 'N';
    char aQuit = 'Q';
    
    int aActualRoll = GetRoll();
    
    int aNumConsecutiveCorrectGuesses = 0;
    
    std::cout << "Is the number between " << myRangeStart << " and " << myRangeEnd << "?" << '\n';
    
    char aPlayerPick;
    std::cin >> aPlayerPick;
    
    bool aPlayerPickedYes = IsCharacter(aPlayerPick, aYes);
    bool aPlayerPickedNo = IsCharacter(aPlayerPick, aNo);
    bool aPlayerIsQuitting = IsCharacter(aPlayerPick, aQuit);

    while (!aPlayerIsQuitting)
    {
        if (!aPlayerPickedYes && !aPlayerPickedNo)
        {
            WriteLine("Please write 'y' for yes or 'n' for no");
        }
        
        while (std::cin.fail())
        {
            ClearInput();
            std::cin >> aPlayerPick;
        }
        
        if (aPlayerPickedYes || aPlayerPickedNo)
        {
            const bool aIsCorrectPick = (aPlayerPickedYes && IsInRange(aActualRoll, myRangeStart, myRangeEnd)) || (aPlayerPickedNo && !IsInRange(aActualRoll, myRangeStart, myRangeEnd));
            
            if (aIsCorrectPick)
            {
                aNumConsecutiveCorrectGuesses++;
                if (aNumConsecutiveCorrectGuesses >= mySettings.rewardIncreaseThreshold)
                {
                    aNumConsecutiveCorrectGuesses = 0;
                    mySettings.rewardMultiplier++;
                    std::cout << "Your reward multiplier has increased. Current multiplier: " << mySettings.rewardMultiplier << '\n';
                }
             
                aActualRoll = GetRoll();
                ForceRange();
                
                std::cout << "Is the number between " << myRangeStart << " and " << myRangeEnd << "?\n";
                mySettings.cachedReward += mySettings.baseReward;
            }
            else
            {
                mySettings.cachedReward = 0;
                std::cout << "You guessed wrong! The number was " << aActualRoll << " which means the number was " << (IsInRange(aActualRoll, myRangeStart, myRangeEnd) ? "in" : "not in") << " range.\n";
                break;
            }
        }
        
        std::cin >> aPlayerPick;
        aPlayerPickedYes = IsCharacter(aPlayerPick, aYes);
        aPlayerPickedNo = IsCharacter(aPlayerPick, aNo);
        aPlayerIsQuitting = IsCharacter(aPlayerPick, aQuit);
    }
    
    int aWinAmount = 0;
    
    if (mySettings.cachedReward > 0)
    {
        aWinAmount = myBet + (mySettings.cachedReward * mySettings.rewardMultiplier);
    }
    
    bool aIsWinner = aWinAmount > 0 && aPlayerIsQuitting;
    BroadcastWinOrLoss(aCasino, aIsWinner, aWinAmount, myBet, aIsWinner ? myBase.winAmount : myBase.lossAmount);
}
