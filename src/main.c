#include "../inc/main.h"

#define GRAVITY 780
#define PLAYER_JUMP_SPD 420.0f
#define PLAYER_ADDITIONAL_JUMP_SPD_REDUCER 1.2f
#define PLAYER_HOR_SPD 200.0f
#define PLAYER_JUMP_LIMIT 2
#define PLAYER_FRAME_WIDTH_WAIT 72.5f
#define PLAYER_FRAME_WIDTH_RUN 77.0f

#define PLAYER_FRAME_HEIGHT 213.0f
#define PLAYER_FRAME_X 70.5f

#define PLAYER_WIDTH 76.0f
#define PLAYER_HEIGHT 100.0f

#define LAVA_DEFAULT_SPEED 40.0f
#define LAVA_SPEED_MULTIPLIER 0.5f

#define BUTTON_FRAMES 3

#define TRIGGER_LENGTH 5
#define ENV_ITEMS_LENGTH 19

const int screenWidth = 800;
const int screenHeight = 800;

static Lava lava = { 0 };
static Player player = { 0 };
static Camera2D camera = { 0 };

static Trigger triggers[TRIGGER_LENGTH] = { 0 };


static EnvItem envItems[ENV_ITEMS_LENGTH] = { 0 };
static bool pause = false;
static bool destroy = false;
static int index;
static int framesCounter = 0;

static Rectangle playerRect = { 0 };

static Texture2D player_temp;
static int status_load = 1;
static int framesCounterforLoad = 0;
static float volume = 0.5f;

static bool turn_music = false;
static Music temp_music;

void InitGame();
void UpdatePlayersEndWin(Texture2D *players);
void WorkMusic(Music temp_music);
void ChangeMusic(Music music);
int main(void) {
    InitWindow(screenWidth, screenHeight, "Endgame");

    Texture2D end_player1 = LoadTexture("resources/textures/display/end_player1.png");
    Texture2D end_player2 = LoadTexture("resources/textures/display/end_player2.png");
    Texture2D end_player3 = LoadTexture("resources/textures/display/end_player3.png");
    Texture2D end_player4 = LoadTexture("resources/textures/display/end_player4.png");
    Texture2D win_player1 = LoadTexture("resources/textures/display/win_player1.png");
    Texture2D win_player2 = LoadTexture("resources/textures/display/win_player2.png");
    Texture2D win_player3 = LoadTexture("resources/textures/display/win_player3.png");
    Texture2D win_player4 = LoadTexture("resources/textures/display/win_player4.png");
    Texture2D players_end[4] = {end_player1,end_player2, end_player3, end_player4};
    Texture2D players_win[4] = {win_player1, win_player2, win_player3, win_player4};

    InitAudioDevice();
    Music StartMusic = LoadMusicStream("resources/audio/StartMusic.mp3");
    Music MainMusic = LoadMusicStream("resources/audio/MainMusic.mp3");
    Music WaitingMusic = LoadMusicStream("resources/audio/WaitingMusic.mp3");
    Music GameOver = LoadMusicStream("resources/audio/GameOver.mp3");
    Music Win = LoadMusicStream("resources/audio/Win.mp3");
    temp_music = StartMusic;
    PlayMusicStream(temp_music);
    float volume = 0.5f;
    bool turn_music = false;
    float timePlayed = 0.0f;

    GameScreen currentScreen = GAME_SCREEN_START;
    Texture2D start_title = LoadTexture("resources/textures/display/start_title.png");
    Texture2D title_intro = LoadTexture("resources/textures/display/title_intro.png");

    Texture2D buttonStartTexture = LoadTexture("resources/textures/buttons/button_start.png");
    Texture2D buttonEndTexture = LoadTexture("resources/textures/buttons/button_exit.png");
    Texture2D buttonRetryTexture = LoadTexture("resources/textures/buttons/button_retry.png");
    Texture2D buttonBackTexture = LoadTexture("resources/textures/buttons/button_back.png");
    Texture2D buttonEndBackTexture = LoadTexture("resources/textures/buttons/button_exit.png");


    float frameHeight = (float)buttonStartTexture.height / BUTTON_FRAMES;
    float e_frameHeight = (float)buttonEndTexture.height / BUTTON_FRAMES;
    float r_frameHeight = (float)buttonRetryTexture.height / BUTTON_FRAMES;
    float b_frameHeight = (float)buttonBackTexture.height / BUTTON_FRAMES;
    float e_end_frameHeight = (float)buttonEndBackTexture.height / BUTTON_FRAMES;

    Button buttonStart = {
            buttonStartTexture,
            (Rectangle) {500, 100 , (float)buttonStartTexture.width, frameHeight},
            (Rectangle) {0, 0, (float)buttonStartTexture.width, frameHeight},
            frameHeight,
            BUTTON_STATE_NORMAL,
            false
    };

    Button buttonExit = {
            buttonEndTexture,
            (Rectangle) {500, 250, (float)buttonEndTexture.width, e_frameHeight},
            (Rectangle) {0, 0, (float)buttonEndTexture.width, e_frameHeight},
            e_frameHeight,
            BUTTON_STATE_NORMAL,
            false
    };

    Button buttonRetry = {
            buttonRetryTexture,
            (Rectangle) {screenWidth / 6.0f - buttonRetryTexture.width / 2.0f, 700 , (float)buttonRetryTexture.width, r_frameHeight},
            (Rectangle) {0, 0, (float)buttonRetryTexture.width, r_frameHeight},
            r_frameHeight,
            BUTTON_STATE_NORMAL,
            false
    };

    Button buttonBack = {
            buttonBackTexture,
            (Rectangle) {screenWidth / 2.0f - buttonBackTexture.width / 2.0f, 700, (float)buttonBackTexture.width, b_frameHeight},
            (Rectangle) {0, 0, (float)buttonBackTexture.width, b_frameHeight},
            b_frameHeight,
            BUTTON_STATE_NORMAL,
            false
    };

    Button buttonEndBack = {
            buttonEndBackTexture,
            (Rectangle) {screenWidth / 1.2f - buttonBackTexture.width / 2.0f, 700, (float)buttonEndBackTexture.width, e_end_frameHeight},
            (Rectangle) {0, 0, (float)buttonEndBackTexture.width, e_end_frameHeight},
            e_end_frameHeight,
            BUTTON_STATE_NORMAL,
            false
    };

    //----------------------------------------------
    Vector2 mousePoint = {0.0f, 0.0f};
    int button_status = 0;
    int lettersCount = 0;


    Texture2D stand = LoadTexture("resources/textures/stand.png");
    Texture2D almaz = LoadTexture("resources/textures/almaz.png");
    Texture2D flag = LoadTexture("resources/textures/flag.png");
    Rectangle almazRec = { 450, 753, (float)almaz.width, (float)almaz.height};

    /**
     * INIT GAME
     */
    InitGame();

    SetTargetFPS(60);

    float deltaTime;
    float lastFrameTime = GetTime();
    /* INIT GAME LOOP */
    while (!WindowShouldClose()) {

        if (currentScreen == GAME_SCREEN_START) {
            {
                WorkMusic(temp_music);

                mousePoint = GetMousePosition();
                buttonStart.isAction = false;
                buttonExit.isAction = false;

                // Check button state
                if (CheckCollisionPointRec(mousePoint, buttonStart.rect)) {
                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                        buttonStart.buttonState = BUTTON_STATE_PRESSED;
                    } else {
                        buttonStart.buttonState = BUTTON_STATE_MOUSE_HOVER;
                    }

                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                        buttonStart.isAction = true;
                        button_status = 1;
                    }
                }
                else if (CheckCollisionPointRec(mousePoint, buttonExit.rect)) {
                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                        buttonExit.buttonState = BUTTON_STATE_PRESSED;
                    }
                    else {
                        buttonExit.buttonState = BUTTON_STATE_MOUSE_HOVER;
                    }

                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                        buttonExit.isAction = true;
                        button_status = 2;
                    }
                }
                else {
                    buttonStart.buttonState = BUTTON_STATE_NORMAL;
                    buttonExit.buttonState = BUTTON_STATE_NORMAL;
                }

                if (buttonStart.isAction && button_status == 1) {
                    ChangeMusic(WaitingMusic);
                    currentScreen = GAME_SCREEN_GAMEPLAY;
                    // TODO: Any desired action
                }
                else if (buttonExit.isAction && button_status == 2) {
                    currentScreen = GAME_SCREEN_EXIT;
                }

                // Calculate button frame rectangle to draw depending on button state
                buttonStart.textureRect.y = buttonStart.buttonState * buttonStart.frameHeight;
                buttonExit.textureRect.y = buttonExit.buttonState * buttonExit.frameHeight;
            }
        }
        else if (currentScreen == GAME_SCREEN_GAMEPLAY) {
            WorkMusic(temp_music);

            // Update the timer variables
            float currentFrameTime = GetTime();
            deltaTime = currentFrameTime - lastFrameTime;
            lastFrameTime = currentFrameTime;

            playerRect.x = player.position.x - (PLAYER_WIDTH / 2);
            playerRect.y = player.position.y - PLAYER_HEIGHT;
            playerRect.width = PLAYER_WIDTH;
            playerRect.height = PLAYER_HEIGHT - 20;

            /* UPDATE LAVA */
            UpdateLava(&lava, deltaTime);
            if (CheckCollisionRecs(lava.rect, playerRect)) {

                ChangeMusic(GameOver);
                currentScreen = GAME_SCREEN_ENDING;
            }

            /* CHECK TRIGGERS */
            for (int i = 0; i < TRIGGER_LENGTH; i++) {
                int isCollision = CheckCollisionRecs(playerRect, triggers[i].rect);
                if (isCollision && triggers[i].isActive) {
                    if (triggers[i].eventType == TRIGGER_TYPE_TAKE_TREASURE) {
                        triggers[i].isActive = false;
                        player.canMove = true;
                        TraceLog(LOG_INFO, "TRIGGER_TYPE_TAKE_TREASURE");
                    }

                    if (triggers[i].eventType == TRIGGER_TYPE_START_LAVA) {
                        lava.speed = LAVA_DEFAULT_SPEED;
                        triggers[i].isActive = false;

                        ChangeMusic(MainMusic);

                        TraceLog(LOG_INFO, "TRIGGER_TYPE_START_LAVA");
                    }

                    if (triggers[i].eventType == TRIGGER_TYPE_INCREASE_LAVA_SPEED) {
                        lava.speedMultiplier += LAVA_SPEED_MULTIPLIER;
                        lava.frameDelay = 15;
                        triggers[i].isActive = false;
                        TraceLog(LOG_INFO, "TRIGGER_TYPE_INCREASE_LAVA_SPEED");
                    }

                    if (triggers[i].eventType == TRIGGER_TYPE_LAVA_SHUTDOWN) {
                        ChangeMusic(WaitingMusic);
                        lava.speed = 0;
                        triggers[i].isActive = false;
                        TraceLog(LOG_INFO, "TRIGGER_TYPE_LAVA_SHUTDOWN");
                    }

                    if (triggers[i].eventType == TRIGGER_TYPE_END_FLAG) {
                        triggers[i].isActive = false;
                        currentScreen = GAME_SCREEN_WIN;

                        ChangeMusic(Win);

                        TraceLog(LOG_INFO, "TRIGGER_TYPE_END_FLAG");
                    }
                }
            }

            UpdateBricks(envItems, ENV_ITEMS_LENGTH, pause);
            UpdatePlayer(&player, envItems, ENV_ITEMS_LENGTH, deltaTime, &destroy, &index);

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
            FixCameraCenterInsideMap(&camera, &player, envItems, ENV_ITEMS_LENGTH, deltaTime, screenWidth, screenHeight);
        } else if (currentScreen == GAME_SCREEN_ENDING) {
            UpdateMusicStream(temp_music);
            if (IsKeyPressed(KEY_P)) {
                turn_music = !turn_music;

                if (turn_music) SetMasterVolume(0);
                else SetMasterVolume(volume);
            }
            if (IsKeyDown(KEY_MINUS) && volume > 0.0f) volume -= 0.1f;
            else if (IsKeyDown(KEY_EQUAL) && volume < 1.0f) volume += 0.1f;

            SetMusicVolume(temp_music, volume);
            mousePoint = GetMousePosition();
            buttonRetry.isAction = false;
            buttonBack.isAction = false;
            buttonEndBack.isAction = false;
            if (CheckCollisionPointRec(mousePoint, buttonRetry.rect)) {
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    buttonRetry.buttonState = BUTTON_STATE_PRESSED;
                }
                else {
                    buttonRetry.buttonState = BUTTON_STATE_MOUSE_HOVER;
                }

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    buttonRetry.isAction = true;
                    button_status = 3;
                }
            }
            else if (CheckCollisionPointRec(mousePoint, buttonBack.rect)) {
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    buttonBack.buttonState = BUTTON_STATE_PRESSED;
                }
                else {
                    buttonBack.buttonState = BUTTON_STATE_MOUSE_HOVER;
                }

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    buttonBack.isAction = true;
                    button_status = 4;
                }
            }
            else if (CheckCollisionPointRec(mousePoint, buttonEndBack.rect)) {
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    buttonEndBack.buttonState = BUTTON_STATE_PRESSED;
                }
                else {
                    buttonEndBack.buttonState = BUTTON_STATE_MOUSE_HOVER;
                }

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    buttonEndBack.isAction = true;
                    button_status = 5;
                }
            }
            else {
                buttonRetry.buttonState = BUTTON_STATE_NORMAL;
                buttonBack.buttonState = BUTTON_STATE_NORMAL;
                buttonEndBack.buttonState = BUTTON_STATE_NORMAL;
            }

            if (buttonRetry.isAction && button_status == 3) {
                ChangeMusic(WaitingMusic);
                currentScreen = GAME_SCREEN_GAMEPLAY;
                // TODO: Any desired action
            }
            else if (buttonBack.isAction && button_status == 4) {
                ChangeMusic(StartMusic);
                currentScreen = GAME_SCREEN_START;
            }
            else if (buttonEndBack.isAction && button_status == 5) {
                currentScreen = GAME_SCREEN_EXIT;
            }

            buttonRetry.textureRect.y = buttonRetry.buttonState * buttonRetry.frameHeight;
            buttonBack.textureRect.y = buttonBack.buttonState * buttonBack.frameHeight;
            buttonEndBack.textureRect.y = buttonEndBack.buttonState * buttonEndBack.frameHeight;
            if (button_status == 3 || button_status == 4) {
                //TODO: INIT GAME
                InitGame();

                lettersCount = 0;
                button_status = 0;
                status_load = 1;
                framesCounterforLoad = 0;

            }
            if(lettersCount < 9) {
                framesCounter++;
                if ((((framesCounter / 10) % 3) == 1)) {
                    lettersCount++;
                    framesCounter = 0;
                }
            }
            timePlayed = GetMusicTimePlayed(temp_music) / GetMusicTimeLength(temp_music);

            if (timePlayed >= 0.999f) StopMusicStream(temp_music);
            UpdatePlayersEndWin(players_end);
        } else if (currentScreen == GAME_SCREEN_WIN) {
            WorkMusic(temp_music);
            mousePoint = GetMousePosition();
            buttonRetry.isAction = false;
            buttonBack.isAction = false;
            buttonEndBack.isAction = false;
            // Check button state
            if (CheckCollisionPointRec(mousePoint, buttonRetry.rect))
            {
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                {
                    buttonRetry.buttonState = BUTTON_STATE_PRESSED;
                }
                else
                {
                    buttonRetry.buttonState = BUTTON_STATE_MOUSE_HOVER;
                }

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                {
                    buttonRetry.isAction = true;
                    button_status = 3;
                }
            }
            else if (CheckCollisionPointRec(mousePoint, buttonBack.rect))
            {
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    buttonBack.buttonState = BUTTON_STATE_PRESSED;
                }
                else {
                    buttonBack.buttonState = BUTTON_STATE_MOUSE_HOVER;
                }

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    buttonBack.isAction = true;
                    button_status = 4;
                }
            }
            else if (CheckCollisionPointRec(mousePoint, buttonEndBack.rect)) {
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    buttonEndBack.buttonState = BUTTON_STATE_PRESSED;
                } else {
                    buttonEndBack.buttonState = BUTTON_STATE_MOUSE_HOVER;
                }

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    buttonEndBack.isAction = true;
                    button_status = 5;
                }
            }
            else
            {
                buttonRetry.buttonState = BUTTON_STATE_NORMAL;
                buttonBack.buttonState = BUTTON_STATE_NORMAL;
                buttonEndBack.buttonState = BUTTON_STATE_NORMAL;
            }

            if (buttonRetry.isAction && button_status == 3)
            {
                ChangeMusic(WaitingMusic);
                currentScreen = GAME_SCREEN_GAMEPLAY;
                // TODO: Any desired action
            }
            else if (buttonBack.isAction && button_status == 4)
            {
                ChangeMusic(StartMusic);
                currentScreen = GAME_SCREEN_START;
            }
            else if (buttonEndBack.isAction && button_status == 5)
            {
                currentScreen = GAME_SCREEN_EXIT;
            }

            // Calculate button frame rectangle to draw depending on button state
            buttonRetry.textureRect.y = buttonRetry.buttonState * buttonRetry.frameHeight;
            buttonBack.textureRect.y = buttonBack.buttonState * buttonBack.frameHeight;
            buttonEndBack.textureRect.y = buttonEndBack.buttonState * e_end_frameHeight;
            if (button_status == 3 || button_status == 4)
            {
                //TODO: INIT GAME
                InitGame();

                lettersCount = 0;
                button_status = 0;
                status_load = 1;
                framesCounterforLoad = 0;
            }
            if(lettersCount < 8) {
                framesCounter++;
                if ((((framesCounter / 10) % 3) == 1)) {
                    lettersCount++;
                    framesCounter = 0;
                }
            }
            timePlayed = GetMusicTimePlayed(temp_music) / GetMusicTimeLength(temp_music);

            if (timePlayed >= 0.989f) StopMusicStream(temp_music);
            UpdatePlayersEndWin(players_win);
        } else if (currentScreen == GAME_SCREEN_EXIT) {
            break;
        }

        /* START DRAWING */
        BeginDrawing();
        ClearBackground(BLANK);

        //TODO: выключать музыку при касании с лавой, для

        if (currentScreen == GAME_SCREEN_START) {
            DrawTexture(start_title,0 , 0 , WHITE );
            DrawText("START to start the game ",20, 10, 16, DARKBROWN );
            DrawText("EXIT to close the game",20, 30, 16, DARKBROWN  );
            DrawText("RETRY to try again",20, 50, 16, DARKBROWN  );
            DrawText("BACK to return to the start screen",20, 70, 16, DARKBROWN  );
            DrawText("P to mute the sound",20, 90, 16, DARKBROWN  );
            DrawText("+ and - to turn the sound louder or quieter",20, 110, 16, DARKBROWN  );
            DrawText("Character control:",20, 140, 16, DARKBROWN  );
            DrawText("<- - move right",20, 160, 16, DARKBROWN  );
            DrawText("-> - move left",20, 180, 16, DARKBROWN  );
            DrawText("SPACE - jump up",20, 200, 16, DARKBROWN  );
            DrawText("double SPACE - double jump up",20, 220, 16, DARKBROWN  );

            DrawText("Powered by:",20, 730, 20, BROWN );

            DrawText("Andrew Laskevych      Inessa Repeshko     Polina Skuratovets",20, 750, 20, BROWN );
            DrawText("Denys Kolesnychenko  Vadym Zharyi          Anton Lukash",20, 770, 20, BROWN );
            DrawTextureRec(buttonStartTexture, buttonStart.textureRect, (Vector2){buttonStart.rect.x, buttonStart.rect.y}, WHITE);
            DrawTextureRec(buttonEndTexture, buttonExit.textureRect, (Vector2){buttonExit.rect.x, buttonExit.rect.y}, WHITE);
        } else if (currentScreen == GAME_SCREEN_GAMEPLAY) {
            UpdateMusicStream(temp_music);

            BeginMode2D(camera);

            /* ENVIRONMENT */
            for (int i = 0; i < ENV_ITEMS_LENGTH; i++) {
                Vector2 vector = {(float)envItems[i].rect.x, (float)envItems[i].rect.y};
                DrawTextureV(envItems[i].photo, vector,envItems[i].color);
            }

            /* TRIGGERS */
            for (int i = 0; i < TRIGGER_LENGTH; i++) {
                if (triggers[i].isActive) {
                    DrawRectangleRec(triggers[i].rect, triggers[i].color);
                }
            }

            //TODO: DELETE AFTER
            //DrawRectangleRec(player.frameRect, RED);

            /* FLAG */
            DrawTexture(flag,518, -505, WHITE);
            /* STAND */
            DrawTexture(stand,450, 753, WHITE);
            /* ALMAZ */
            DrawTexture(almaz,455, 733, WHITE);

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

            if (CheckCollisionRecs(almazRec, playerRect)) {
                almaz.id = 0;
                DrawText("Hooray, I found the treasure!\n"
                         "Something is getting hot...",
                         450, 450, 15, WHITE);
            }
        } else if (currentScreen == GAME_SCREEN_ENDING) {
            DrawTexture(title_intro,0 , 0 , WHITE );
            DrawTexture(player_temp, screenWidth/2 - player_temp.width/2 , screenHeight/2 - player_temp.height/2 + 103, WHITE);
            DrawText(TextSubtext("Game Over", 0, lettersCount), GetScreenWidth()/2 - 250, GetScreenHeight()/2 - 200, 100, BLACK);
            DrawTextureRec(buttonRetryTexture, buttonRetry.textureRect, (Vector2){buttonRetry.rect.x, buttonRetry.rect.y}, WHITE);
            DrawTextureRec(buttonBackTexture, buttonBack.textureRect, (Vector2){buttonBack.rect.x, buttonBack.rect.y}, WHITE);
            DrawTextureRec(buttonEndBackTexture, buttonEndBack.textureRect, (Vector2){buttonEndBack.rect.x, buttonEndBack.rect.y}, WHITE);
        } else if (currentScreen == GAME_SCREEN_WIN) {
            DrawTexture(title_intro,0 , 0 , WHITE );
            DrawTexture(player_temp, screenWidth/2 - player_temp.width/2 , screenHeight/2 - player_temp.height/2 + 103, WHITE);
            DrawText(TextSubtext("You Win", 0, lettersCount), GetScreenWidth()/2 - 220, GetScreenHeight()/2 - 200, 100, BLACK);
            DrawTextureRec(buttonRetryTexture, buttonRetry.textureRect, (Vector2){buttonRetry.rect.x, buttonRetry.rect.y}, WHITE); // Draw button frame
            DrawTextureRec(buttonBackTexture, buttonBack.textureRect, (Vector2){buttonBack.rect.x, buttonBack.rect.y}, WHITE); // Draw button frame
            DrawTextureRec(buttonEndBackTexture, buttonEndBack.textureRect, (Vector2){buttonEndBack.rect.x, buttonEndBack.rect.y}, WHITE); // Draw button frame
        } else if (currentScreen == GAME_SCREEN_EXIT) {
            break;
        }

        EndDrawing();
        /* EMD DRAWING */
    }

    /* END GAME LOOP */

    /* INIT UNLOAD TEXTURES */
    UnloadTexture(lava.texture2D);
    UnloadImage(lava.animatedImage);

    UnloadTexture(flag);
    UnloadTexture(stand);
    UnloadTexture(almaz);

    UnloadTexture(player.texture2D);
    /* END UNLOAD TEXTURES */

    /* INIT UNLOAD AUDIO */
    UnloadMusicStream(StartMusic);
    UnloadMusicStream(MainMusic);
    UnloadMusicStream(GameOver);
    CloseAudioDevice();
    /* END UNLOAD AUDIO */

    CloseWindow();

    return 0;
}

void UpdatePlayersEndWin(Texture2D *players) {
        if(status_load != 6) {
            framesCounterforLoad++;
            //Every two seconds (120 frames) a new random value is generated
            if (status_load == 1) {
                player_temp = players[0];
                status_load = 2;
                framesCounterforLoad = 0;
            } else if (((framesCounterforLoad / 30) % 2) == 1 && status_load == 2) {
                player_temp = players[1];
                status_load = 3;
                framesCounterforLoad = 0;
            } else if (((framesCounterforLoad / 30) % 2) == 1 && status_load == 3) {
                player_temp = players[2];
                status_load = 4;
                framesCounterforLoad = 0;
            } else if (((framesCounterforLoad / 30) % 2) == 1 && status_load == 4) {
                player_temp = players[3];
                status_load = 5;
                framesCounterforLoad = 0;
            } else if (((framesCounterforLoad / 30) % 2) == 1 && status_load == 5)
                status_load = 6;
        }
}

void WorkMusic(Music temp_music) {
    UpdateMusicStream(temp_music);
    if (IsKeyPressed(KEY_M)) {
        turn_music = !turn_music;

        if (turn_music) SetMasterVolume(0);
        else SetMasterVolume(volume);
    }
    if (IsKeyDown(KEY_MINUS) && volume > 0.0f) volume -= 0.1f;
    else if (IsKeyDown(KEY_EQUAL) && volume < 1.0f) volume += 0.1f;
    SetMusicVolume(temp_music, volume);
}

void ChangeMusic(Music music) {
    StopMusicStream(temp_music);
    temp_music = music;
    PlayMusicStream(temp_music);
}

void InitGame() {

    /* INIT LAVA */
    lava.rect = (Rectangle) {100, screenHeight + 200, screenWidth - 200, screenHeight - 100};
    lava.nextFrameDataOffset = 0;
    lava.frame = 0;
    lava.frameDelay = 20;
    lava.currentAnimFrameLava = 0;
    lava.speed = 0;
    lava.speedMultiplier = 1;
    lava.animationFrames = 0;
    lava.animatedImage = LoadImageAnim("resources/textures/lava/lava2.gif", &lava.animationFrames);
    lava.texture2D = LoadTextureFromImage(lava.animatedImage);
    /* END LAVA */

    /* INIT CHARACTER */
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
    player.canMove = false;
    player.playerStatus = PLAYER_STATUS_WAIT;
    /* END CHARACTER */

    /* INIT CAMERA */
    camera.target = player.position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.2f;
    /* END CAMERA */

    /* INIT TRIGGERS */
    triggers[0] = (Trigger) {{(GetScreenWidth() / 1.77f), 0, 20, GetScreenHeight()}, BLUE, true, TRIGGER_TYPE_TAKE_TREASURE};
    triggers[1] = (Trigger) {{(GetScreenWidth() / 1.5f), -430, 20, 50}, BLUE, true, TRIGGER_TYPE_END_FLAG};
    triggers[2] = (Trigger) {{(0), 400, GetScreenWidth(), 20}, BLUE, true, TRIGGER_TYPE_START_LAVA};
    triggers[3] = (Trigger) {{(0), 250, GetScreenWidth(), 20}, BLUE, true, TRIGGER_TYPE_INCREASE_LAVA_SPEED};
    triggers[4] = (Trigger) {{(0), -420, GetScreenWidth(), 20}, BLUE, true, TRIGGER_TYPE_LAVA_SHUTDOWN};
    /* END TRIGGERS */

    /* INIT MAP */
    Texture2D staticPlatformTexture = LoadTexture("resources/textures/static.png");
    Texture2D dynamicPlatformTexture = LoadTexture("resources/textures/dynamic.png");
    Texture2D sideWallTexture = LoadTexture("resources/textures/board.png");
    Texture2D floorBackgroundTexture = LoadTexture("resources/textures/flour.png");
    Texture2D backgroundTexture = LoadTexture("resources/textures/back.png");
    Texture2D finalBackgroundTexture = LoadTexture("resources/textures/pyramid.png");

    envItems[0] = (EnvItem) {finalBackgroundTexture, {0, -700, screenWidth, 400}, {false, 0}, 0, false, 1,WHITE};
    envItems[1] = (EnvItem) {backgroundTexture, {0, -300, screenWidth, 1400}, {false, 0}, 0, false, 1, BROWN};
    envItems[2] = (EnvItem) {floorBackgroundTexture, {0, 800, screenWidth, 200}, {false, 0}, 1, false, 1, BROWN};
    envItems[3] = (EnvItem) {sideWallTexture, {0, -200, 100, 1000}, {false, 0}, 1, false, 1, BROWN};
    envItems[4] = (EnvItem) {sideWallTexture, {700, -200, 100, 1000}, {false, 0}, 1, false, 1, BROWN};
    envItems[5] = (EnvItem) {dynamicPlatformTexture, {350, 600, 100, 20}, {false, 0}, 1, true, 3, RED};

    envItems[6] = (EnvItem) {dynamicPlatformTexture, {250, 450, 100, 20}, {true, 2}, 1, false, 1, DARKBROWN};
    envItems[7] = (EnvItem) {staticPlatformTexture, {50, 700, 400, 20}, {false, 0}, 1, false, 1, DARKBROWN};
    envItems[8] = (EnvItem) {staticPlatformTexture, {0, 300, 400, 20}, {false, 0}, 1, false, 1, DARKBROWN};
    envItems[9] = (EnvItem) {staticPlatformTexture, {400, 100, 400, 20}, {false, 0}, 1, false, 1, DARKBROWN};
    envItems[10] = (EnvItem) {dynamicPlatformTexture, {200, 50, 100, 20}, {true, -2}, 1, true, 1, GREEN};

    envItems[11] = (EnvItem) {staticPlatformTexture, {0, -50, 400, 20}, {false, 0}, 1, false, 1, DARKBROWN};
    envItems[12] = (EnvItem) {staticPlatformTexture, {80, -100, 400, 20}, {false, 0}, 1, false, 1, DARKBROWN};
    envItems[13] = (EnvItem) {dynamicPlatformTexture, {350, -150, 100, 20}, {true, 2}, 1, true, 1, GREEN};
    envItems[14] = (EnvItem) {staticPlatformTexture, {0, -300, 400, 20}, {false, 0}, 1, false, 1, DARKBROWN};
    envItems[15] = (EnvItem) {staticPlatformTexture, {250, -380, 400, 20}, {false, 0}, 1, false, 1, DARKBROWN};

    envItems[16] = (EnvItem) {staticPlatformTexture, {0, -700, 400, 20}, {false, 0}, 1, false, 1, BLANK};
    envItems[17] = (EnvItem) {dynamicPlatformTexture, {300, 230, 100, 20}, {true, -1}, 1, true, 1, GREEN};
    envItems[18] = (EnvItem) {staticPlatformTexture, {0, 0, 0, 0}, {false, 0}, 0, false, 0, BLANK};
    /* END MAP */
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
    }
    else if (IsKeyDown(KEY_RIGHT)) {
        player->position.x += PLAYER_HOR_SPD * delta;
        player->playerStatus = PLAYER_STATUS_MOVE_RIGHT;
    }
    else {
        player->playerStatus = PLAYER_STATUS_WAIT;
    }

    if (IsKeyPressed(KEY_SPACE)
        && player->canJump
        && player->jumpCounter < PLAYER_JUMP_LIMIT
        && player->canMove
        ) {
        player->speed = player->jumpCounter >= 1
                ? (-PLAYER_JUMP_SPD / PLAYER_ADDITIONAL_JUMP_SPD_REDUCER)
                : (-PLAYER_JUMP_SPD);
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
        player->speed += GRAVITY * delta;
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
