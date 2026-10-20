#include "MenuController.h"

#include <iostream>

#include "StateController.h"
#include "Utilities.h"

namespace Menu
{
    void EnterMainMenu(Casino& aCasino)
    {
        int input;
        DrawMenu(aCasino, input, "THE ULTIMATE CASINO", "[1] Play Game\n[2] About\n[3] Stats\n[4] Quit", 4);
        Pick(input, false, aCasino);
    }
    
     void EnterGamePicker(Casino& aCasino)
    {
        const int OPTIONS = static_cast<int>(aCasino.minigameTypes.size()) + 2;

        int input;

        DrawTitle("GAME PICKER");

        int minigameCount = int(aCasino.minigameTypes.size());

        for (int i = 0; i < minigameCount; i++)
        {
            ETableType minigameType = aCasino.minigameTypes[i];
            std::cout << "[" << (i + 1) << "] " << FromMinigameToChar(minigameType) << '\n';
        }

        DrawBreakerLine();

        std::cout << "[" << (minigameCount + 1) << "] " << "Cash Out" << '\n';
        std::cout << "[" << (minigameCount + 2) << "] " << "Back To Menu" << '\n';

        DrawMenuLine();

        BroadcastPlayerBalance(false, aCasino);

        ForceInput(input);

        input = Clamp(input, 1, OPTIONS);

        Pick(input, true, aCasino);
    }

    void EnterNamePicker(Casino& aCasino)
    {
        if (aCasino.player.hasEnteredName)
        {
            EnterGamePicker(aCasino);
            return;
        }

        char confirmName = 'Y';
        char denyName = 'N';

        WriteLine("What's your name? (Must be between 2-16 characters)");

        char playerNameInput[17];
        std::cin >> playerNameInput;

        while (std::cin.fail() || !IsValidName(playerNameInput))
        {
            WriteLine("Please enter letters only! / Ensure the name is between 2-16 characters.");
            ClearInput();
            std::cin >> playerNameInput;
        }

        ClearInput();

        std::cout << "Your name is: " << playerNameInput << ". Continue? (y to confirm, n to deny)" << '\n';

        char confirmInput;
        std::cin >> confirmInput;
        while (std::cin.fail())
        {
            ClearInput();
            std::cin >> confirmInput;
            WriteLine("Please confirm or deny!");
        }

        bool hasConfirmedName = IsCharacter(confirmInput, confirmName);
        bool hasDeniedName = IsCharacter(confirmInput, denyName);

        if (!hasConfirmedName && !hasDeniedName)
        {
            std::cin >> confirmInput;

            hasConfirmedName = IsCharacter(confirmInput, confirmName);
            hasDeniedName = IsCharacter(confirmInput, denyName);
        }

        if (hasConfirmedName)
        {
            aCasino.player.hasEnteredName = true;
            aCasino.player.name = playerNameInput;

            ClearConsole();
            EnterGamePicker(aCasino);
        }
        else if (hasDeniedName)
        {
            ClearConsole();
            EnterNamePicker(aCasino);
        }
    }
}