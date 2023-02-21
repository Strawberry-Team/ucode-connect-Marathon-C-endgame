
#include "../inc/main.h"
#define G 400
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f
#define PLAYER_JUMP_LIMIT 2

const int screenWidth = 600;
const int screenHeight = 800;

typedef struct Player {
    Vector2 position;
    float speed;
    bool canJump;
    int jumpCounter;
} Player;

typedef struct EnvItem
{
    Rectangle rect;
    int blocking;
    bool moving;
    bool destroy;
    Color color;
} EnvItem;

//----------------------------------------------------------------------------------
// Module functions declaration
//----------------------------------------------------------------------------------
void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta, bool *destroy, int *index);
void UpdateBricks(EnvItem *envItems, int envItemsLength, bool pause, int *envItems_speed);
void UpdateCameraCenterInsideMap(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(screenWidth, screenHeight, "Endgame");

    Player player = {0};
    player.position = (Vector2){300, 800};
    player.speed = 0;
    player.canJump = false;
    EnvItem envItems[] = {
        {{0, 0, screenWidth, screenHeight}, 0, false, false, LIGHTGRAY},
        {{0, 800, screenWidth, 200}, 1, false, false, GRAY},
        {{450, 600, 100, 10}, 1, false, true, RED},
        {{250, 450, 100, 10}, 1, true, false, GRAY},
        {{50, 700, 200, 10}, 1, false, false, GRAY},
        {{0, 300, 200, 10}, 1, false, false, GRAY},
        {{0, 0, 0, 0}, 0, false, false, GRAY}};
    bool pause = false;
    int envItemsLength = sizeof(envItems) / sizeof(envItems[0]) - 1;
    int envItems_speed = 2;
    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    int framesCounter = 0;
    // Store pointers to the multiple update camera functions
    void (*cameraUpdaters[])(Camera2D *, Player *, EnvItem *, int, float, int, int) = {
        UpdateCameraCenterInsideMap};

    int cameraOption = 0;
    int cameraUpdatersLength = sizeof(cameraUpdaters) / sizeof(cameraUpdaters[0]);

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        
        //----------------------------------------------------------------------------------

        float deltaTime = GetFrameTime();
        bool destroy = false;
        int index;

        UpdateBricks(envItems, envItemsLength, pause, &envItems_speed);
        UpdatePlayer(&player, envItems, envItemsLength, deltaTime, &destroy, &index);
        if (destroy)
        {
            framesCounter++;
        }
        if ((((framesCounter / 180) % 3) == 1) && envItems[index].destroy == true)
        {
            envItems[index] = envItems[5];
            framesCounter = 0;
        }
        // Call update camera function by its pointer

        cameraUpdaters[cameraOption](&camera, &player, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        BeginMode2D(camera);

        for (int i = 0; i < envItemsLength; i++)
            DrawRectangleRec(envItems[i].rect, envItems[i].color);

        Rectangle playerRect = {player.position.x - 35, player.position.y - 70, 70, 70};
        DrawRectangleRec(playerRect, RED);

        EndMode2D();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
void UpdateBricks(EnvItem *envItems, int envItemsLength, bool pause, int *envItems_speed)
{
    for (int i = 0; i < envItemsLength; i++)
    {
        if (envItems[i].moving == true)
        {
            if (!pause)
                envItems[i].rect.x += *envItems_speed;

            // Bounce box on x screen limits
            if (((envItems[i].rect.x + envItems[i].rect.width) >= GetScreenWidth()) || (envItems[i].rect.x <= 0))
                *envItems_speed *= -1;
        }
    }
}

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta, bool *destroy, int *index)
{
    *destroy = false;
    Rectangle playerRect = {player->position.x - 35, player->position.y - 70, 70, 70};
    if (IsKeyDown(KEY_LEFT))
        player->position.x -= PLAYER_HOR_SPD * delta;
    if (player->position.x - 35 <= 0)
        player->position.x = 35;
    if (IsKeyDown(KEY_RIGHT))
        player->position.x += PLAYER_HOR_SPD * delta;
    if (player->position.x + playerRect.width >= screenWidth)
        player->position.x = screenWidth - playerRect.width;
    if (IsKeyPressed(KEY_SPACE) && player->canJump && player->jumpCounter < PLAYER_JUMP_LIMIT)
    {
        player->speed = -PLAYER_JUMP_SPD;
        player->canJump = false;
        player->jumpCounter += 1;
    }

    int hitObstacle = 0;
    Vector2 *p = &(player->position);
    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem *ei = envItems + i;

        if (ei->blocking &&
            ei->rect.x <= p->x &&
            ei->rect.x + ei->rect.width >= p->x &&
            ei->rect.y >= p->y &&
            ei->rect.y <= p->y + player->speed * delta)
        {
            hitObstacle = 1;
            player->speed = 0.0f;
            p->y = ei->rect.y;
            *index = i;
            
            if (ei->moving == true)
            {
                p->x = ei->rect.x + ei->rect.width / 2;
            }
            *destroy = ei->destroy;
        }
    }

    if (!hitObstacle)
    {
        player->position.y += player->speed * delta;
        player->speed += G * delta;
        if (player->jumpCounter < PLAYER_JUMP_LIMIT) {
            player->canJump = true;
        } else {
            player->canJump = false;
        }
    }
    else {
        player->canJump = true;
        player->jumpCounter = 0;
    }
}

void UpdateCameraCenterInsideMap(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    camera->target = player->position;
    camera->offset = (Vector2){width / 2.0f, height / 2.0f};
    float minX = 1000, minY = 2000, maxX = -1000, maxY = -2000;

    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem *ei = envItems + i;
        minX = fminf(ei->rect.x, minX);
        maxX = fmaxf(ei->rect.x + ei->rect.width, maxX);
        minY = fminf(ei->rect.y, minY);
        maxY = fmaxf(ei->rect.y + ei->rect.height, maxY);
    }

    Vector2 max = GetWorldToScreen2D((Vector2){maxX, maxY}, *camera);
    Vector2 min = GetWorldToScreen2D((Vector2){minX, minY}, *camera);

    if (max.x < width)
        camera->offset.x = width - (max.x - width / 2);
    if (max.y < height)
        camera->offset.y = height - (max.y - height / 2);
    if (min.x > 0)
        camera->offset.x = width / 2 - min.x;
    if (min.y > 0)
        camera->offset.y = height / 2 - min.y;
}
