#include "../inc/main.h"

#define G 680
#define PLAYER_JUMP_SPD 340.0f
#define PLAYER_HOR_SPD 220.0f
#define PLAYER_JUMP_LIMIT 100
#define PLAYER_FRAME_WIDTH_WAIT 72.5f
#define PLAYER_FRAME_WIDTH_RUN 77.0f

#define PLAYER_FRAME_HEIGHT 213.0f
#define PLAYER_FRAME_X 70.5f

#define PLAYER_WIDTH 76.0f
#define PLAYER_HEIGHT 100.0f

#define LAVA_DEFAULT_SPEED 40.0f
#define LAVA_SPEED_MULTIPLIER 0.5f

const int screenWidth = 800;
const int screenHeight = 800;

int main(void) {
    InitWindow(screenWidth, screenHeight, "Endgame");

    /* INIT LAVA */
    Lava lava = { 0 };
    lava.rect = (Rectangle) {0, screenHeight + 200, screenWidth, screenHeight + 200};
    lava.nextFrameDataOffset = 0;
    lava.frame = 0;
    lava.frameDelay = 20;
    lava.currentAnimFrameLava = 0;
    lava.speed = 0;
    lava.speedMultiplier = 1;
    lava.animationFrames = 0;
    lava.animatedImage = LoadImageAnim("resources/textures/lava.gif", &lava.animationFrames);
    lava.texture2D = LoadTextureFromImage(lava.animatedImage);
    /* END LAVA */

    /* INIT CHARACTER */
    Player player = {0};
    player.position = (Vector2){(GetScreenWidth() / 4.0f), 800};
    player.texture2D = LoadTexture("resources/textures/character_1.png");
    player.texture2D.width /= 2;
    player.texture2D.height /= 2;

    player.frameRect = (Rectangle){ 0, 20.0f, PLAYER_FRAME_WIDTH_WAIT, PLAYER_FRAME_HEIGHT};

    player.frames = 24;
    player.frameDelay = 1;
    player.frameDelayCounter = 0;
    player.frameIndex = 0;

    player.speed = 0;
    player.canJump = false;
    player.playerStatus = PLAYER_STATUS_WAIT;

    /* END CHARACTER */

    /* INIT MAP */
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

    /* END MAP */


    /* INIT CAMERA */

    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    int framesCounter = 0;

    /* END CAMERA */


    /* INIT TRIGGERS */
    Trigger triggers[] = {
            {{(GetScreenWidth() / 2.0f), 0, 20, GetScreenHeight()}, BLUE, true, TRIGGER_TYPE_TAKE_TREASURE},
            {{(0), 400, GetScreenWidth(), 20}, BLUE, true, TRIGGER_TYPE_START_LAVA},
            {{(0), 250, GetScreenWidth(), 20}, BLUE, true, TRIGGER_TYPE_INCREASE_LAVA_SPEED},
    };
    int triggersLength = sizeof(triggers) / sizeof(triggers[0]);
    /* END TRIGGERS */

    SetTargetFPS(60);

    float deltaTime;
    float lastFrameTime = GetTime();

    /* INIT GAME LOOP */
    while (!WindowShouldClose()) {
        // Update the timer variables
        float currentFrameTime = GetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        Rectangle playerRect = {player.position.x - (PLAYER_WIDTH / 2), player.position.y - PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT - 20};

        /* UPDATE LAVA */
        UpdateLava(&lava, deltaTime);
        if (CheckCollisionRecs(lava.rect, playerRect)) {
            break;
        }

        /* CHECK TRIGGERS */
        for (int i = 0; i < triggersLength; i++) {
            int isCollision = CheckCollisionRecs(playerRect, triggers[i].rect);
            if (isCollision && triggers[i].isActive) {
                if (triggers[i].eventType == TRIGGER_TYPE_TAKE_TREASURE) {
                    triggers[i].isActive = false;
                    TraceLog(LOG_INFO, "TRIGGER_TYPE_TAKE_TREASURE");
                }

                if (triggers[i].eventType == TRIGGER_TYPE_START_LAVA) {
                    lava.speed = LAVA_DEFAULT_SPEED;
                    triggers[i].isActive = false;
                    TraceLog(LOG_INFO, "TRIGGER_TYPE_START_LAVA");
                }

                if (triggers[i].eventType == TRIGGER_TYPE_INCREASE_LAVA_SPEED) {
                    lava.speedMultiplier += LAVA_SPEED_MULTIPLIER;
                    lava.frameDelay = 15;
                    triggers[i].isActive = false;
                    TraceLog(LOG_INFO, "TRIGGER_TYPE_INCREASE_LAVA_SPEED");
                }
            }
        }

        UpdateBricks(envItems, envItemsLength, pause);
        UpdatePlayer(&player, envItems, envItemsLength, deltaTime, &destroy, &index);

        //Update destroy platforms
        if (destroy) {
            framesCounter++;
        }


        /* INIT PLAYER FRAMES */
        ++player.frameDelayCounter;
        if(player.frameDelayCounter > player.frameDelay) {
            player.frameDelayCounter = 0;

            ++player.frameIndex;
            player.frameIndex %= player.frames;
            player.frameRect.y = (float) PLAYER_FRAME_HEIGHT * player.frameIndex;
        }
        /* END PLAYER FRAMES */

        if ((((framesCounter / (envItems[index].destroy_time * 60)) % envItems[index].destroy_time) == 1) && envItems[index].destroy == true) {
            envItems[index] = envItems[17];
            framesCounter = 0;
        }

        if ((((framesCounter / (envItems[index].destroy_time * 60)) % envItems[index].destroy_time) == 0) && envItems[index].destroy == true && envItems[index].destroy_time == 1 ) {
            envItems[index] = envItems[17];
            framesCounter = 0;
        }

        /* FIX CAMERA */
        FixCameraCenterInsideMap(&camera, &player, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);

        /* START DRAWING */
        BeginDrawing();
        ClearBackground(BLANK);
        BeginMode2D(camera);

        /* ENVIRONMENT */
        for (int i = 0; i < envItemsLength; i++) {
            Vector2 vector = {(float)envItems[i].rect.x, (float)envItems[i].rect.y};
            DrawTextureV(envItems[i].photo, vector,envItems[i].color);
        }

        /* TRIGGERS */
        for (int i = 0; i < triggersLength; i++) {
            if (triggers[i].isActive) {
                DrawRectangleRec(triggers[i].rect, triggers[i].color);
            }
        }

        //TODO: DELETE AFTER
        DrawRectangleRec(player.frameRect, RED);

        /* LAVA */
        DrawTexture(lava.texture2D, lava.rect.x, lava.rect.y, WHITE);

        /* CHARACTER */
        if (player.playerStatus == PLAYER_STATUS_MOVE_RIGHT) {
            if(player.frameRect.width < 0) {
                player.frameRect.width = -PLAYER_FRAME_WIDTH_RUN;
            }
            player.frameRect.x = PLAYER_FRAME_X;
            player.frameRect.width = PLAYER_FRAME_WIDTH_RUN;
        } else if (player.playerStatus == PLAYER_STATUS_MOVE_LEFT) {
            if(player.frameRect.width > 0) {
                player.frameRect.width = -PLAYER_FRAME_WIDTH_RUN;
            }
            player.frameRect.x = PLAYER_FRAME_X;
        } else if (player.playerStatus == PLAYER_STATUS_WAIT) {
            player.frameRect.x = 0;
            player.frameRect.width = PLAYER_FRAME_WIDTH_WAIT;
        }

        DrawTextureRec(player.texture2D, player.frameRect, (Vector2) { playerRect.x, playerRect.y}, WHITE);

        EndMode2D();
        EndDrawing();

        /* EMD DRAWING */
    }

    /* END GAME LOOP */

    /* INIT UNLOAD TEXTURES */

    UnloadTexture(lava.texture2D);
    UnloadImage(lava.animatedImage);

    UnloadTexture(player.texture2D);

    /* END UNLOAD TEXTURES */

    CloseWindow();

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
    if (IsKeyDown(KEY_LEFT)) {
        player->position.x -= PLAYER_HOR_SPD * delta;
        player->playerStatus = PLAYER_STATUS_MOVE_LEFT;
    } else if (IsKeyDown(KEY_RIGHT)) {
        player->position.x += PLAYER_HOR_SPD * delta;
        player->playerStatus = PLAYER_STATUS_MOVE_RIGHT;
    }
    else {
        player->playerStatus = PLAYER_STATUS_WAIT;
    }

    if (IsKeyPressed(KEY_SPACE) && player->canJump && player->jumpCounter < PLAYER_JUMP_LIMIT) {
        player->speed = -PLAYER_JUMP_SPD;
        player->canJump = false;
        player->jumpCounter += 1;
    }

    //check if the character comes beyond the boundaries of the card
    if (player->position.y > envItems[3].rect.y) {
        if (player->position.x - (playerRect.width / 2) <= envItems[3].rect.width)
            player->position.x = envItems[3].rect.width + (playerRect.width / 2);
        if (player->position.x + (playerRect.width / 2) >= screenWidth-envItems[3].rect.width)
            player->position.x = screenWidth-envItems[3].rect.width - (playerRect.width / 2);
    }
    else {
        if (player->position.x - (playerRect.width / 2) <= 0)
            player->position.x = playerRect.width / 2;
        if (player->position.x + (playerRect.width / 2) >= screenWidth)
            player->position.x = screenWidth - (playerRect.width / 2);
    }

    int hitObstacle = 0;
    Vector2 *p = &(player->position);
    for (int i = 0; i < envItemsLength; i++) {
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

            if (ei->if_dynamic.moving == true) {
                p->x = ei->rect.x + ei->rect.width / 2;
            }

            *destroy = ei->destroy;
        }
    }

    if (!hitObstacle) {
        player->position.y += player->speed * delta;
        player->speed += G * delta;
        if (player->jumpCounter < PLAYER_JUMP_LIMIT) {
            player->canJump = true;
        }
        else {
            player->canJump = false;
        }
    }
    else {
        player->canJump = true;
        player->jumpCounter = 0;
    }
}

void UpdateLava(Lava *lava, float delta) {
    lava->rect.y -= lava->speed * lava->speedMultiplier * delta;

    lava->frame++;
    if (lava->frame >= lava->frameDelay) {
        // Move to next frame
        // NOTE: If final frame is reached we return to first frame
        lava->currentAnimFrameLava++;
        if (lava->currentAnimFrameLava >= lava->animationFrames) {
            lava->currentAnimFrameLava = 0;
        }

        // Get memory offset position for next frame data in image.data
        lava->nextFrameDataOffset = lava->animatedImage.width * lava->animatedImage.height * 4 * lava->currentAnimFrameLava;

        // Update GPU texture data with next frame image data
        // WARNING: Data size (frame size) and pixel format must match already created texture
        UpdateTexture(lava->texture2D, ((unsigned char *)lava->animatedImage.data) + lava->nextFrameDataOffset);

        lava->frame = 0;
    }
}
