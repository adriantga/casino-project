#include "Stats.h"
#include "Utilities.h"
#include "Helpers.h"
#include <iostream>

#include "INeedToRemoveThisLater.h"

void AddPlayedGame(Casino& aCasino, bool aIsWinner)
{
    aCasino.playerStats.gamesPlayed++;

    // This is a bit of a hack. It shifts the array to the right, and adds the new result to the front.
    // Certainly not the best way to do this but it is readable.
    int historySize = sizeof(aCasino.playerStats.games) / sizeof(aCasino.playerStats.games[0]);
    for (int i = historySize - 1; i > 0; i--)
    {
        aCasino.playerStats.games[i] = aCasino.playerStats.games[i - 1];
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

    int resultIndex = aIsWin ? aCasino.playerStats.winIndex : aCasino.playerStats.lossIndex;
    aCasino.playerStats.games[aMatchIndex] = aCasino.playerStats.gameOutcomes[resultIndex];
}

void ResetStats(Casino& aCasino)
{
    for (char& match : aCasino.playerStats.games)
    {
        match = '-';
    }
}

void ShowStats(Casino& aCasino)
{
    ClearConsole();

    int resultCount = 0;

    std::cout << "================== PLAYER STATS ==================" << '\n';
    std::cout << "Games Played: " << aCasino.playerStats.gamesPlayed << '\n';
    std::cout << "Wins: " << aCasino.playerStats.wins << '\n';
    std::cout << "Losses: " << aCasino.playerStats.losses << '\n';
    std::cout << "================= GAME OUTCOMES ==================" << '\n';


    for (int i = 0; i < Constants::minigameCount; i++)
    {
        INeedToRemoveThisLater minigame = aCasino.minigames[i];
        EMinigameType minigameType = aCasino.minigameTypes[i];
        const char* minigameName = FromMinigameToChar(minigameType);

        std::cout << minigameName << " Profit: " << minigame.myWinAmount << '\n';
        std::cout << minigameName << " Loss: " << minigame.myLossAmount << '\n';

        // The last line-breaker is unnecessary
        if (i != Constants::minigameCount - 1) DrawBreakerLine();
    }

    std::cout << "================== GAME HISTORY ==================" << '\n';

    for (char match : aCasino.playerStats.games)
    {
        if (IsCharacter(match, '-'))
        {
            resultCount++;
        }

        std::cout << match << '\n';
    }

    if (resultCount == (sizeof(aCasino.playerStats.games) / sizeof(aCasino.playerStats.games[0])))
    {
        std::cout << "(No games played!)" << '\n';
    }

    std::cout << "===================================================" << '\n';
}
