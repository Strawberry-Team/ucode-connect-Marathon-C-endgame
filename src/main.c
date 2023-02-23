
#include "../inc/main.h"

#define G 680
#define PLAYER_JUMP_SPD 320.0f
#define PLAYER_HOR_SPD 220.0f
#define PLAYER_JUMP_LIMIT 10
#define LAVA_SPEED 25
#define LAVA_SPEED_MULTIPLIER 1.3

const int screenWidth = 800;
const int screenHeight = 800;

void UpdateBoards(Player *player, EnvItem *board, int boardLength, int delta);

int main(void)
{
    
    InitWindow(screenWidth, screenHeight, "Endgame");
//-------------lava-----------

    Rectangle lava = {0, 1000, 800, 1000};
    Texture2D texture = LoadTexture("resources/textures/brick.png");
    float deltaTime = 0.0f;
    float lastFrameTime = GetTime();
    int lavaSpeedMultiplier = 1;
//-------------player-----------

    Player player = {0};
    player.position = (Vector2){300, 800};
    player.speed = 0;
    player.canJump = false;
//-------------map texture------

    Texture2D stat = LoadTexture("resources/textures/static.png");
    Texture2D dynamic = LoadTexture("resources/textures/dynamic.png");
    Texture2D board = LoadTexture("resources/textures/board.png");
    Texture2D flour = LoadTexture("resources/textures/flour.png");
    Texture2D back = LoadTexture("resources/textures/back.png");
    Texture2D pyramid = LoadTexture("resources/textures/pyramid.png");

    EnvItem envItems[] = {
        {pyramid, {0, -700, screenWidth, 400}, {false, 0}, 0, false, 1,WHITE},
        {back, {0, -300, screenWidth, 1400}, {false, 0}, 0, false, 1, BROWN},
        {flour, {0, 800, screenWidth, 200}, {false, 0}, 1, false, 1, BROWN},
        {board, {0, -200, 100, 1000}, {false, 0}, 1, false, 1, BROWN}, 
        {board, {700, -200, 100, 1000}, {false, 0}, 1, false, 1, BROWN},
        {dynamic, {350, 600, 100, 20}, {false, 0}, 1, true, 3, RED},
        {dynamic, {250, 450, 100, 20}, {true, 2}, 1, false, 1, DARKBROWN},
        {stat, {50, 700, 400, 20}, {false, 0}, 1, false, 1, DARKBROWN},
        {stat, {0, 300, 400, 20}, {false, 0}, 1, false, 1, DARKBROWN},
        {stat, {400, 100, 400, 20}, {false, 0}, 1, false, 1, DARKBROWN},
        {dynamic, {200, 50, 100, 20}, {true, -2}, 1, true, 1, GREEN},
        {stat, {0, -50, 400, 20}, {false, 0}, 1, false, 1, DARKBROWN},
        {stat, {80, -100, 400, 20}, {false, 0}, 1, false, 1, DARKBROWN},
        {dynamic, {350, -150, 100, 20}, {true, 2}, 1, true, 1, GREEN},
        {stat, {0, -300, 400, 20}, {false, 0}, 1, false, 1, DARKBROWN},
        {stat, {250, -380, 400, 20}, {false, 0}, 1, false, 1, DARKBROWN},
        {stat, {0, -700, 400, 20}, {false, 0}, 1, false, 1, BLANK},
        {dynamic, {300, 230, 100, 20}, {true, -1}, 1, true, 1, GREEN},
        {stat, {0, 0, 0, 0}, {false, 0}, 0, false, 0, BLANK}};
    
    bool pause = false;
    int envItemsLength = sizeof(envItems) / sizeof(envItems[0]) - 1;
    bool destroy = false;
    int index;

//-------------camera------

    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    int framesCounter = 0;

//------------- event-----

    EventCheckPoint eventCheckPoint = {0};
    eventCheckPoint.rect = (Rectangle){0, 500, screenWidth, 20};
    eventCheckPoint.color = BLUE;
    eventCheckPoint.isActive = true;
    eventCheckPoint.eventType = INCREASE_LAVA_SPEED;
    EventCheckPoint events[] = {eventCheckPoint};
    int eventsLength = sizeof(events) / sizeof(events[0]);

//------------------------------
    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {
        // Update the timer variables
        float currentFrameTime = GetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
        Rectangle playerRect = {player.position.x - 35, player.position.y - 70, 70, 70};

        // Update the position of the lava object
        lava.y -= LAVA_SPEED * deltaTime * lavaSpeedMultiplier;

        //Update after event
        for (int i = 0; i < eventsLength; i++) {
            int isCollision = CheckCollisionRecs(playerRect, events[i].rect);
            // TraceLog(LOG_INFO, "Collision: [%d]; ScreenWith: [%d]", test, GetScreenWidth());
            if (isCollision && events[i].eventType == INCREASE_LAVA_SPEED && events[i].isActive) {
                lavaSpeedMultiplier += LAVA_SPEED_MULTIPLIER;
                events[i].isActive = false;
            }
        }

        float deltaTime = GetFrameTime();
       
      
        UpdateBricks(envItems, envItemsLength, pause);
        UpdatePlayer(&player, envItems, envItemsLength, deltaTime, &destroy, &index);

        //Update destroy platforms
        if (destroy) {
            framesCounter++;
        }

        if ((((framesCounter / (envItems[index].destroy_time * 60)) % envItems[index].destroy_time) == 1) && envItems[index].destroy == true) {
            envItems[index] = envItems[17];
            framesCounter = 0;
        }

        if ((((framesCounter / (envItems[index].destroy_time * 60)) % envItems[index].destroy_time) == 0) && envItems[index].destroy == true && envItems[index].destroy_time==1 ) {
            envItems[index] = envItems[17];
            framesCounter = 0;
        }

        // Camera
        FixCameraCenterInsideMap(&camera, &player, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
      
        // Draw
        BeginDrawing();
        ClearBackground(LIGHTGRAY);
        BeginMode2D(camera);

        //EnviItems drawing
        for (int i = 0; i < envItemsLength; i++)
        {
            Vector2 vector = {(float)envItems[i].rect.x, (float)envItems[i].rect.y};
            DrawTextureV(envItems[i].photo, vector,envItems[i].color);
        }
        //events drawing
        for (int i = 0; i < eventsLength; i++)
        {
            if (events[i].isActive)
            {

                DrawRectangleRec(events[i].rect, events[i].color);
            }
        }

        //check collision
        if (CheckCollisionRecs(lava, playerRect))
        {
            break;
        }

        DrawRectangleRec(playerRect, RED);
        DrawRectangleRec(lava, RED);

        EndMode2D();

        EndDrawing();
       
    }

    // De-Initialization

    CloseWindow(); // Close window and OpenGL context
   
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
            if (((envItems[i].rect.x + envItems[i].rect.width) >= screenWidth-envItems[3].rect.width) || (envItems[i].rect.x <= envItems[3].rect.width))
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
    
    if (IsKeyDown(KEY_RIGHT))
        player->position.x += PLAYER_HOR_SPD * delta;
  
    if (IsKeyPressed(KEY_SPACE) && player->canJump && player->jumpCounter < PLAYER_JUMP_LIMIT)
    {
        player->speed = -PLAYER_JUMP_SPD;
        player->canJump = false;
        player->jumpCounter += 1;
    }

    //check if the character comes beyond the boundaries of the card
    if (player->position.y > envItems[3].rect.y)
    {
        if (player->position.x - (playerRect.width / 2) <= envItems[3].rect.width)
            player->position.x = envItems[3].rect.width + (playerRect.width / 2);
        if (player->position.x + (playerRect.width / 2) >= screenWidth-envItems[3].rect.width)
            player->position.x = screenWidth-envItems[3].rect.width - (playerRect.width / 2);
    }
    else
    {
        if (player->position.x - (playerRect.width / 2) <= 0)
            player->position.x = playerRect.width / 2;
        if (player->position.x + (playerRect.width / 2) >= screenWidth)
            player->position.x = screenWidth - (playerRect.width / 2);
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
        if (player->jumpCounter < PLAYER_JUMP_LIMIT)
        {
            player->canJump = true;
        }
        else
        {
            player->canJump = false;
        }
    }
    else
    {
        player->canJump = true;
        player->jumpCounter = 0;
    }
}
