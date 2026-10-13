#include "MenuController.h"

#include <iostream>

#include "StateController.h"
#include "Utilities.h"

namespace Menu
{
    void EnterMainMenu(Casino& aCasino)
    {
        int aInput;
        DrawMenu(aCasino, aInput, "THE ULTIMATE CASINO", "[1] Play Game\n[2] About\n[3] Stats\n[4] Quit", 4);
        Pick(aInput, false, aCasino);
    }
    
     void EnterGamePicker(Casino& aCasino)
    {
        const int aOptions = static_cast<int>(aCasino.minigameTypes.size()) + 2;

        int aInput;

        // Sooner or later I'll have to change this based on 
        DrawTitle("GAME PICKER");

        int aMinigameCount = int(aCasino.minigameTypes.size());

        for (int aI = 0; aI < aMinigameCount; aI++)
        {
            EMinigameType aMinigameType = aCasino.minigameTypes[aI];
            std::cout << "[" << (aI + 1) << "] " << FromMinigameToChar(aMinigameType) << '\n';
        }

        DrawBreakerLine();

        std::cout << "[" << (aMinigameCount + 1) << "] " << "Cash Out" << '\n';
        std::cout << "[" << (aMinigameCount + 2) << "] " << "Back To Menu" << '\n';

        DrawMenuLine();

        BroadcastPlayerBalance(false, aCasino);

        ForceInput(aInput);

        aInput = Clamp(aInput, 1, aOptions);

        Pick(aInput, true, aCasino);
    }

    void EnterNamePicker(Casino& aCasino)
    {
        if (aCasino.player.hasEnteredName)
        {
            EnterGamePicker(aCasino);
            return;
        }

        char aConfirmName = 'Y';
        char aDenyName = 'N';

        WriteLine("What's your name? (Must be between 2-16 characters)");

        char aPlayerNameInput[17];
        std::cin >> aPlayerNameInput;

        while (std::cin.fail() || !IsValidName(aPlayerNameInput))
        {
            WriteLine("Please enter letters only! / Ensure the name is between 2-16 characters.");
            ClearInput();
            std::cin >> aPlayerNameInput;
        }

        ClearInput();

        std::cout << "Your name is: " << aPlayerNameInput << ". Continue? (y to confirm, n to deny)" << '\n';

        char aConfirmInput;
        std::cin >> aConfirmInput;
        while (std::cin.fail())
        {
            ClearInput();
            std::cin >> aConfirmInput;
            WriteLine("Please confirm or deny!");
        }

        bool aHasConfirmedName = IsCharacter(aConfirmInput, aConfirmName);
        bool aHasDeniedName = IsCharacter(aConfirmInput, aDenyName);

        if (!aHasConfirmedName && !aHasDeniedName)
        {
            std::cin >> aConfirmInput;

            aHasConfirmedName = IsCharacter(aConfirmInput, aConfirmName);
            aHasDeniedName = IsCharacter(aConfirmInput, aDenyName);
        }

        if (aHasConfirmedName)
        {
            aCasino.player.hasEnteredName = true;
            aCasino.player.name = aPlayerNameInput;

            ClearConsole();
            EnterGamePicker(aCasino);
        }
        else if (aHasDeniedName)
        {
            ClearConsole();
            EnterNamePicker(aCasino);
        }
    }
   
}
