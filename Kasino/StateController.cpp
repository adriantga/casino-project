#include "Helpers.h"
#include "Kasino.h"
#include "StateController.h"

#include <iostream>

#include "Utilities.h"
#include "MenuController.h"

void Pick(int aChoice, bool aIsInGame, Casino& aCasino)
{
    ClearConsole();
    if (aIsInGame)
    {
        int aMinigamesCount = Constants::minigameCount;

        aChoice = Clamp(aChoice, 1, aMinigamesCount + 2);

        if (aChoice > aMinigamesCount)
        {
            if (aChoice == aMinigamesCount + 1)
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
            aCasino.EnterGameMenu(aCasino.game.currentMinigame);
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
            Menu::EnterNamePicker(aCasino);
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
        Menu::EnterGamePicker(aCasino);
        break;
    case EStates::MainMenu:
        Menu::EnterMainMenu(aCasino);
        break;
    }
}
