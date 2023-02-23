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
    TRIGGER_TYPE_INCREASE_LAVA_SPEED = 2
} TriggerType;

typedef struct Trigger {
    Rectangle rect;
    Color color;
    bool isActive;
    TriggerType eventType;
} Trigger;

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta, bool *destroy, int *index);
void UpdateBricks(EnvItem *envItems, int envItemsLength, bool pause);
void FixCameraCenterInsideMap(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);

#endif
