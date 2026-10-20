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
    for (char& aMatch : aCasino.playerStats.games)
    {
        aMatch = '-';
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

    for (int i = 0; i < Constants::tableCount; i++)
    {
        ETableType tableType = aCasino.minigameTypes[i];
        const char* tableName = FromMinigameToChar(tableType);
        int aWinAmount = 0;
        int aLossAmount = 0;
        GetMinigameStats(aCasino, i, aWinAmount, aLossAmount);

        std::cout << tableName << " Profit: " << aWinAmount << '\n';
        std::cout << tableName << " Loss: " << aLossAmount << '\n';

        // The last line-breaker is unnecessary
        if (i != Constants::tableCount - 1) DrawBreakerLine();
    }

    std::cout << "================== GAME HISTORY ==================" << '\n';

    for (char aMatch : aCasino.playerStats.games)
    {
        if (IsCharacter(aMatch, '-'))
        {
            resultCount++;
        }

        std::cout << aMatch << " ";
    }

    std::cout << "\n==================================================" << '\n';
}