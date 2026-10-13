#include "Stats.h"
#include "Utilities.h"
#include "Helpers.h"
#include <iostream>

void GetMinigameStats(const Casino& aCasino, int aIndex, int& aWinAmount, int& aLossAmount)
{
    switch (aIndex)
    {
    case Minigames::MINIGAME_GUESS_THE_DICE_SUM:
        aWinAmount = aCasino.guessTheDiceSumTable.GetWinAmount();
        aLossAmount = aCasino.guessTheDiceSumTable.GetLossAmount();
        break;
    case Minigames::MINIGAME_ODD_OR_EVEN:
        aWinAmount = aCasino.oddOrEvenTable.GetWinAmount();
        aLossAmount = aCasino.oddOrEvenTable.GetLossAmount();
        break;
    case Minigames::MINIGAME_YES_OR_NO:
        aWinAmount = aCasino.yesOrNoTable.GetWinAmount();
        aLossAmount = aCasino.yesOrNoTable.GetLossAmount();
        break;
    case Minigames::MINIGAME_HIGHER_OR_LOWER:
        aWinAmount = aCasino.higherOrLowerTable.GetWinAmount();
        aLossAmount = aCasino.higherOrLowerTable.GetLossAmount();
        break;
    case Minigames::MINIGAME_ROULETTE:
        aWinAmount = aCasino.rouletteTable.GetWinAmount();
        aLossAmount = aCasino.rouletteTable.GetLossAmount();
        break;
    default:
        aWinAmount = 0;
        aLossAmount = 0;
        break;
    }
}

void AddPlayedGame(Casino& aCasino, bool aIsWinner)
{
    aCasino.playerStats.gamesPlayed++;

    // This is a bit of a hack. It shifts the array to the right, and adds the new result to the front.
    // Certainly not the best way to do this but it is readable.
    int aHistorySize = sizeof(aCasino.playerStats.games) / sizeof(aCasino.playerStats.games[0]);
    for (int aI = aHistorySize - 1; aI > 0; aI--)
    {
        aCasino.playerStats.games[aI] = aCasino.playerStats.games[aI - 1];
    }

    SetResult(aCasino, 0, aIsWinner);
}

void SetResult(Casino& aCasino, int aMatchIndex, bool aIsWin)
{
    if (aIsWin)
    {
        aCasino.playerStats.wins++;
    }
    else
    {
        aCasino.playerStats.losses++;
    }

    int aResultIndex = aIsWin ? aCasino.playerStats.winIndex : aCasino.playerStats.lossIndex;
    aCasino.playerStats.games[aMatchIndex] = aCasino.playerStats.gameOutcomes[aResultIndex];
}

void ResetStats(Casino& aCasino)
{
    for (char& aMatch : aCasino.playerStats.games)
    {
        aMatch = '-';
    }
}

void ShowStats(Casino& aCasino)
{
    ClearConsole();

    int aResultCount = 0;

    std::cout << "================== PLAYER STATS ==================" << '\n';
    std::cout << "Games Played: " << aCasino.playerStats.gamesPlayed << '\n';
    std::cout << "Wins: " << aCasino.playerStats.wins << '\n';
    std::cout << "Losses: " << aCasino.playerStats.losses << '\n';
    std::cout << "================= GAME OUTCOMES ==================" << '\n';

    for (int aI = 0; aI < Constants::minigameCount; aI++)
    {
        EMinigameType aMinigameType = aCasino.minigameTypes[aI];
        const char* aMinigameName = FromMinigameToChar(aMinigameType);
        int aWinAmount = 0;
        int aLossAmount = 0;
        GetMinigameStats(aCasino, aI, aWinAmount, aLossAmount);

        std::cout << aMinigameName << " Profit: " << aWinAmount << '\n';
        std::cout << aMinigameName << " Loss: " << aLossAmount << '\n';

        // The last line-breaker is unnecessary
        if (aI != Constants::minigameCount - 1) DrawBreakerLine();
    }

    std::cout << "================== GAME HISTORY ==================" << '\n';

    for (char aMatch : aCasino.playerStats.games)
    {
        if (IsCharacter(aMatch, '-'))
        {
            aResultCount++;
        }

        std::cout << aMatch << " ";
    }

    std::cout << "\n==================================================" << '\n';
}
