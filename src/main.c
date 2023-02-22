
#include "../inc/main.h"

#define G 680
#define PLAYER_JUMP_SPD 320.0f
#define PLAYER_HOR_SPD 220.0f
#define PLAYER_JUMP_LIMIT 2
#define LAVA_SPEED 25
#define LAVA_SPEED_MULTIPLIER 1.3

const int screenWidth = 800;
const int screenHeight = 800;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(screenWidth, screenHeight, "Endgame");

    Rectangle lava = { 0, 1000, 600, 1000 };

    float deltaTime = 0.0f;
    float lastFrameTime = GetTime();
    int lavaSpeedMultiplier = 1;

    const char *filename = "scarfy.png";
    Texture2D scarfy = LoadTexture(filename);
    scarfy.height /= 2;
    scarfy.width /= 2;

    unsigned numFrames = 6;
    int frameWidth = scarfy.width / numFrames;
    Rectangle frameRec = { 0.0f, 0.0f, (float)frameWidth, (float)scarfy.height};

    TraceLog(LOG_INFO, "Player X: [%d]; Player Y: [%d]", frameRec.width, frameRec.height);
    Vector2 scarfyPosition = {screenWidth / 2.0f, screenHeight / 2.0f};
    Vector2 scarfyVelocity = {0.0f,0.0f};
    unsigned frameDelay = 5;
    unsigned frameDelayCounter = 0;
    unsigned frameIndex = 0;


    Player player = {0};
    player.position = (Vector2){300, 800};
    player.speed = 0;
    player.canJump = false;

    EnvItem envItems[] = {
        {{0, 0, screenWidth, screenHeight}, {false, 0}, 0, false, 1, LIGHTGRAY},
        {{0, 800, screenWidth, 200}, {false, 0}, 1, false, 1, GRAY},
        {{450, 600, 100, 10}, {false, 0}, 1, true, 3, RED},
        {{250, 450, 100, 10}, {true, 2}, 1, false, 1, GRAY},
        {{50, 700, 200, 10}, {false, 0}, 1, false, 1, GRAY},
        {{0, 300, 200, 10}, {false, 0}, 1, false, 1, BLACK},
        {{300, 230, 100, 10}, {true, -1}, 1, true, 1, GREEN},
        {{0, 0, 0, 0}, {false, 0}, 0, false, 0, GRAY}};
    bool pause = false;
    int envItemsLength = sizeof(envItems) / sizeof(envItems[0]) - 1;
    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    int framesCounter = 0;


    EventCheckPoint eventCheckPoint = { 0 };
    eventCheckPoint.rect = (Rectangle) {0, 500, screenWidth, 20};
    eventCheckPoint.color = BLUE;
    eventCheckPoint.isActive = true;
    eventCheckPoint.eventType = INCREASE_LAVA_SPEED;

    EventCheckPoint events[] = {eventCheckPoint};
    int eventsLength = sizeof(events) / sizeof(events[0]);

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update the timer variables
        float currentFrameTime = GetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // Update the position of the lava object
        lava.y -= LAVA_SPEED * deltaTime * lavaSpeedMultiplier;

        Rectangle playerRect = {player.position.x - frameRec.width, player.position.y - frameRec.height, frameRec.width, frameRec.height};

        for (int i = 0; i < eventsLength; i++) {
            int isCollision = CheckCollisionRecs(playerRect, events[i].rect);
            //TraceLog(LOG_INFO, "Collision: [%d]; ScreenWith: [%d]", test, GetScreenWidth());
            if (isCollision && events[i].eventType == INCREASE_LAVA_SPEED && events[i].isActive) {
                lavaSpeedMultiplier += LAVA_SPEED_MULTIPLIER;
                events[i].isActive = false;
            }
        }

        float deltaTime = GetFrameTime();
        bool destroy = false;
        int index;

        UpdateBricks(envItems, envItemsLength, pause);
        UpdatePlayer(&player, envItems, envItemsLength, deltaTime, &destroy, &index);
        if (destroy) {
            framesCounter++;
        }

        bool scarfyMoving = player.position.x != 0.0f; //|| player.position.y != 0.0f;
        //----------------------------------------------------------------------------------
        scarfyPosition = Vector2Add(scarfyPosition, scarfyVelocity);
        ++frameDelayCounter;
        if(frameDelayCounter > frameDelay) {
            frameDelayCounter = 0;

            if(scarfyMoving) {
                ++frameIndex;
                frameIndex %= numFrames;
                frameRec.x = (float) frameWidth * frameIndex;
            }
        }

        if ((((framesCounter / 180) % 3) == 1) && envItems[index].destroy == true) {
            envItems[index] = envItems[5];
            framesCounter = 0;
        }

        // Camera
        FixCameraCenterInsideMap(&camera, &player, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        BeginMode2D(camera);

        for (int i = 0; i < envItemsLength; i++) {
            DrawRectangleRec(envItems[i].rect, envItems[i].color);
        }

        for (int i = 0; i < eventsLength; i++) {
            if (events[i].isActive) {
                DrawRectangleRec(events[i].rect, events[i].color);
            }
        }

        if (CheckCollisionRecs(lava, playerRect)) {
            break;
        }

        //DrawRectangleRec(playerRect, RED);
        DrawRectangleRec(lava, RED);



        DrawTextureRec(scarfy, frameRec, (Vector2) { playerRect.x, playerRect.y}, WHITE);
        //DrawTextureRec(scarfy, frameRec, scarfyPosition, WHITE);

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
void UpdateBricks(EnvItem *envItems, int envItemsLength, bool pause)
{
    for (int i = 0; i < envItemsLength; i++)
    {
        if (envItems[i].if_dynamic.moving == true)
        {
            if (!pause)
                envItems[i].rect.x += envItems[i].if_dynamic.speed;

            // Bounce box on x screen limits
            if (((envItems[i].rect.x + envItems[i].rect.width) >= GetScreenWidth()) || (envItems[i].rect.x <= 0))
                envItems[i].if_dynamic.speed *= -1;
        }
    }
}

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta, bool *destroy, int *index)
{
    *destroy = false;
    Rectangle playerRect = {player->position.x - 35, player->position.y - 70, 70, 70};
    if (IsKeyDown(KEY_LEFT))
        player->position.x -= PLAYER_HOR_SPD * delta;
    if (player->position.x - (playerRect.width / 2) <= 0)
        player->position.x = (playerRect.width / 2);
    if (IsKeyDown(KEY_RIGHT))
        player->position.x += PLAYER_HOR_SPD * delta;
    if (player->position.x + (playerRect.width / 2) >= screenWidth)
        player->position.x = screenWidth - (playerRect.width / 2);
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

            if (ei->if_dynamic.moving == true)
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
