#include "iGraphics.h"
#include "iSound.h"
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 510
#define BTN_NEWGAME_X 110
#define BTN_NEWGAME_Y 140
#define BTN_WIDTH 120
#define BTN_HEIGHT 30
#define BTN_SAVED_X 110
#define BTN_SAVED_Y 100
#define BTN_OPTIONS_X 110
#define BTN_OPTIONS_Y 60
#define BTN_EXIT_X 110
#define BTN_EXIT_Y 20
#define BTN_SCOREBOARD_X 350
#define BTN_SCOREBOARD_Y 300
#define BTN_SCOREBOARD_W 200
#define BTN_SCOREBOARD_H 40
#define BTN_GENERALSET_X 350
#define BTN_GENERALSET_Y 240
#define BTN_GENERALSET_W 200
#define BTN_GENERALSET_H 40
#define BTN_BACK_X 350
#define BTN_BACK_Y 180
#define BTN_BACK_W 200
#define BTN_BACK_H 40
#define STATE_MENU 0
#define STATE_PLAYING 1
#define STATE_OPTIONS 3
#define STATE_SCOREBOARD 4
#define STATE_SETTINGS 5
#define STATE_SAVED_GAMES 11
#define STATE_LEVEL_COMPLETE 12
#define STATE_GAME_OVER 13
#define STATE_WELCOME 99
#define STATE_ABOUT_US 100

#define BTN_SCORE_BACK_X 370
#define BTN_SCORE_BACK_Y 100
#define BTN_SCORE_BACK_W 150
#define BTN_SCORE_BACK_H 40
// adding a name state for name entry
#define STATE_NAME_INPUT 6
#define STATE_GAME_RUNNING 7
#define STATE_GAME_STARTED 8
#define STATE_PAUSED 9
#define MAX_SAVED_GAMES 5
char savedPlayerNames[MAX_SAVED_GAMES][100];
int savedLevels[MAX_SAVED_GAMES];
int savedGameCount = 0;
#define SAVED_BTN_X 320
#define SAVED_BTN_Y_START 320
#define SAVED_BTN_WIDTH 300
#define SAVED_BTN_HEIGHT 40
#define SAVED_BTN_GAP 50

#define PAUSE_BTN_X 20
#define PAUSE_BTN_Y (SCREEN_HEIGHT - 50) // This keeps it top-left corner
#define PAUSE_BTN_W 50
#define PAUSE_BTN_H 30

#define MAX_BUTTONS 20

float buttonHoverAlpha[MAX_BUTTONS]; // 0 to 1
bool buttonHovering[MAX_BUTTONS];    // true if mouse is over

Image saved_games_background;
// define coin count
#define LEVEL_WIDTH 1500
#define MIN_DISTANCE_BETWEEN 200
#define BASE_COIN_COUNT 50
#define BASE_LAB_COUNT 10

#define MAX_LINES 8
#define MAX_NAME_LEN 100

typedef struct
{
    char name[MAX_NAME_LEN];
    int level;
    int score;
} Entry;

// void updateLeaderboard(const char *name, int score, int level);
int base_coin_positions[BASE_COIN_COUNT] = {
    400, 600, 900, 1300, 1530, 1800, 2100, 2550, 2700, 3000,
    3300, 3600, 3900, 4200, 4300, 4850, 5175, 5435, 5700, 6000,
    6300, 6600, 6900, 7200, 7500, 7880, 8100, 8400, 8700, 9000,
    9360, 9600, 9900, 10100, 10500, 10830, 11120, 11400, 11700, 12000,
    12300, 12600, 12900, 13250, 13500, 13800, 14100, 14400, 14700, 15000};

// global variables for sprite animation
#define NUM_RUN_FRAMES 10
float velocityY = 0; // Vertical velocity
bool isJumping = false;
float gravity = -0.5f;
float groundY = 85;
#define TF_WIDTH 30
#define TF_HEIGHT 30
bool tfVisible = false;
int tfX = 0;
Image tfFrames[8];
int currentTfFrame = 0;
bool tf_visible = false;
int tf_position = 0;
#define TILE_WIDTH 8000
Image runFrames[NUM_RUN_FRAMES];
Image platformTiles[1];
int currentRunFrame = 0;
int runTimerId;
int playerX = 100, playerY = 100;
int pic_y = 85;
char playerName[100] = "";
int nameLength = 0;
bool isEnteringName = true;
const int MAX_SCORES = 5;
char playerNames[MAX_SCORES][20];
int playerScores[MAX_SCORES];
Image coin;
Image bg;
Image img;
Image lvlbg1;
int worldOffsetX = 0;
int hoveredSavedIndex = -1;
int savedScores[MAX_SAVED_GAMES];
int savedPositions[MAX_SAVED_GAMES]; // for worldOffsetX

int gameState = STATE_WELCOME; // 0 = menu, 1 = playing, etc.
// obstacle globals
#define OBSTACLE_FRAMES 4
Image rotatingObstacleFrames[OBSTACLE_FRAMES];
int currentObstacleFrame = 0;

int currentSaveSlot = -1;
int optionsReturnState = STATE_MENU; // default is to return to main menu
int t;
int welcomeTimer = 300; // 300 frames = ~10 seconds if timer interval is 33ms
int hoveredButtonID = -1;
int coin_sound;
int point_lose;
// int game_start;
int returnFromSettingsState = STATE_MENU; // default fallback

typedef struct
{
    char name[100];
    int level;
    int score;
} LeaderboardEntry;

LeaderboardEntry leaderboard[MAX_SCORES];
int leaderboardCount = 0;

// Adding sound

// animating the character sprite
enum
{
    IDLE,
    WALK,
    JUMP
};
enum ButtonIDs
{
    BTN_NONE = -1,
    BTN_NEWGAME,
    BTN_SAVEDGAMES,
    BTN_OPTIONS,
    BTN_EXIT,
    BTN_PAUSE,
    BTN_SCOREBOARD,
    BTN_GENERALSET,
    BTN_BACK,
    BTN_RESUME,
    BTN_SAVEEXIT,
    BTN_OPTIONS_PAUSED,
    BTN
    // ...add more if needed
};

int pic_x;
int tf_idx = 0;
int idle_idx = 0;
int jump_idx = 0;
int walk_idx = 0;
int state = IDLE;
char hero_idle[8][100];
char hero_jump[3][100];
char hero_walk[12][100];
char *hero_image;

Image coinFrames[6];

Image labFrames[8];
char lab_move[8][100];
char coin_move[6][100];
char platform_move[5][100];
char tf[8][100];
int currentCoinFrame = 0;
int currentLabFrame = 0;
int pic_coin_x = 1000;
int pic_coin_y;
int coin_dx = 50;
int ball_x, ball_y;
int dx = 5, dy = 7;
int ball_radius = 7;
#define MAX_COINS 1000
#define MAX_LAB 10
int playerScore = 0;
#define MAX_LEVELS 8
int levelScores[MAX_LEVELS] = {0};
int totalLevelsPlayed = 0;

int currentLevel = 0;

Image levelBackgrounds[MAX_LEVELS];
bool showCGPA = false;
int cgpaDisplayTimer = 200;

#define MAX_OBSTACLES 1000
int obstacle_positions[MAX_OBSTACLES]; // X positions in world
int obstacle_width = 70;
int obstacle_height = 70;
bool obstacle_visible[MAX_OBSTACLES];
int numObstacles[MAX_LEVELS] = {25};
int coin_positions[MAX_COINS];
int lab_positions[MAX_LAB];
bool coin_collected[MAX_COINS] = {false};
bool lab_collected[MAX_LAB] = {false};
int coin_interval = 600; // distance between coins in world
float velocityX = 0;
bool isAirborne = false; // Already covered by isJumping, but useful if you want
float baseSpeed = 10.0f;
float speedMultiplier = 1.0f;

bool soundOn = true;

int gameStartChannel = -1;
int coinChannel = -1;
int loseChannel = -1;
int bhag_bhagChannel = -1;

char terms[8][5] = {
    "1-1", "1-2", "2-1", "2-2",
    "3-1", "3-2", "4-1", "4-2"};
// Fixed positions for coins and obstacles per level
int fixedCoinPositions[MAX_LEVELS][BASE_COIN_COUNT] = {
    {300, 600, 950, 1250, 1550, 1850, 2150, 2450, 2750, 3050,
     3350, 3650, 3950, 4250, 4550, 4850, 5150, 5450, 5750, 6050,
     6350, 6650, 6950, 7250, 7550, 7850, 8150, 8450, 8750, 9050,
     9350, 9650, 9950, 10250, 10550, 10850, 11150, 11450, 11750, 12050,
     12350, 12650, 12950, 13250, 13550, 13850, 14150, 14450, 14750, 15000},
    // Add more levels as needed...
};

int fixedLabPositions[MAX_LEVELS][BASE_LAB_COUNT] = {
    {450, 2000, 2550, 4550, 6780, 9900, 11300, 13100, 14000, 14400},
    // Add more levels as needed...
};

int fixedObstaclePositions[MAX_LEVELS][MAX_OBSTACLES] = {
    {1100, 1700, 2300, 2900, 3500, 4100, 4700, 5300, 5900,
     6500, 7100, 7700, 8300, 8900, 9500, 10100, 10700, 11300, 11900,
     12450, 13000, 13780, 14300, 14900},

};

void returntoMenu()
{
    if (gameState == STATE_GAME_OVER)
    {
        printf("Return to Menu\n");
        gameState = STATE_MENU;
        iPauseSound(gameStartChannel);
        iResumeSound(t);
    }
}

void initializeTF()
{
    tfVisible = false;
    tfX = 0;
}

void drawButton(int id, int x, int y, int w, int h, const char *text)
{
    float alpha = buttonHoverAlpha[id];

    int r = (int)(26 + alpha * (180 - 26));
    int g = (int)(151 + alpha * (255 - 151));
    int b = (int)(42 + alpha * (180 - 42));

    int expand = (int)(alpha * 5); // makes the button "grow"

    iSetColor(r, g, b);
    iFilledRectangle(x - expand, y - expand, w + 2 * expand, h + 2 * expand);

    iSetColor(41, 19, 36);
    iText(x + 20, y + 10, text);
}

void appendToLeaderboard(const char *name, int level, int score)
{
    Entry entries[MAX_LINES + 1]; // +1 for potential new entry
    int count = 0;

    FILE *fp = fopen("leaderboard.txt", "r");
    if (fp != NULL)
    {
        while (fscanf(fp, "%s %d %d", entries[count].name, &entries[count].level, &entries[count].score) == 3)
        {
            count++;
            if (count >= MAX_LINES)
                break;
        }
        fclose(fp);
    }

    // Find index to replace
    int replaceIndex = -1;
    int bestLowerLevel = -1;
    int bestLowerScore = -1;

    for (int i = 0; i < count; i++)
    {
        if (entries[i].level < level)
        {
            if (entries[i].level > bestLowerLevel)
            {
                bestLowerLevel = entries[i].level;
                replaceIndex = i;
            }
        }
        else if (entries[i].level == level && entries[i].score < score)
        {
            if (entries[i].score > bestLowerScore)
            {
                bestLowerScore = entries[i].score;
                replaceIndex = i;
            }
        }
    }

    Entry newEntry;
    strncpy(newEntry.name, name, MAX_NAME_LEN);
    newEntry.level = level;
    newEntry.score = score;

    // Insert the new entry
    if (replaceIndex != -1)
    {
        // Shift lines down from replaceIndex
        for (int i = count < MAX_LINES ? count : MAX_LINES - 1; i > replaceIndex; i--)
        {
            entries[i] = entries[i - 1];
        }
        entries[replaceIndex] = newEntry;
        if (count < MAX_LINES)
            count++;
    }
    else if (count < MAX_LINES)
    {
        // No suitable replacement, but there's space, just append
        entries[count++] = newEntry;
    }

    // Truncate to MAX_LINES
    if (count > MAX_LINES)
        count = MAX_LINES;

    // Write back to file
    fp = fopen("leaderboard.txt", "w");
    if (fp == NULL)
    {
        printf("Error: Could not open leaderboard.txt to write.\n");
        return;
    }
    for (int i = 0; i < count; i++)
    {
        fprintf(fp, "%s %d %d\n", entries[i].name, entries[i].level, entries[i].score);
    }
    fclose(fp);
}

void loadAndSortLeaderboardFromFile(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        printf("Could not open leaderboard file.\n");
        return;
    }

    leaderboardCount = 0;

    while (fscanf(fp, "%s %d %d", leaderboard[leaderboardCount].name,
                  &leaderboard[leaderboardCount].score,
                  &leaderboard[leaderboardCount].level) == 3)
    {
        leaderboardCount++;
        if (leaderboardCount >= MAX_SCORES)
            break;
    }
    fclose(fp);

    // Sort descending: level first, then score
    for (int i = 0; i < leaderboardCount - 1; i++)
    {
        for (int j = i + 1; j < leaderboardCount; j++)
        {
            if (leaderboard[j].level > leaderboard[i].level ||
                (leaderboard[j].level == leaderboard[i].level &&
                 leaderboard[j].score > leaderboard[i].score))
            {
                LeaderboardEntry temp = leaderboard[i];
                leaderboard[i] = leaderboard[j];
                leaderboard[j] = temp;
            }
        }
    }
}
void saveSavedGamesToFile()
{
    FILE *fp = fopen("saved_games.txt", "w");
    if (fp == NULL)
        return;

    fprintf(fp, "%d\n", savedGameCount);
    for (int i = 0; i < savedGameCount; i++)
    {
        fprintf(fp, "%s %d %d %d\n", savedPlayerNames[i], savedLevels[i], savedScores[i], savedPositions[i]);
    }

    fclose(fp);
}
void showSortedLeaderboard()
{
    loadAndSortLeaderboardFromFile("leaderboard.txt");

    for (int i = 0; i < leaderboardCount && i < 5; i++)
    {
        char line[200];
        sprintf(line, "%d. %s - Level %d - CGPA: %.2f",
                i + 1,
                leaderboard[i].name,
                leaderboard[i].level + 1,
                leaderboard[i].score / 100.0);
        iText(300, 400 - i * 30, line);
    }
}

void loadSavedGamesFromFile()
{
    FILE *fp = fopen("saved_games.txt", "r");
    if (fp == NULL)
        return;

    fscanf(fp, "%d", &savedGameCount);
    for (int i = 0; i < savedGameCount; i++)
    {
        fscanf(fp, "%s %d %d %d", savedPlayerNames[i], &savedLevels[i], &savedScores[i], &savedPositions[i]);
    }

    fclose(fp);
}

// void saveLeaderboard()
// {
//     // updateLeaderboard();
//     // sortLeaderboard();
//     FILE *fp = fopen("leaderboard.txt", "w");
//     if (fp == NULL)
//     {
//         printf("Error: Could not save leaderboard!\n");
//         return;
//     }
//     for (int i = 0; i < MAX_SCORES; i++)
//     {
//         fprintf(fp, "%d %s %d\n", totalLevelsPlayed, playerNames[i], playerScores[i]);
//     }
//     loadAndSortLeaderboardFromFile("leaderboard.txt");
//     for (int i = 0; i < leaderboardCount; i++)
//     {
//         char msg[100];
//         sprintf(msg, "%s - Level %d - CGPA %.2f", leaderboard[i].name,
//                 leaderboard[i].level,
//                 leaderboard[i].score / 100.0);
//         iText(300, 400 - i * 30, msg);
//     }

//     fclose(fp);
//}

// void updateLeaderboard(const char *name, int score, int level)
// {
//     // Insert into leaderboard only if it's better than existing ones
//     for (int i = 0; i < MAX_SCORES; i++)
//     {
//         if (level > savedLevels[i] || (level == savedLevels[i] && score > playerScores[i]))
//         {
//             // Shift lower scores down
//             for (int j = MAX_SCORES - 1; j > i; j--)
//             {
//                 savedLevels[j] = savedLevels[j - 1];
//                 playerScores[j] = playerScores[j - 1];
//                 strcpy(playerNames[j], playerNames[j - 1]);
//             }

//             // Insert new score
//             savedLevels[i] = level;
//             playerScores[i] = score;
//             strncpy(playerNames[i], name, 19);
//             playerNames[i][19] = '\0';

//             break;
//         }
//     }

//     saveLeaderboard(); // Save to file after update
// }

bool isPositionFarFromObstacles(int posX, int obstacle_positions[], int count)
{
    for (int i = 0; i < count; i++)
    {
        if (abs(posX - obstacle_positions[i]) < MIN_DISTANCE_BETWEEN)
        {
            return false;
        }
    }
    return true;
}

void initializeObstacles()
{
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        if (i < numObstacles[currentLevel])
        {
            obstacle_positions[i] = fixedObstaclePositions[currentLevel][i];
            obstacle_visible[i] = true;
        }
        else
        {
            obstacle_positions[i] = 0;
            obstacle_visible[i] = false;
        }
    }
}

void initializeCoins()
{
    // Clear coins
    for (int i = 0; i < MAX_COINS; i++)
    {
        coin_collected[i] = true;
        coin_positions[i] = 0;
    }

    int count = BASE_COIN_COUNT;
    for (int i = 0; i < count && i < MAX_COINS; i++)
    {
        int pos = fixedCoinPositions[currentLevel][i];
        if (pos > 0)
        {
            coin_positions[i] = pos;
            coin_collected[i] = false;
        }
    }
}

void populateLab()
{
    for (int i = 0; i < 8; i++)
    {
        sprintf(lab_move[i], "assets/images/beaker/beaker%03d.png", i);
    }
}

void initializeLabTests()
{

    for (int i = 0; i < MAX_LAB; i++)
    {
        lab_collected[i] = true;
        lab_positions[i] = 0;
    }

    int count = BASE_LAB_COUNT;
    for (int i = 0; i < count && i < MAX_LAB; i++)
    {
        int pos = fixedLabPositions[currentLevel][i];
        if (pos > 0)
        {
            lab_positions[i] = pos;
            lab_collected[i] = false;
        }
    }
}

void drawPlatformFloor()
{
    int startX = -(worldOffsetX % TILE_WIDTH);

    for (int i = 0; i <= SCREEN_WIDTH / TILE_WIDTH + 2; i++)
    {
        int x = startX + i * TILE_WIDTH;
        iShowLoadedImage(x, 0, &platformTiles[0]);
    }
}

void populate_collectable()
{
    for (int i = 0; i < 7; i++)
    {
        sprintf(coin_move[i], "assets/images/Collectable_Object/Coin_%02d.png", i + 1);
    }
}
void populate_tf()
{
    for (int i = 0; i < 8; i++)
    {
        sprintf(tf[i], "assets/images/tf/tf%03d.png", i);
    }
}
void populate_hero_images()
{
    for (int i = 0; i < 8; i++)
    {
        sprintf(hero_idle[i], "assets/images/School_Boy/RCG2_Beanball_Idle_%02d.png", i + 1);
    }
    for (int i = 0; i < 3; i++)
    {
        sprintf(hero_jump[i], "assets/images/School_Boy/RCG_Jump_%02d.png", i + 1);
    }
    for (int i = 0; i < 10; i++)
    {
        sprintf(hero_walk[i], "assets/images/School_Boy/RCG_Run_%02d.png", i + 1);
    }
    hero_image = hero_idle[0];
}

void update_hero()
{
    switch (state)
    {
    case IDLE:
        hero_image = hero_idle[idle_idx];
        idle_idx = (idle_idx + 1) % 8;
        break;
    case WALK:
        hero_image = hero_walk[walk_idx];
        walk_idx = (walk_idx + 1) % 10;
        break;
    case JUMP:
        hero_image = hero_jump[jump_idx];
        jump_idx = (jump_idx + 1) % 3;
        if (jump_idx == 0)
        {
            state = IDLE;
        }
        break;
    }
}
void move()
{
    pic_coin_x -= 2;
    if (pic_coin_x < -50)
        pic_coin_x = SCREEN_WIDTH;
}

/*
function iDraw() is called again and again by the system.
*/

void iDraw()
{
    iClear();

    if (gameState == STATE_WELCOME)
    {
        // iPauseSound(game_start);
        iLoadImage(&img, "assets/images/credit.jpg"); // Or your image path
        iResizeImage(&img, SCREEN_WIDTH, SCREEN_HEIGHT);
        iShowLoadedImage(0, 0, &img);
    }
    else if (gameState == STATE_ABOUT_US)
    {
        iLoadImage(&img, "assets/images/about_us.jpg"); // Or your image path
        iResizeImage(&img, SCREEN_WIDTH, SCREEN_HEIGHT);
        iShowLoadedImage(0, 0, &img);
        // iPauseSound(game_start);
    }

    else if (gameState == 0)
    {
        // Show menu
        iIncreaseVolume(t, 100);
        // iPauseSound(game_start);

        iLoadImage(&img, "assets/images/background_jump.jpg");
        iResizeImage(&img, SCREEN_WIDTH, SCREEN_HEIGHT);
        iShowLoadedImage(0, 0, &img);
        iShowImage(100, 250, "assets/images/image.png");

        iSetColor(80, 200, 120);
        iFilledRectangle(70, 185, 250, 30);
        iSetColor(41, 19, 36);
        iText(80, 200, "SURVIVING BUET!!!!!", GLUT_BITMAP_9_BY_15);

        // Buttons
        drawButton(BTN_NEWGAME, BTN_NEWGAME_X, BTN_NEWGAME_Y, BTN_WIDTH, BTN_HEIGHT, "NEW GAME");
        drawButton(BTN_SAVEDGAMES, BTN_SAVED_X, BTN_SAVED_Y, BTN_WIDTH, BTN_HEIGHT, "SAVED GAMES");
        drawButton(BTN_OPTIONS, BTN_OPTIONS_X, BTN_OPTIONS_Y, BTN_WIDTH, BTN_HEIGHT, "OPTIONS");
        drawButton(BTN_EXIT, BTN_EXIT_X, BTN_EXIT_Y, BTN_WIDTH, BTN_HEIGHT, "EXIT");

        // iSetColor(26, 151, 42);
        // iFilledRectangle(BTN_SAVED_X, BTN_SAVED_Y, BTN_WIDTH, BTN_HEIGHT);
        // iSetColor(41, 19, 36);
        // iText(BTN_SAVED_X + 10, BTN_SAVED_Y + 10, "SAVED GAMES", GLUT_BITMAP_9_BY_15);

        // iSetColor(26, 151, 42);
        // iFilledRectangle(BTN_OPTIONS_X, BTN_OPTIONS_Y, BTN_WIDTH, BTN_HEIGHT);
        // iSetColor(41, 19, 36);
        // iText(BTN_OPTIONS_X + 20, BTN_OPTIONS_Y + 10, "OPTIONS", GLUT_BITMAP_9_BY_15);

        // iSetColor(26, 151, 42);
        // iFilledRectangle(BTN_EXIT_X, BTN_EXIT_Y, BTN_WIDTH, BTN_HEIGHT);
        // iSetColor(41, 19, 36);
        // iText(BTN_EXIT_X + 30, BTN_EXIT_Y + 12, "EXIT", GLUT_BITMAP_9_BY_15);
    }

    else if (gameState == 1)
    {
        // Start game
        // user name input
        iPauseSound(t);
        // iPauseSound(game_start);

        iLoadImage(&img, "assets/images/background.jpg");
        iResizeImage(&img, SCREEN_WIDTH, SCREEN_HEIGHT);
        iShowLoadedImage(0, 0, &img);
        iSetColor(255, 255, 255);
        iText(300, 300, "Game Started!", GLUT_BITMAP_9_BY_15);

        iSetColor(0, 0, 255);
        iFilledRectangle(300, 130, 300, 210);
        iLoadImage(&img, "assets/images/top_symbol_name_entering.jpg");
        iResizeImage(&img, 100, 100);
        iShowLoadedImage(400, 290, &img);
        iSetColor(255, 255, 255);
        iText(390, 250, "ENTER YOUR NAME", GLUT_BITMAP_9_BY_15);
        iSetColor(255, 255, 255);
        iFilledRectangle(330, 150, 240, 30);
        iSetColor(0, 0, 0);
        iText(345, 160, playerName, GLUT_BITMAP_HELVETICA_18);

        iSetColor(180, 180, 180);
        // iText(310, 320, "Press ENTER to continue", GLUT_BITMAP_9_BY_15);
    }
    else if (gameState == STATE_OPTIONS)
    {
        // iPauseSound(game_start);
        iLoadImage(&img, "assets/images/background.jpg");
        iResizeImage(&img, SCREEN_WIDTH, SCREEN_HEIGHT);
        iShowLoadedImage(0, 0, &img);

        iSetColor(255, 255, 255);
        iText(380, 380, "OPTIONS MENU");

        // Scoreboard Button
        iSetColor(70, 130, 180);
        iFilledRectangle(BTN_SCOREBOARD_X, BTN_SCOREBOARD_Y, BTN_SCOREBOARD_W, BTN_SCOREBOARD_H);
        iSetColor(0, 0, 0);
        iText(BTN_SCOREBOARD_X + 50, BTN_SCOREBOARD_Y + 15, "Scoreboard");

        // General Settings Button
        iSetColor(70, 130, 180);
        iFilledRectangle(BTN_GENERALSET_X, BTN_GENERALSET_Y, BTN_GENERALSET_W, BTN_GENERALSET_H);
        iSetColor(0, 0, 0);
        iText(BTN_GENERALSET_X + 30, BTN_GENERALSET_Y + 15, "General Settings");

        // Back Button
        iSetColor(255, 100, 100);
        iFilledRectangle(BTN_BACK_X, BTN_BACK_Y, BTN_BACK_W, BTN_BACK_H);
        iSetColor(0, 0, 0);
        iText(BTN_BACK_X + 70, BTN_BACK_Y + 15, "Back");
    }
    else if (gameState == STATE_SCOREBOARD)
    {
        iClear();
        // sortLeaderboard();

        iSetColor(255, 255, 255);
        iText(350, 450, "Leaderboard");

        // back button
        iSetColor(255, 100, 100);
        iFilledRectangle(BTN_SCORE_BACK_X, BTN_SCORE_BACK_Y, BTN_SCORE_BACK_W, BTN_SCORE_BACK_H);
        iSetColor(0, 0, 0);
        iText(BTN_SCORE_BACK_X + 50, BTN_SCORE_BACK_Y + 15, "Back");

        // Draw leaderboard entries

        for (int i = 0; i < MAX_SCORES; i++)
        {
            loadAndSortLeaderboardFromFile("leaderboard.txt");

            // Show only top 8 scores with term labels
            char terms[8][5] = {
                "1-1", "1-2", "2-1", "2-2",
                "3-1", "3-2", "4-1", "4-2"};

            int startY = 400;

            for (int i = 0; i < leaderboardCount && i < 8; i++)
            {
                float cg = ((float)leaderboard[i].score / 100);
                if (cg > 4.0)
                    cg = 4.0;

                char entry[100];
                sprintf(entry, "%s. %s - %.2f", terms[leaderboard[i].level], leaderboard[i].name, cg);

                iSetColor(255, 255, 0);
                iText(350, startY - i * 30, entry);
            }
        }
    }

    else if (gameState == STATE_SETTINGS)
    {
        // Background
        iLoadImage(&img, "assets/images/background.jpg");
        iResizeImage(&img, SCREEN_WIDTH, SCREEN_HEIGHT);
        iShowLoadedImage(0, 0, &img);

        // Title
        iSetColor(255, 255, 255);
        iText(330, 350, "GENERAL SETTINGS", GLUT_BITMAP_TIMES_ROMAN_24);

        // === Sound Toggle Button ===
        if (soundOn)
            iSetColor(100, 255, 100); // green for ON
        else
            iSetColor(255, 100, 100); // red for OFF

        iFilledRectangle(350, 240, 200, 40);
        iSetColor(0, 0, 0);
        iText(370, 255, soundOn ? "Sound: ON" : "Sound: OFF");

        // === Back Button ===
        iSetColor(100, 180, 255);
        iFilledRectangle(350, 180, 200, 40);
        iSetColor(0, 0, 0);
        iText(420, 195, "Back");
    }

    else if (gameState == STATE_GAME_RUNNING)
    {
        iClear();

        iSetColor(255, 255, 255);
        iText(300, 300, "Welcome, adventurer!", GLUT_BITMAP_TIMES_ROMAN_24);

        char welcomeLine[100];
        sprintf(welcomeLine, "Player: %s", playerName);
        iText(300, 260, welcomeLine, GLUT_BITMAP_HELVETICA_18);

        iText(300, 220, "Game content starts here...");
        // gameState = STATE_GAME_STARTED;
    }

    else if (gameState == STATE_GAME_STARTED)
    {
        iClear();

        // iResumeSound(game_start);
        //  score??
        if (totalLevelsPlayed > 0)
        {
            char cgpaStr[100];
            char cgpaMsg[150];
            float cgpa = 0;

            for (int i = 0; i < totalLevelsPlayed; i++)
            {
                cgpa += (float)levelScores[i] / 100.0f;
            }
            cgpa /= totalLevelsPlayed;

            sprintf(cgpaStr, "CGPA: %.2f", cgpa);
            sprintf(cgpaMsg, "Your CGPA is %.2f", cgpa);

            iSetColor(255, 255, 0);
            iText(20, SCREEN_HEIGHT - 80, cgpaStr, GLUT_BITMAP_HELVETICA_18);

            iSetColor(255, 100, 100);
            iText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 50, cgpaMsg, GLUT_BITMAP_TIMES_ROMAN_24);
        }

        //   background
        iShowLoadedImage(0, 0, &levelBackgrounds[currentLevel]);
        //  platforms
        drawPlatformFloor();
        pic_x = 100;
        // pic_y = 85;
        iShowImage(pic_x, pic_y, hero_image);
        // gravity
        if (isJumping)
        {
            velocityY += gravity;
            pic_y += velocityY;

            if (velocityX > 0)
            {
                worldOffsetX += velocityX;
            }
            else if (velocityX < 0)
            {
                worldOffsetX += velocityX;
                if (worldOffsetX < 0)
                    worldOffsetX = 0;
            }

            if (pic_y <= groundY)
            {
                pic_y = groundY;
                velocityY = 0;
                velocityX = 0;
                isJumping = false;
                state = IDLE;
            }
        }

        // Level-Term Box
        char term_msg[30];

        iSetColor(255, 255, 100);
        iFilledRectangle((SCREEN_WIDTH / 2) - 10, SCREEN_HEIGHT - 60, 120, 40);
        iSetColor(0, 0, 0);
        sprintf(term_msg, "Level : %s", terms[currentLevel]);
        iText((SCREEN_WIDTH / 2), SCREEN_HEIGHT - 48, term_msg, GLUT_BITMAP_HELVETICA_18);

        // Draw Coins
        for (int i = 0; i < MAX_COINS; i++)
        {
            if (!coin_collected[i])
            {
                int screenX = coin_positions[i] - worldOffsetX;
                if (screenX >= -30 && screenX <= SCREEN_WIDTH) // Only draw visible ones
                {
                    iShowLoadedImage(screenX, 100, &coinFrames[currentCoinFrame]);
                }
            }
        }

        for (int i = 0; i < MAX_COINS; i++)
        {
            if (!coin_collected[i])
            {
                int screenX = coin_positions[i] - worldOffsetX;
                int coinWidth = 30, coinHeight = 30;
                int heroWidth = 40, heroHeight = 60;
                // Bounding Box Check
                if (screenX < pic_x + heroWidth &&
                    screenX + coinWidth > pic_x &&
                    100 < pic_y + heroHeight &&
                    100 + coinHeight > pic_y)
                {
                    coin_collected[i] = true;
                    playerScore += 10;
                    // iPauseSound(game_start);
                    if (soundOn)
                    {
                        coinChannel = iPlaySound("assets/sounds/coin_collision.wav", false);
                        iIncreaseVolume(coinChannel, 100);
                    }

                    // iPauseSound(coin_sound);
                }
            }
        }
        // iPauseSound(coin_sound);
        //   Draw labs
        for (int i = 0; i < BASE_LAB_COUNT; i++)
        {
            if (!lab_collected[i])
            {
                int screenX = lab_positions[i] - worldOffsetX;
                if (screenX >= -30 && screenX <= SCREEN_WIDTH)
                {

                    iShowLoadedImage(screenX, 200, &labFrames[currentLabFrame]);
                }
            }
        }
        for (int i = 0; i < MAX_LAB; i++)
        {
            if (!lab_collected[i])
            {
                int screenX = lab_positions[i] - worldOffsetX;
                int labWidth = 30, labHeight = 30;
                int heroWidth = 40, heroHeight = 60;
                // Bounding Box Check
                if (screenX < pic_x + heroWidth &&
                    screenX + labWidth > pic_x &&
                    200 < pic_y + heroHeight &&
                    200 + labHeight > pic_y)
                {
                    lab_collected[i] = true;
                    if (playerScore >= 20)
                    {
                        playerScore -= 20;
                        if (soundOn && loseChannel == -1)
                        {
                            loseChannel = iPlaySound("assets/sounds/losing_sound.wav", false);
                        }
                    }
                }
            }
            if (soundOn && bhag_bhagChannel == -1)
            {
                bhag_bhagChannel = iPlaySound("assets/sounds/bhag_bhag.wav", true);
            }
        }

        // labtest
        char scoreText[50];
        sprintf(scoreText, "Score: %d", playerScore);
        // Draw yellow rectangle behind score
        int scoreX = SCREEN_WIDTH - 130;
        int scoreY = SCREEN_HEIGHT - 50;
        int scoreW = 120;
        int scoreH = 30;

        iSetColor(255, 255, 100);
        iFilledRectangle(scoreX, scoreY, scoreW, scoreH);

        // score
        iSetColor(0, 0, 0);
        iText(scoreX + 10, scoreY + 10, scoreText, GLUT_BITMAP_HELVETICA_18);

        // GRAvity
        if (isJumping)
        {
            velocityY += gravity;
            pic_y += velocityY;
            worldOffsetX += velocityX;

            if (pic_y <= groundY)
            {
                pic_y = groundY;
                velocityY = 0;
                velocityX = 0;
                isJumping = false;
                state = IDLE;
            }
        }
        //  obstacles
        // iSetColor(255, 0, 0); // red color
        for (int i = 0; i < MAX_OBSTACLES; i++)
        {
            if (obstacle_visible[i])
            {
                int screenX = obstacle_positions[i] - worldOffsetX;
                if (screenX >= playerX && screenX <= SCREEN_WIDTH + obstacle_width)
                {
                    iShowLoadedImage(screenX, groundY, &rotatingObstacleFrames[currentObstacleFrame]);
                }
            }
        }

        // Obstacle e laga
        int heroWidth = 40;
        int heroHeight = 60;

        for (int i = 0; i < MAX_OBSTACLES; i++)
        {
            if (obstacle_visible[i])
            {
                int screenX = obstacle_positions[i] - worldOffsetX;

                int obstacleTop = groundY + obstacle_height;
                int heroTop = pic_y + heroHeight;

                if (screenX < pic_x + heroWidth &&
                    screenX + obstacle_width > pic_x &&
                    pic_y < obstacleTop &&
                    heroTop > groundY)
                {
                    printf("Hit obstacle! Score -20\n");
                    playerScore -= 20;
                    point_lose = iPlaySound("assets/sounds/losing_sound.wav", false);
                    iIncreaseVolume(point_lose, 1000);
                    if (playerScore <= 0)
                    {
                        playerScore = 0;

                        if (totalLevelsPlayed < MAX_LEVELS)
                        {
                            levelScores[totalLevelsPlayed] = playerScore;
                            totalLevelsPlayed++;
                        }

                        showCGPA = true;
                        cgpaDisplayTimer = 150;
                        gameState = STATE_GAME_OVER;

                        break;
                    }

                    obstacle_visible[i] = false;
                    appendToLeaderboard(playerName, playerScore, currentLevel);

                    // updateLeaderboard(playerName, playerScore, currentLevel);
                }
            }
        }

        // coinAcheKi?
        bool allCoinsBehind = true;
        int heroWorldX = pic_x + worldOffsetX;

        for (int i = 0; i < MAX_COINS; i++)
        {
            if (!coin_collected[i] && coin_positions[i] > heroWorldX + 100)
            {
                allCoinsBehind = false;
                break;
            }
        }

        if (allCoinsBehind && !tfVisible)
        {
            tfVisible = true;
            tfX = heroWorldX + 500;
        }

        // tf dao

        if (tfVisible)
        {
            int tfScreenX = tfX - worldOffsetX;
            iShowLoadedImage(tfScreenX, groundY + 10, &tfFrames[currentTfFrame]);

            // Collision with hero
            if (tfScreenX < pic_x + 40 && tfScreenX + 40 > pic_x && pic_y <= groundY + 40)
            {
                // COLLISION — switch to next level
                playerScore += 50;
                if (playerScore > 400)
                    playerScore = 400;
                tfVisible = false;

                gameState = STATE_LEVEL_COMPLETE;
                cgpaDisplayTimer = 100; // ⬅ Set timer to display CGPA

                if (totalLevelsPlayed < MAX_LEVELS)
                {
                    levelScores[totalLevelsPlayed] = playerScore;
                }

                appendToLeaderboard(playerName, playerScore, currentLevel);
                currentLevel++;
                totalLevelsPlayed++;
                if (currentLevel >= MAX_LEVELS)
                {
                    currentLevel = MAX_LEVELS - 1;
                }

                playerScore = 0;
                speedMultiplier *= 1.1f;

                initializeCoins();
                initializeObstacles();
                initializeLabTests();
                initializeTF();

                worldOffsetX = 0;
            }
        }

        // d press idle issue
        if (!(GetAsyncKeyState('A') & 0x8000) && !(GetAsyncKeyState('D') & 0x8000) && !isJumping)
        {
            state = IDLE;
        }

        // Draw pause button (top-right)
        iSetColor(220, 50, 50);
        iFilledRectangle(PAUSE_BTN_X, PAUSE_BTN_Y, PAUSE_BTN_W, PAUSE_BTN_H);
        iSetColor(255, 255, 255);
        iText(PAUSE_BTN_X + 20, PAUSE_BTN_Y + 10, "| |", GLUT_BITMAP_HELVETICA_18);
    }
    else if (gameState == STATE_PAUSED)
    {
        iClear();
        iLoadImage(&img, "assets/images/background.jpg");
        iResizeImage(&img, SCREEN_WIDTH, SCREEN_HEIGHT);
        iShowLoadedImage(0, 0, &img);
        // iSetColor(0, 0, 0); // jhapsha dark
        // iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        iSetColor(255, 0, 0);
        iFilledRectangle(350, 300, 200, 50); // Continue
        iFilledRectangle(350, 220, 200, 50); // Save and Exit

        iSetColor(0, 100, 255); // Options button
        iFilledRectangle(350, 140, 200, 50);

        iSetColor(255, 255, 255);
        iText(410, 320, "Continue");
        iText(390, 240, "Save and Exit");
        iText(420, 160, "Options");

        // iSetColor(255, 255, 255);
        // iText(400, 320, "Continue");
        // iText(370, 240, "Save and Exit");
    }

    else if (gameState == STATE_LEVEL_COMPLETE)
    {
        iClear();
        iSetColor(255, 255, 255);
        if (cgpaDisplayTimer > 0)
        {
            float cgpa = 0;
            for (int i = 0; i < totalLevelsPlayed; i++)
            {
                cgpa += (float)levelScores[i] / 100.0f;
            }
            if (totalLevelsPlayed > 0)
                cgpa /= totalLevelsPlayed;

            if (cgpa > 4.0f)
                cgpa = 4.0f;

            char cgpaMsg[100];
            sprintf(cgpaMsg, "YOUR CGPA is %.2f", cgpa);
            iSetColor(255, 255, 255);
            iText(320, 200, cgpaMsg, GLUT_BITMAP_TIMES_ROMAN_24);
        }

        if (currentLevel >= MAX_LEVELS)
        {
            // FINAL LEVEL MESSAGE
            iText(250, 320, "🎓 You Have Just Survived the BUET Life!", GLUT_BITMAP_TIMES_ROMAN_24);

            float finalCgpa = 0;
            for (int i = 0; i < totalLevelsPlayed; i++)
            {
                finalCgpa += (float)levelScores[i] / 100.0f;
            }
            if (totalLevelsPlayed > 0)
                finalCgpa /= totalLevelsPlayed;

            char finalMsg[100];
            sprintf(finalMsg, " Final CGPA: %.2f", finalCgpa);
            iSetColor(255, 215, 0); // gold text
            iText(320, 270, finalMsg, GLUT_BITMAP_HELVETICA_18);
        }
        else
        {
            // REGULAR LEVEL COMPLETE MESSAGE
            iText(370, 320, "🎉 Congratulations!", GLUT_BITMAP_TIMES_ROMAN_24);
            iText(350, 280, "You have completed this level.", GLUT_BITMAP_HELVETICA_18);

            float cgpa = 0;
            for (int i = 0; i < totalLevelsPlayed; i++)
            {
                cgpa += (float)levelScores[i] / 100.0f;
            }
            if (totalLevelsPlayed > 0)
                cgpa /= totalLevelsPlayed;

            // char cgpaMsg[100];
            //  sprintf(cgpaMsg, "📘 Your CGPA so far: %.2f", cgpa);

            iSetColor(255, 200, 0);
            // iText(340, 230, cgpaMsg, GLUT_BITMAP_HELVETICA_18);
        }
    }

    else if (gameState == STATE_GAME_OVER)
    {
        iClear();
        iSetColor(255, 0, 0);
        iText(380, 300, "GAME OVER!", GLUT_BITMAP_TIMES_ROMAN_24);

        float cgpa = 0;
        for (int i = 0; i < totalLevelsPlayed; i++)
        {
            cgpa += (float)levelScores[i] / 100.0f;
        }
        if (totalLevelsPlayed > 0)
            cgpa /= totalLevelsPlayed;

        char cgpaMsg[100];
        sprintf(cgpaMsg, "Your CGPA: %.2f", cgpa);

        iSetColor(255, 200, 0);
        iText(380, 250, cgpaMsg, GLUT_BITMAP_HELVETICA_18);
    }

    else if (gameState == STATE_SAVED_GAMES)
    {
        iShowLoadedImage(0, 0, &saved_games_background); // background
        iSetColor(0, 0, 0);
        iText(370, 400, "SAVED GAMES");

        for (int i = 0; i < savedGameCount; i++)
        {
            int btnY = SAVED_BTN_Y_START - i * SAVED_BTN_GAP;

            // Draw button background
            iSetColor(100, 200, 250); // light blue
            iFilledRectangle(SAVED_BTN_X, btnY, SAVED_BTN_WIDTH, SAVED_BTN_HEIGHT);

            // Border
            iSetColor(0, 100, 200);
            iRectangle(SAVED_BTN_X, btnY, SAVED_BTN_WIDTH, SAVED_BTN_HEIGHT);

            // Text
            char line[100];
            char terms[8][5] = {
                "1-1", "1-2", "2-1", "2-2",
                "3-1", "3-2", "4-1", "4-2"};
            sprintf(line, "%s - Level %s", savedPlayerNames[i], terms[i]);
            iSetColor(0, 0, 0);
            iText(SAVED_BTN_X + 20, btnY + 12, line);
        }

        // Back Button
        iSetColor(255, 100, 100);
        iFilledRectangle(370, 60, 150, 40);
        iSetColor(0, 0, 0);
        iText(420, 80, "Back");

        for (int i = 0; i < savedGameCount; i++)
        {
            int btnY = SAVED_BTN_Y_START - i * SAVED_BTN_GAP;

            if (i == hoveredSavedIndex)
                iSetColor(200, 250, 255); // Hover color
            else
                iSetColor(100, 200, 250); // Normal color

            iFilledRectangle(SAVED_BTN_X, btnY, SAVED_BTN_WIDTH, SAVED_BTN_HEIGHT);
            iSetColor(0, 100, 200);
            iRectangle(SAVED_BTN_X, btnY, SAVED_BTN_WIDTH, SAVED_BTN_HEIGHT);
            char line[100];
            char terms[8][5] = {
                "1-1", "1-2", "2-1", "2-2",
                "3-1", "3-2", "4-1", "4-2"};
            sprintf(line, "%s - Level %s", savedPlayerNames[i], terms[i]);
            iSetColor(0, 0, 0);
            iText(SAVED_BTN_X + 20, btnY + 12, line);
        }
    }
}

void iMouse(int button, int state, int mx, int my)
{

    if (gameState == STATE_MENU)
    {
        // Check if mouse is inside New Game button
        if (mx >= BTN_NEWGAME_X && mx <= BTN_NEWGAME_X + BTN_WIDTH &&
            my >= BTN_NEWGAME_Y && my <= BTN_NEWGAME_Y + BTN_HEIGHT)
        {
            printf("New Game clicked\n");
            gameState = 1;
        }
        // Saved Games
        if (mx >= BTN_SAVED_X && mx <= BTN_SAVED_X + BTN_WIDTH &&
            my >= BTN_SAVED_Y && my <= BTN_SAVED_Y + BTN_HEIGHT)
        {
            printf("Saved Games clicked\n");
            gameState = STATE_SAVED_GAMES; // <--- Fix here
        }

        // Options
        if (mx >= BTN_OPTIONS_X && mx <= BTN_OPTIONS_X + BTN_WIDTH &&
            my >= BTN_OPTIONS_Y && my <= BTN_OPTIONS_Y + BTN_HEIGHT)
        {
            printf("Options clicked from Main Menu\n");
            optionsReturnState = STATE_MENU;

            gameState = STATE_OPTIONS;
        }
        if (mx >= BTN_EXIT_X && mx <= BTN_EXIT_X + BTN_WIDTH &&
            my >= BTN_EXIT_Y && my <= BTN_EXIT_Y + BTN_HEIGHT)
        {
            printf("Exit clicked\n");
            exit(0);
        }
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // Pause Game
        if (gameState == STATE_GAME_STARTED)
        {
            if (mx >= PAUSE_BTN_X && mx <= PAUSE_BTN_X + PAUSE_BTN_W &&
                my >= PAUSE_BTN_Y && my <= PAUSE_BTN_Y + PAUSE_BTN_H)
            {
                printf("Pause button clicked\n");
                gameState = STATE_PAUSED;
            }
        }

        else if (gameState == STATE_SETTINGS)
        {
            printf("We are in the state setting\n");
            // if (mx >= 350 && mx <= 550 && my >= 300 && my <= 340)
            // {

            // }

            if (mx >= 350 && mx <= 550 && my >= 240 && my <= 280)
            {
                soundOn = !soundOn;

                if (soundOn)
                {
                    printf("music on clicked");
                    // iResumeSound(game_start);
                    iResumeSound(t); // or use your bg music channel
                }
                else
                {
                    printf("music off clicked");
                    // iPauseSound(game_start);
                    iPauseSound(t);
                }
                return;
            }
            // Check Back Button in General Settings
            if (mx >= 350 && mx <= 550 &&
                my >= 180 && my <= 220)
            {
                printf("Back from General Settings to Options\n");
                gameState = STATE_OPTIONS;
                return;
            }
        }

        else if (gameState == STATE_SAVED_GAMES)
        {
            // Check each saved button
            for (int i = 0; i < savedGameCount; i++)
            {
                int btnY = SAVED_BTN_Y_START - i * SAVED_BTN_GAP;

                if (mx >= SAVED_BTN_X && mx <= SAVED_BTN_X + SAVED_BTN_WIDTH &&
                    my >= btnY && my <= btnY + SAVED_BTN_HEIGHT)
                {
                    // Load that saved game

                    strcpy(playerName, savedPlayerNames[i]);
                    nameLength = strlen(playerName);
                    currentLevel = savedLevels[i];
                    gameState = STATE_GAME_STARTED;

                    playerScore = savedScores[i];
                    worldOffsetX = savedPositions[i];
                    appendToLeaderboard(playerName, playerScore, currentLevel);
                    // updateLeaderboard(playerName, playerScore, currentLevel);

                    speedMultiplier = 1.0f + 0.1f * currentLevel;
                    initializeCoins();
                    initializeObstacles();
                    initializeLabTests();

                    currentSaveSlot = i; // <--- Track which save slot was loaded

                    printf("Loaded saved game: %s at level %d\n", playerName, currentLevel + 1);
                    return;
                }
            }

            // Back button click
            if (mx >= 370 && mx <= 520 && my >= 60 && my <= 100)
            {
                gameState = STATE_MENU;
                if ((soundOn && gameStartChannel == -1))
                iResumeSound(t);
            }
        }

        else if (gameState == STATE_OPTIONS)
        {
            if (mx >= BTN_SCOREBOARD_X && mx <= BTN_SCOREBOARD_X + BTN_SCOREBOARD_W &&
                my >= BTN_SCOREBOARD_Y && my <= BTN_SCOREBOARD_Y + BTN_SCOREBOARD_H)
            {
                printf("Scoreboard clicked\n");
                gameState = STATE_SCOREBOARD;
                return; // optional early return to avoid multiple button triggers
            }

            if (mx >= BTN_GENERALSET_X && mx <= BTN_GENERALSET_X + BTN_GENERALSET_W &&
                my >= BTN_GENERALSET_Y && my <= BTN_GENERALSET_Y + BTN_GENERALSET_H)
            {
                printf("General Settings clicked\n");
                gameState = STATE_SETTINGS;

                return;
            }
            if (mx >= BTN_BACK_X && mx <= BTN_BACK_X + BTN_BACK_W &&
                my >= BTN_BACK_Y && my <= BTN_BACK_Y + BTN_BACK_H)
            {
                printf("Back to Options from Scoreboard\n");
                gameState = STATE_MENU;
                return;
            }

            // You can also handle other buttons like toggles here (optional)
        }

        else if (gameState == STATE_SCOREBOARD)
        {
            if (mx >= BTN_SCORE_BACK_X && mx <= BTN_SCORE_BACK_X + BTN_SCORE_BACK_W &&
                my >= BTN_SCORE_BACK_Y && my <= BTN_SCORE_BACK_Y + BTN_SCORE_BACK_H)
            {
                printf("Back to Options from Scoreboard\n");
                gameState = STATE_OPTIONS;
                return;
            }
        }
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
    }
    if (gameState == STATE_PAUSED)
    {
        if (mx >= 350 && mx <= 550)
        {
            if (my >= 300 && my <= 350)
            {
                // Continue
                gameState = STATE_GAME_STARTED;
            }
            else if (my >= 220 && my <= 270)
            {
                // Save full state
                if (currentSaveSlot >= 0 && currentSaveSlot < savedGameCount)
                {
                    // Overwrite the existing slot
                    strcpy(savedPlayerNames[currentSaveSlot], playerName);
                    savedLevels[currentSaveSlot] = currentLevel;
                    savedScores[currentSaveSlot] = playerScore;
                    savedPositions[currentSaveSlot] = worldOffsetX;
                }
                else
                {
                    // Save at top of list (shift older ones down)
                    for (int i = MAX_SAVED_GAMES - 1; i > 0; i--)
                    {
                        strcpy(savedPlayerNames[i], savedPlayerNames[i - 1]);
                        savedLevels[i] = savedLevels[i - 1];
                        savedScores[i] = savedScores[i - 1];
                        savedPositions[i] = savedPositions[i - 1];
                    }

                    strcpy(savedPlayerNames[0], playerName);
                    savedLevels[0] = currentLevel;
                    savedScores[0] = playerScore;
                    savedPositions[0] = worldOffsetX;

                    if (savedGameCount < MAX_SAVED_GAMES)
                        savedGameCount++;
                }

                if (savedGameCount < MAX_SAVED_GAMES)
                    savedGameCount++;

                saveSavedGamesToFile();

                gameState = STATE_MENU;
                iResumeSound(t);
            }
            else if (my >= 140 && my <= 190)
            {
                printf("Options clicked from Pause Menu\n");
                optionsReturnState = STATE_PAUSED;
                gameState = STATE_OPTIONS;
            }
        }
    }
    if (gameState == STATE_PAUSED)
    {
        if (mx >= 350 && mx <= 550)
        {
            if (my >= 300 && my <= 350)
            {
                // Continue
                gameState = STATE_GAME_STARTED;
            }
            else if (my >= 220 && my <= 270)
            {

                for (int i = MAX_SAVED_GAMES - 1; i > 0; i--)
                {
                    strcpy(savedPlayerNames[i], savedPlayerNames[i - 1]);
                    savedLevels[i] = savedLevels[i - 1];
                }

                strcpy(savedPlayerNames[0], playerName);
                savedLevels[0] = currentLevel;
                if (savedGameCount < MAX_SAVED_GAMES)
                    savedGameCount++;

                saveSavedGamesToFile(); // <- persist to file

                gameState = STATE_MENU;
                iResumeSound(t);
            }
        }
    }
}

void iMouseMove(int mx, int my)
{
    for (int i = 0; i < MAX_BUTTONS; i++)
        buttonHovering[i] = false;

    if (gameState == STATE_MENU)
    {
        if (mx >= BTN_NEWGAME_X && mx <= BTN_NEWGAME_X + BTN_WIDTH &&
            my >= BTN_NEWGAME_Y && my <= BTN_NEWGAME_Y + BTN_HEIGHT)
            buttonHovering[BTN_NEWGAME] = true;

        if (mx >= BTN_SAVED_X && mx <= BTN_SAVED_X + BTN_WIDTH &&
            my >= BTN_SAVED_Y && my <= BTN_SAVED_Y + BTN_HEIGHT)
            buttonHovering[BTN_SAVEDGAMES] = true;

        if (mx >= BTN_OPTIONS_X && mx <= BTN_OPTIONS_X + BTN_WIDTH &&
            my >= BTN_OPTIONS_Y && my <= BTN_OPTIONS_Y + BTN_HEIGHT)
            buttonHovering[BTN_OPTIONS] = true;

        if (mx >= BTN_EXIT_X && mx <= BTN_EXIT_X + BTN_WIDTH &&
            my >= BTN_EXIT_Y && my <= BTN_EXIT_Y + BTN_HEIGHT)
            buttonHovering[BTN_EXIT] = true;
    }
}

void iMouseDrag(int mx, int my)
{
    // place your codes here
}

void iMouseWheel(int dir, int mx, int my)
{
    // place your code here
}

/*
function iKeyboard() is called whenever the user hits a key in keyboard.
key- holds the ASCII value of the key pressed.
*/
void iKeyboard(unsigned char key)
{
    if (gameState == STATE_WELCOME)
    {
        if (key == '\r')
        {
            printf("Enter Pressed\n");
            gameState = STATE_ABOUT_US;
        }
    }
    else if (gameState == STATE_ABOUT_US)
    {
        if (key == '\r')
        {
            printf("Enter Pressed\n");
            gameState = STATE_MENU;
        }
    }
    if (gameState == STATE_PLAYING)
    {
        if (key == '\r')
        {
            printf("Player name entered: %s\n", playerName);
            gameState = STATE_GAME_STARTED;
            initializeCoins();
            initializeLabTests();
            initializeObstacles();
            currentLevel = 0;
            worldOffsetX = 0;
            playerScore = 0;
        }
        else if (key == '\b')
        { // BACKSPACE
            if (nameLength > 0)
            {
                nameLength--;
                playerName[nameLength] = '\0';
            }
        }
        else if (key >= 32 && key <= 126 && nameLength < 99)
        {
            playerName[nameLength] = key;
            nameLength++;
            playerName[nameLength] = '\0';
        }
    }
    if (gameState == STATE_GAME_RUNNING)
    {
        if (key == '\r')
        { // ENTER key
            printf("Player name entered: %s\n", playerName);
            gameState = STATE_GAME_STARTED; //  Switch to next screen
        }
    }
    if (gameState == STATE_GAME_STARTED)
    {
        if (key == GLUT_KEY_END)
        {
            exit(0);
        }
        if (key == 27)
        { // 27 is ASCII for ESC
            gameState = STATE_PAUSED;
        }

        if (key == 'd')
        {
            worldOffsetX += (int)(baseSpeed * speedMultiplier); // faster
            state = WALK;
        }
        if (key == 'a')
        {
            worldOffsetX -= (int)(baseSpeed * speedMultiplier); // fastt speed
            state = WALK;
            if (worldOffsetX < 0)
                worldOffsetX = 0;
        }

        if (key == 'w')
        {
            if (!isJumping)
            {
                velocityY = 12;
                isJumping = true;

                if (GetAsyncKeyState('D') & 0x8000)
                {
                    velocityX = (6 + currentLevel) * speedMultiplier;
                    state = JUMP;
                }
                else if (GetAsyncKeyState('A') & 0x8000)
                {
                    velocityX = -6 * speedMultiplier;
                    state = JUMP;
                }
                else
                {
                    velocityX = 0;
                    state = JUMP;
                }

                jump_idx = 0;
            }
        }

        if (key == 'd' && key == 'w')
        {
            velocityX = 6; // give forward motion during jump
        }

        if (key == 's')
        {
        }
    }
}

void iSpecialKeyboard(unsigned char key)
{
    switch (key)
    {
    case GLUT_KEY_END:

        break;

    default:
        break;
    }
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    srand(time(0));
    // loadLeaderboard();

    for (int i = 0; i <= MAX_LEVELS; i++)
    {
        char path[100];
        sprintf(path, "assets/images/Level_background/level%03d.jfif", i);
        iLoadImage(&levelBackgrounds[i], path);
        iResizeImage(&levelBackgrounds[i], SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    // Load general purpose background (for menus etc.)
    iLoadImage(&img, "assets/images/background.jpg");
    iResizeImage(&img, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Load coin frames
    for (int i = 0; i < 6; i++)
    {
        char filename[100];
        sprintf(filename, "assets/images/Collectable_Object/Coin_%02d.png", i + 1);
        iLoadImage(&coinFrames[i], filename);
        iResizeImage(&coinFrames[i], 30, 30);
    }

    for (int i = 1; i < MAX_LEVELS; i++)
    {
        for (int j = 0; j < BASE_COIN_COUNT; j++)
        {
            fixedCoinPositions[i][j] = fixedCoinPositions[0][j] + i * 300;
        }
    }

    // lab
    for (int i = 0; i < 8; i++)
    {
        char filename[100];
        sprintf(filename, "assets/images/beaker/beaker%03d.png", i);
        iLoadImage(&labFrames[i], filename);
        iResizeImage(&labFrames[i], 30, 30);
    }

    iSetTimer(150, []()
              { currentLabFrame = (currentLabFrame + 1) % 8; });

    // the platform

    iLoadImage(&platformTiles[0], "assets/images/Frame_137.png");
    iResizeImage(&platformTiles[0], TILE_WIDTH, 90);

    // level 1
    populate_hero_images();
    iSetTimer(100, update_hero);
    for (int i = 0; i < 8; i++)
    {
        char path[100];
        sprintf(path, "assets/images/tf/tf%03d.png", i);
        iLoadImage(&tfFrames[i], path);
        iResizeImage(&tfFrames[i], 40, 40);
    }
    iSetTimer(100, []()
              { currentTfFrame = (currentTfFrame + 1) % 8; });
    iSetTimer(100, []()
              { currentCoinFrame = (currentCoinFrame + 1) % 6; });

    //  obstacles
    for (int i = 1; i < MAX_LEVELS; i++)
    {
        numObstacles[i] = numObstacles[0];

        for (int j = 0; j < numObstacles[0]; j++)
        {
            fixedObstaclePositions[i][j] = fixedObstaclePositions[0][j] + i * 300;
        }
    }

    for (int i = 1; i < MAX_LEVELS; i++)
    {
        for (int j = 0; j < BASE_LAB_COUNT; j++)
        {
            fixedLabPositions[i][j] = fixedLabPositions[0][j] + i * 300;
        }
    }

    initializeObstacles();
    //  rotate obstacle
    for (int i = 0; i < OBSTACLE_FRAMES; i++)
    {
        char path[300];
        sprintf(path, "assets/images/CT_sprite/tile00%d.png", i);
        iLoadImage(&rotatingObstacleFrames[i], path);
        iResizeImage(&rotatingObstacleFrames[i], obstacle_width, obstacle_height);
    }
    int speedDelay = 120 - currentLevel * 10;
    if (speedDelay < 40)
        speedDelay = 40;

    iSetTimer(speedDelay, []()
              { currentObstacleFrame = (currentObstacleFrame + 1) % OBSTACLE_FRAMES; });

    iSetTimer(5000, []()
              {
                if (gameState == STATE_LEVEL_COMPLETE && cgpaDisplayTimer > 0)
                {
                    cgpaDisplayTimer--;
                    if (cgpaDisplayTimer == 0)
                    {
                        gameState = STATE_GAME_STARTED;
                    }
                } });

    loadSavedGamesFromFile();
    // return time
    iSetTimer(10000, returntoMenu);
    // Saved games
    iLoadImage(&saved_games_background, "assets/images/background.jpg"); // already done, reused here
    iResizeImage(&saved_games_background, SCREEN_WIDTH, SCREEN_HEIGHT);

    iSetTimer(100, []()
              {
    if (gameState == STATE_LEVEL_COMPLETE && cgpaDisplayTimer > 0)
    {
        cgpaDisplayTimer--;
        if (cgpaDisplayTimer == 0)
        {
            gameState = STATE_GAME_STARTED;
        }
    } });

    iSetTimer(8, []()
              {
    for (int i = 0; i < MAX_BUTTONS; i++) {
        if (buttonHovering[i] && buttonHoverAlpha[i] < 1.0f)
            buttonHoverAlpha[i] += 0.1f;
        else if (!buttonHovering[i] && buttonHoverAlpha[i] > 0.0f)
            buttonHoverAlpha[i] -= 0.1f;

        // Clamp values
        if (buttonHoverAlpha[i] > 1.0f) buttonHoverAlpha[i] = 1.0f;
        if (buttonHoverAlpha[i] < 0.0f) buttonHoverAlpha[i] = 0.0f;
    } });

    // Initialize window
    welcomeTimer = 300; // 10 seconds
    gameState = STATE_WELCOME;

    iInitializeSound();

    // game_start = iPlaySound("assets/sounds/bhag_bhag.wav", true);

    t = iPlaySound("assets/sounds/Opening_Titles.wav", true);
    iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Surviving BUET");

    return 0;
}