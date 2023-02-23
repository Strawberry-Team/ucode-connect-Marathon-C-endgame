#ifndef UCODE_ENDGAME_MAIN_H
#define UCODE_ENDGAME_MAIN_H

#include "../resources/framework/raylib/src/raylib.h"
#include "../resources/framework/raylib/src/raymath.h"
#include <math.h>

typedef struct Player
{
    
    Vector2 position;
    float speed;
    bool canJump;
    int jumpCounter;
} Player;

typedef struct Move
{
    bool moving;
    int speed;
} Move;

typedef struct EnvItem
{
    Texture2D photo;
    Rectangle rect;
    Move if_dynamic;
    int blocking;
    bool destroy;
    int destroy_time;
    Color color;
} EnvItem;

typedef enum {
    INCREASE_LAVA_SPEED = 0
} EventType;

typedef struct EventCheckPoint {
    Rectangle rect;
    Color color;
    bool isActive;
    EventType eventType;
} EventCheckPoint;

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta, bool *destroy, int *index);
void UpdateBricks(EnvItem *envItems, int envItemsLength, bool pause);
void FixCameraCenterInsideMap(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);

#endif
