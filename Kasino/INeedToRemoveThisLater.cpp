#include "INeedToRemoveThisLater.h"
#include "Utilities.h"
#include <iostream>
#include "StateController.h"

INeedToRemoveThisLater::INeedToRemoveThisLater(Player& aPlayer, std::array<int, 3>& aAllowedBets, bool aHasStakes)
{
    this->myAllowedBets = aAllowedBets;
    this->myHasStakes = aHasStakes;
    UpdateBets(aPlayer);
}

void INeedToRemoveThisLater::UpdateBets(Player& aPlayer)
{
    if (!myHasStakes)
    {
        myMinAllowedBet = myAllowedBets[LOW_NO_STAKES_MIN];
        myMaxAllowedBet = aPlayer.money;
    }
    else
    {
        if (myIsPlayingHighStakes)
        {
            myMinAllowedBet = myAllowedBets[HIGH_STAKES_MIN];
            myMaxAllowedBet = aPlayer.money;
        }
        else
        {
            myMinAllowedBet = myAllowedBets[LOW_NO_STAKES_MIN];
            myMaxAllowedBet = myAllowedBets[LOW_NO_STAKES_MAX];
        }
        
    }
}

void INeedToRemoveThisLater::Initialize(const EMinigameType& aMinigameType)
{
    myMinigameType = aMinigameType;

    switch (aMinigameType)
    {
    case EMinigameType::GuessTheDiceSum:
        myRewardMultiplier = 5;
        myWinLimit = 2500;
        myGameIndex = MINIGAME_GUESS_THE_DICE_SUM;
        myHasStakes = true;
        break;
    case EMinigameType::OddOrEven:
        
        break;
    case EMinigameType::YesOrNo:
        myBaseReward = 25;
        myUseCachedReward = true;
        myRewardIncreaseThreshold = 3;
        myWinLimit = 4000;
        myGameIndex = MINIGAME_YES_OR_NO;
        myShouldPlayerQuit = true;
        myIsForcedVictory = true;
        break;
    case EMinigameType::HigherOrLower:
        myRewardMultiplier = 2;
        myBaseReward = 50;
        myUseCachedReward = true;
        myWinLimit = 3500;
        myGameIndex = MINIGAME_HIGHER_OR_LOWER;
        myShouldPlayerQuit = true;
        myIsForcedVictory = true;
        break;
    case EMinigameType::Roulette:
        myRewardMultiplier = 3;
        myAlternativeRewardMultiplier = 36;
        myWinLimit = 3000;
        myGameIndex = MINIGAME_ROULETTE;
        myIsForcedWinAmount = false;
        break;
    }
}

void INeedToRemoveThisLater::EnterGameMenu(Casino& aCasino)
{
    ClearConsole();
    
    int input;
    DrawBreakerLine();
    std::cout << "Welcome to the " << FromMinigameToChar(myMinigameType) << " table!\n";
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
        PlayGame(aCasino);
        break;
    case SHOW_INSTRUCTIONS:
        ShowRules(aCasino);
        break;
    case LEAVE_TABLE:
        EnterGamePicker(aCasino);
        break;
    }
}

void INeedToRemoveThisLater::PlayGame(Casino& aCasino)
{
    if (HasExceeded(myWinAmount, myWinLimit))
    {
        RefuseGame(myCantPlay, aCasino);
        return;
    }

    myCachedReward = 0;
    
    myIsPlayingHighStakes = false;
    
    /*
     * Force update the min and max bets so they don't get
     * carried over from i.e. high stakes to low stakes.
     */
    UpdateBets(aCasino.player);
    

    if (myHasStakes)
    {
        WriteLine("Alright, buddy. Wanna play high stakes or low stakes?('h' for high stakes, 'l' for low stakes)");

        myFirstChoice = 'H';
        mySecondChoice = 'L';

        char playerHighLowInput;
        std::cin >> playerHighLowInput;

        myHasPlayerPickedFirst = IsCharacter(playerHighLowInput, myFirstChoice);
        myHasPlayerPickedSecond = IsCharacter(playerHighLowInput, mySecondChoice);

        myMaxAllowedBet = aCasino.player.money;
        
        while (!myHasPlayerPickedFirst && !myHasPlayerPickedSecond)
        {
            std::cout << myIsPlayingHighStakes << '\n';
            WriteLine("Please write 'h' for high stakes or 'l' for low stakes");

            while (std::cin.fail())
            {
                ClearInput();
                std::cin >> playerHighLowInput;
            }

            std::cin >> playerHighLowInput;

            myHasPlayerPickedFirst = IsCharacter(playerHighLowInput, myFirstChoice);
            myHasPlayerPickedSecond = IsCharacter(playerHighLowInput, mySecondChoice);
            myIsPlayingHighStakes = myHasPlayerPickedFirst;
        }

        ClearInput();

        if (myHasPlayerPickedFirst)
        {
            myIsPlayingHighStakes = true;
            myMinAllowedBet = myAllowedBets[HIGH_STAKES_MIN];
            myMaxAllowedBet = aCasino.player.money;
        }
        else
        {
            myIsPlayingHighStakes = false;

            myMinAllowedBet = myAllowedBets[LOW_NO_STAKES_MIN];
            myMaxAllowedBet = myAllowedBets[LOW_NO_STAKES_MAX];
        }
    }
    
    UpdateBets(aCasino.player);
    myBet = GetBetAmount(aCasino, myGameIndex, myIsPlayingHighStakes);

    TauntOrImpress(aCasino, myWinAmount, myLossAmount, myWinImpressAmount,
                   myLossTauntAmount);
    
    myRangeStart = GetRoll();
    myRangeEnd = GetRoll();

    if (myRangeStart > myRangeEnd)
    {
        int temp = myRangeEnd;
        myRangeStart = myRangeEnd;
        myRangeEnd = temp;
    }

    if (myRangeStart == myRangeEnd)
    {
        myRangeEnd++;
    }

    RollDice(aCasino);

    switch (myMinigameType)
    {
    case EMinigameType::GuessTheDiceSum:
        WriteLine("Guess a number (2-12)");
        break;
    case EMinigameType::OddOrEven:
        WriteLine("What are you guessing? Even or odd? Type 'e' for even or 'o' for odd!");
        break;
    case EMinigameType::YesOrNo:
        std::cout << "Is the number between " << myRangeStart << " and " << myRangeEnd << "?" << '\n';
        break;
    case EMinigameType::HigherOrLower:
        std::cout << "Do you think " << myRangeStart << " is higher or lower than the next number?" << '\n';
        break;
    case EMinigameType::Roulette:
        WriteLine("What do you feel like playing?\n1. Straight\n2. Red/Black\n3. Odd/even\n4. Column Bet");
        break;
    }

    OnPlay(aCasino);
}

void INeedToRemoveThisLater::Reset()
{
    myWinAmount = 0;
    myLossAmount = 0;
    myCantPlay = false;
}

void INeedToRemoveThisLater::ShowRules(Casino& aCasino)
{
    DrawMenuLine();
    std::cout << FromMinigameToChar(myMinigameType) << " RULES\n";
    DrawMenuLine();

    switch (myMinigameType)
    {
    case EMinigameType::GuessTheDiceSum:
        WriteLine(
            "I'll roll two dice and your only goal from that point\nonwards is to guess the sum produced the aforementioned\nrolled dice. It's important to note: Your guess should\n**NOT** exceed 12 or subceed 2!");
        break;
    case EMinigameType::OddOrEven:
        WriteLine(
            "Here you must guess if the dice are even or odd. If both\ndice don't show even or odd numbers, or if you guess\nincorrectly(i.e. guessing odd but both dice show even),\nthe house wins.");
        break;
    case EMinigameType::YesOrNo:
        WriteLine("Your sole objective is to guess whether the number picked\nis within the given range. ", false);
        WriteLine("Use 'y' if you think the number\nis in range and 'n' to guess if it is not. ", false);
        std::cout << "Every time you\nhave " << myRewardIncreaseThreshold <<
            " consecutive correct guesses in a row, your reward\nmight increase!" << '\n';
        WriteLine("\nNOTE: If you quit before guessing, you'll automatically\nlose!");
        break;
    case EMinigameType::HigherOrLower:
        WriteLine(
            "The goal is simple: Guess whether or not the number visible\non the screen is lower than the next one. ",
            false);
        WriteLine(
            "Guess correctly\nand you'll get rewarded. However, one wrong guess will\nturn your fortune into demise!\n");
        WriteLine("(By quitting without any guesses, you'll forfeit\nyour profit)");
        break;
    case EMinigameType::Roulette:
        WriteLine(
            "Your main objective is to guess what color the ball will\nland on. ", false);
        WriteLine("First, you'll have to pick how you want to bet:");

        WriteLine("** STRAIGHT **: Requires you to bet on a rouge. You're\nonly able to guess 0-36");
        WriteLine("** RED/BLACK **: Picking this will require you to pick\neither red or black");
        WriteLine("** ODD/EVEN **: You'll need to guess either odd or even.");
        WriteLine("** COLUMN BET **: Allows you to bet from 0-3. In this case,\nzero is standalone.\n");

        WriteLine(
            "If you guess correctly, your pay out stays the same.\n(If you bet on 0 and it lands, you'll win 36x your bet!");
        break;
    }

    Pause();
    ClearConsole();
    EnterGameMenu(aCasino);
}

void INeedToRemoveThisLater::OnPlay(Casino& aCasino)
{
    bool isWinner = false;
    int winAmount = 0;

    switch (myMinigameType)
    {
    case EMinigameType::GuessTheDiceSum:
        int playerGuess;

        ForceInput(playerGuess);

        while (playerGuess < aCasino.dice.diceSumMin || playerGuess > aCasino.dice.diceSumMax)
        {
            std::cout << "Please enter a valid guess!" << '\n';
            std::cin >> playerGuess;
        }

        BroadcastDiceResult(aCasino, true);
        isWinner = playerGuess == aCasino.dice.diceSum;
        break;
    case EMinigameType::OddOrEven:
        {
            myFirstChoice = 'E';
            mySecondChoice = 'O';

            char picked;
            std::cin >> picked;

            myHasPlayerPickedFirst = IsCharacter(picked, myFirstChoice);
            myHasPlayerPickedSecond = IsCharacter(picked, mySecondChoice);

            bool isEven = IsEven(aCasino.dice.die1) && IsEven(aCasino.dice.die2);
            bool isOdd = !IsEven(aCasino.dice.die1) && !IsEven(aCasino.dice.die2);

            while (!myHasPlayerPickedFirst && !myHasPlayerPickedSecond)
            {
                if (!myHasPlayerPickedFirst && !myHasPlayerPickedSecond)
                {
                    WriteLine("Please write 'e' for even or 'o' for odd");
                }

                while (std::cin.fail())
                {
                    ClearInput();
                    std::cin >> picked;
                }

                std::cin >> picked;
                myHasPlayerPickedFirst = IsCharacter(picked, myFirstChoice);
                myHasPlayerPickedSecond = IsCharacter(picked, mySecondChoice);
            }

            isWinner = (myHasPlayerPickedFirst && isEven) || (myHasPlayerPickedSecond && isOdd);

            BroadcastDiceResult(aCasino, false);
        }
        break;
    case EMinigameType::YesOrNo:
        {
            myRewardMultiplier = 1;

            myFirstChoice = 'Y';
            mySecondChoice = 'N';

            int actualRoll = GetRoll();

            int numConsecutiveCorrectGuesses = 0;

            char playerInput;
            std::cin >> playerInput;
            myHasPlayerPickedFirst = IsCharacter(playerInput, myFirstChoice);
            myHasPlayerPickedSecond = IsCharacter(playerInput, mySecondChoice);

            myIsPlayerQuitting = IsCharacter(playerInput, 'Q');

            while (!myIsPlayerQuitting)
            {
                if (!myHasPlayerPickedFirst && !myHasPlayerPickedSecond)
                {
                    WriteLine("Please write 'y' for yes or 'n' for no");
                }

                while (std::cin.fail())
                {
                    ClearInput();
                    std::cin >> playerInput;
                }

                if (myHasPlayerPickedFirst || myHasPlayerPickedSecond)
                {
                    if (myHasPlayerPickedSecond && !IsInRange(actualRoll, myRangeStart, myRangeEnd) ||
                        myHasPlayerPickedFirst && IsInRange(
                            actualRoll, myRangeStart, myRangeEnd))
                    {
                        numConsecutiveCorrectGuesses++;
                        if (numConsecutiveCorrectGuesses >= myRewardIncreaseThreshold)
                        {
                            numConsecutiveCorrectGuesses = 0;
                            myRewardMultiplier++;
                            std::cout << "Your reward multiplier has increased! It is now " << myRewardMultiplier << "!"
                                << '\n';
                        }

                        actualRoll = GetRoll();
                        myRangeStart = GetRoll();
                        myRangeEnd = GetRoll();

                        if (myRangeStart > myRangeEnd)
                        {
                            int temp = myRangeEnd;
                            myRangeStart = myRangeEnd;
                            myRangeEnd = temp;
                        }

                        if (myRangeStart == myRangeEnd)
                        {
                            myRangeEnd++;
                        }

                        std::cout << "Is the number between " << myRangeStart << " and " << myRangeEnd << "?" << '\n';

                        myCachedReward += myBaseReward;
                    }
                    else
                    {
                        myCachedReward = 0; // Reset the cached reward if the player guesses wrong
                        std::cout << "You guessed wrong! The number was " << actualRoll << " which means it was " << (
                            IsInRange(actualRoll, myRangeStart, myRangeEnd) ? "in" : "not in") << " the range." << '\n';
                        break;
                    }
                }

                std::cin >> playerInput;

                myHasPlayerPickedFirst = IsCharacter(playerInput, myFirstChoice);
                myHasPlayerPickedSecond = IsCharacter(playerInput, mySecondChoice);
                myIsPlayerQuitting = IsCharacter(playerInput, 'q');
            }
        }

        break;
    case EMinigameType::HigherOrLower:
        {
            bool isHigher = myRangeStart > myRangeEnd;

            myFirstChoice = 'H';
            mySecondChoice = 'L';

            char playerPick;
            std::cin >> playerPick;

            myHasPlayerPickedFirst = IsCharacter(playerPick, myFirstChoice);
            myHasPlayerPickedSecond = IsCharacter(playerPick, mySecondChoice);

            myIsPlayerQuitting = IsCharacter(playerPick, myQuitChoice);

            while (!myIsPlayerQuitting)
            {
                if (!myHasPlayerPickedFirst && !myHasPlayerPickedSecond)
                {
                    WriteLine("Please write 'h' for higher or 'l' for lower");
                }

                while (std::cin.fail())
                {
                    ClearInput();
                    std::cin >> playerGuess;
                }

                if (myHasPlayerPickedFirst || myHasPlayerPickedSecond)
                {
                    if (myHasPlayerPickedFirst && isHigher || myHasPlayerPickedSecond && !isHigher)
                    {
                        // It's flipped hehe
                        std::cout << "That's correct! " << myRangeStart << " is " << (isHigher ? "higher" : "lower") <<
                            " than " << myRangeEnd << "!" << "\n";
                        myCachedReward += myBaseReward;

                        int temp = myRangeEnd;
                        myRangeStart = temp;
                        myRangeEnd = GetRoll();

                        while (myRangeStart == myRangeEnd)
                        {
                            myRangeStart = GetRoll();
                        }

                        isHigher = myRangeStart > myRangeEnd;

                        std::cout << "Do you think " << myRangeStart << " is higher or lower than the next number?" <<
                            '\n';
                    }
                    else
                    {
                        std::cout << "The number after " << myRangeStart << " was " << myRangeEnd << "!\n";
                        WriteLine("Better luck next time!");
                        myCachedReward = 0;
                        break;
                    }
                }

                std::cin >> playerPick;
                myHasPlayerPickedFirst = IsCharacter(playerPick, myFirstChoice);
                myHasPlayerPickedSecond = IsCharacter(playerPick, mySecondChoice);
                myIsPlayerQuitting = IsCharacter(playerPick, 'Q');
            }
        }
        break;
    case EMinigameType::Roulette:
        {
            char correctPick = ' ';
            int correctRow = 0;
            int landedCol = -1;

            int landing = GetRandomNumber(aCasino.game.rouletteStraightMin, aCasino.game.rouletteStraightMax);
            winAmount = landing == 0 ? myBet * myAlternativeRewardMultiplier : myBet * myRewardMultiplier;
            std::array<int, 12> correctRowArray;

            int playerDecision;
            std::cin >> playerDecision;

            while (std::cin.fail())
            {
                ClearInput();
                std::cin >> playerDecision;
            }

            playerDecision = Clamp(playerDecision, aCasino.game.rouletteBettingTypeMin,
                                   aCasino.game.rouletteBettingTypeMax);

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
                
                myFirstChoice = 'R';
                mySecondChoice = 'B';

                int cachedRoll = GetRoll();
                bool isRedPicked = HasSubceeded(cachedRoll, 50) && cachedRoll > 0;

                correctPick = isRedPicked ? myFirstChoice : mySecondChoice;

                char playerPick;
                std::cin >> playerPick;

                // These two are in place to prevent the player from writing nonsense, ultimately preventing players from losing because of it (although I found it very funny)
                myHasPlayerPickedFirst = IsCharacter(playerPick, myFirstChoice);
                myHasPlayerPickedSecond = IsCharacter(playerPick, mySecondChoice);

                bool isCorrectPick = IsCharacter(playerPick, correctPick);

                while (!myHasPlayerPickedFirst && !myHasPlayerPickedSecond)
                {
                    WriteLine("Please make a valid pick!");

                    while (std::cin.fail())
                    {
                        ClearInput();
                        std::cin >> playerPick;
                    }

                    std::cin >> playerPick;

                    myHasPlayerPickedFirst = IsCharacter(playerPick, myFirstChoice);
                    myHasPlayerPickedSecond = IsCharacter(playerPick, mySecondChoice);
                }

                isWinner = isCorrectPick;
            }
            else if (playerDecision == aCasino.game.rouletteOddEven)
            {
                myFirstChoice = 'O';
                mySecondChoice = 'E';

                bool isLandingEven = IsEven(landing);

                WriteLine("What's your guess? ('e' for even, 'o' for odd)");

                char playerPick;
                std::cin >> playerPick;

                myHasPlayerPickedFirst = IsCharacter(playerPick, myFirstChoice);
                myHasPlayerPickedSecond = IsCharacter(playerPick, mySecondChoice);

                while (!myHasPlayerPickedFirst && !myHasPlayerPickedSecond)
                {
                    WriteLine("Please make a valid pick!");

                    while (std::cin.fail())
                    {
                        ClearInput();
                        std::cin >> playerPick;
                    }

                    std::cin >> playerPick;

                    myHasPlayerPickedFirst = IsCharacter(playerPick, myFirstChoice);
                    myHasPlayerPickedSecond = IsCharacter(playerPick, mySecondChoice);
                }

                isWinner = myHasPlayerPickedFirst && !isLandingEven || myHasPlayerPickedSecond && isLandingEven;
            }
            else
            {
                int correctRowIndex = GetRandomNumber(0, 3);
                correctRowArray = BuildRow(correctRowIndex);
                correctRow = correctRowIndex;
                landedCol = GetRandomNumber(aCasino.game.rouletteRowMin, aCasino.game.rouletteRowMax);

                WriteLine("Which row are you picking? (0-3)");

                int playerRowPick;
                std::cin >> playerRowPick;

                while (std::cin.fail())
                {
                    WriteLine("Please make a valid pick!");
                    ClearInput();
                    std::cin >> playerRowPick;
                }

                playerRowPick = Clamp(playerRowPick, 0, aCasino.game.rouletteRowMax);

                winAmount = correctRow == 0
                                ? myBet * myAlternativeRewardMultiplier
                                : myBet * myRewardMultiplier;
                isWinner = playerRowPick == correctRowIndex;
            }

            bool hasNumbers = playerDecision == aCasino.game.rouletteStraight || playerDecision == aCasino.game.
                rouletteOddEven;
            bool hasRow = playerDecision == aCasino.game.rouletteColumnRow;

            if (hasRow)
            {
                bool isAboveZero = correctRow > 0;

                std::cout << (isWinner ? "Congratulations! It landed on" : "Nice try! It landed on") << " " <<
                    correctRow;
                if (isAboveZero) std::cout << "(" << (isAboveZero ? correctRowArray[landedCol] : 0) << ")" << '\n';
                else std::cout << '\n';
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
        break;
    }

    if (myIsForcedWinAmount) winAmount = (myUseCachedReward
                                              ? myBet + (myCachedReward * myRewardMultiplier)
                                              : myBet * myRewardMultiplier);
    bool hasWonMoney = winAmount > 0;

    if (myIsForcedVictory) isWinner = (!myShouldPlayerQuit ? hasWonMoney : hasWonMoney && myIsPlayerQuitting);

    BroadcastWinOrLoss(aCasino, isWinner, winAmount, myBet,
                       isWinner ? myWinAmount : myLossAmount);
}
