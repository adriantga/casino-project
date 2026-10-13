#pragma once
#include <array>
#include <string>

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

struct Minigames
{
    static constexpr int MINIGAME_GUESS_THE_DICE_SUM{0};
    static constexpr int MINIGAME_ODD_OR_EVEN = {1};
    static constexpr int MINIGAME_YES_OR_NO = {2};
    static constexpr int MINIGAME_HIGHER_OR_LOWER = {3};
    static constexpr int MINIGAME_ROULETTE = {4};

    static constexpr int LOW_NO_STAKES_MIN = {0};
    static constexpr int LOW_NO_STAKES_MAX = {1};
    static constexpr int HIGH_STAKES_MIN = {2};
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

struct MinigameSettings
{
    int rewardMultiplier = 1;
    int winImpressAmount = 500;
    int lossTauntAmount = 500;
    int rewardIncreaseThreshold = 0;

    int cachedReward = 0;
    bool useCachedReward = false;

    // Used by 'Roulette' and 'Yes or No'
    int rangeStart = 0;
    int rangeEnd = 0;

    char firstChoice = ' ';
    char secondChoice = ' ';
    char quitChoice = 'Q';

    // Only used by 'Roulette' at the moment
    int alternativeRewardMultiplier = 1;
    int baseReward = 0;

    bool shouldPlayerQuit = false;
    bool isPlayerQuitting = false;
    bool isForcedVictory = false;
    bool isForcedWinAmount = true;

    bool hasPlayerPickedFirst = false;
    bool hasPlayerPickedSecond = false;
    
    bool isPlayingHighStakes = false;

    EMinigameType minigameType;
    int winLimit = 0;
};

struct BettingSettings
{
    std::array<int, 3> allowedBets = {0, 0};
    int minAllowedBet = 0;
    int maxAllowedBet = 0;
};

struct Minigame
{
    int winAmount = 0;
    int lossAmount = 0;
    bool cantPlay = false;
};

struct Casino;
