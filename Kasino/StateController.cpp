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
        int aMinigamesCount = Constants::tableCount;

        aChoice = Clamp(aChoice, 1, aMinigamesCount + 2);

        if (aChoice > aMinigamesCount)
        {
            if (aChoice == aMinigamesCount + 1)
            {
                SwitchTo(EOptions::CashOut, aCasino);
            }
            else
            {
                ChangeState(EGameState::MainMenu, aCasino);
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
            ChangeState(EGameState::Game, aCasino);
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

void ChangeState(EGameState aTargetState, Casino& aCasino)
{
    switch (aTargetState)
    {
    case EGameState::Game:
        Menu::EnterGamePicker(aCasino);
        break;
    case EGameState::MainMenu:
        Menu::EnterMainMenu(aCasino);
        break;
    }
}
