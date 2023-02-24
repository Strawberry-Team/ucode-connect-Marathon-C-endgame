#ifndef UCODE_ENDGAME_MAIN_H
#define UCODE_ENDGAME_MAIN_H

#include "../resources/framework/raylib/src/raylib.h"
#include "../resources/framework/raylib/src/raymath.h"
#include <math.h>

typedef enum {
    BUTTON_STATE_NORMAL,
    BUTTON_STATE_MOUSE_HOVER,
    BUTTON_STATE_PRESSED
} ButtonState;

typedef struct Button {
    Texture2D texture2D;
    Rectangle rect;
    Rectangle textureRect;

    float frameHeight;

    ButtonState buttonState;
    bool isAction;
} Button;


typedef enum GameScreen {
    GAME_SCREEN_START = 0,
    GAME_SCREEN_GAMEPLAY,
    GAME_SCREEN_ENDING,
    GAME_SCREEN_WIN,
    GAME_SCREEN_EXIT
} GameScreen;

typedef enum {
    PLAYER_STATUS_MOVE_LEFT = 0,
    PLAYER_STATUS_MOVE_RIGHT = 1,
    PLAYER_STATUS_MOVE_UP = 2,
    PLAYER_STATUS_MOVE_DOWN = 3,
    PLAYER_STATUS_WAIT = 4
} PlayerStatus;

typedef struct Player {
    Vector2 position;
    Texture2D texture2D;
    Rectangle frameRect;

    unsigned int frames;
    unsigned int frameDelay;
    unsigned int frameDelayCounter;
    unsigned int frameIndex;

    float speed;
    bool canJump;
    int jumpCounter;
    bool canMove;
    PlayerStatus playerStatus;
} Player;

typedef struct Lava {
    Image animatedImage;
    Texture2D texture2D;
    Rectangle rect;

    int frame;
    int frameDelay; // Frame delay to switch between animation frames
    unsigned int nextFrameDataOffset;
    int currentAnimFrameLava; // Current animation frame to load and draw
    int animationFrames;

    float speed;
    float speedMultiplier;
} Lava;

typedef struct Move {
    bool moving;
    int speed;
} Move;

typedef struct EnvItem {
    Texture2D photo;
    Rectangle rect;
    Move if_dynamic;
    int blocking;
    bool destroy;
    int destroy_time;
    Color color;
} EnvItem;

typedef enum {
    TRIGGER_TYPE_TAKE_TREASURE = 0,
    TRIGGER_TYPE_START_LAVA = 1,
    TRIGGER_TYPE_INCREASE_LAVA_SPEED = 2,
    TRIGGER_TYPE_LAVA_SHUTDOWN = 3,
    TRIGGER_TYPE_END_FLAG = 4
} TriggerType;

typedef struct Trigger {
    Rectangle rect;
    Color color;
    bool isActive;
    TriggerType eventType;
} Trigger;

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta, bool *destroy, int *index);
void UpdateBricks(EnvItem *envItems, int envItemsLength, bool pause);
void UpdateBoards(Player *player, EnvItem *board, int boardLength, int delta);
void UpdateLava(Lava *lava, float delta);
void FixCameraCenterInsideMap(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void InitGame();
void UpdatePlayersEndWin(Texture2D *players);
void WorkMusic(Music temp_music);
void ChangeMusic(Music music);

#endif
