#pragma once
#include <array>
#include <string>

enum class EMinigameType;
class Minigame;
struct Constants;

struct Constants
{
    static const int minigameCount = 5;
    static const int startingBalance = 250;
    static const int playerNameMinSize = 2;
    static const int playerNameMaxSize = 16;
};

struct Player
{
    int money = 0;
    const char*& name;
    bool& hasEnteredName;
    
    std::array<int, 3> standardBets;
    std::array<int, 3> stakeBets;
};

struct PlayerStats
{
    const int winIndex = 0;
    const int lossIndex = 1;

    int wins = 0;
    int losses = 0;
    int gamesPlayed = 0;

    char games[5] = {'-', '-', '-', '-', '-'};
    const char gameOutcomes[2] = {'W', 'L'};
};

struct Dice
{
    const int diceSumMin = 2;
    const int diceSumMax = 12;

    int die1 = 0;
    int die2 = 0;
    int diceSum = 0;
};

struct Game
{
    const int rouletteBettingTypeMin = 1;
    const int rouletteBettingTypeMax = 4;

    const int rouletteStraightMin = 0;
    const int rouletteStraightMax = 36;

    const int rouletteStraight = 1;
    const int rouletteRedBlack = 2;
    const int rouletteOddEven = 3;
    const int rouletteColumnRow = 4;

    const int rouletteRowMin = 1;
    const int rouletteRowMax = 3;

    const int rouletteColumnMin = 0;
    const int rouletteColumnMax = 12;

    int currentMinigame;

    bool isGameOver = false;
    bool isQuitting = false;
};

struct Casino
{
    Dice& dice;
    Player& player;
    PlayerStats& playerStats;
    Game& game;
    std::array<Minigame, Constants::minigameCount>& minigames;
    std::array<EMinigameType, Constants::minigameCount>& minigameTypes;
};

enum class EOptions
{
    CashOut,
    Stats,
    About,
    Quit
};

enum class EMinigameType
{
    GuessTheDiceSum = 1,
    OddOrEven = 2,
    YesOrNo = 3,
    HigherOrLower = 4,
    Roulette = 5
};

enum class EStates
{
    MainMenu,
    Game
};

enum class EStakeType
{
    High,
    Low
};
