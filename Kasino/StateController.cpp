#include "Helpers.h"
#include "Kasino.h"
#include "StateController.h"

#include <iostream>

#include "INeedToRemoveThisLater.h"
#include "Utilities.h"

void Pick(int aChoice, bool aIsInGame, Casino& aCasino)
{
    ClearConsole();
    if (aIsInGame)
    {
        int minigamesCount = int(aCasino.minigames.size());

        aChoice = Clamp(aChoice, 1, minigamesCount + 2);

        if (aChoice > aCasino.minigames.size())
        {
            if (aChoice == aCasino.minigames.size() + 1)
            {
                SwitchTo(EOptions::CashOut, aCasino);
            }
            else
            {
                ChangeState(EStates::MainMenu, aCasino);
            }
        }
        else
        {
            aCasino.game.currentMinigame = aChoice - 1;
            aCasino.minigames[aCasino.game.currentMinigame].EnterGameMenu(aCasino);
        }

        return;
    }

    switch (aChoice)
    {
    case 1:
        if (aCasino.player.hasEnteredName)
        {
            ChangeState(EStates::Game, aCasino);
        }
        else
        {
            EnterNamePicker(aCasino);
        }
        break;
    case 2:
        SwitchTo(EOptions::About, aCasino);
        break;
    case 3:
        SwitchTo(EOptions::Stats, aCasino);
        break;
    case 4:
        SwitchTo(EOptions::Quit, aCasino);
        break;
    }
}

void ChangeState(EStates aTargetState, Casino& aCasino)
{
    switch (aTargetState)
    {
    case EStates::Game:
        EnterGamePicker(aCasino);
        break;
    case EStates::MainMenu:
        EnterMainMenu(aCasino);
        break;
    }
}
