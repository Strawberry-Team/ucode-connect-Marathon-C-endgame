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
#define ENV_ITEMS_LENGTH 79

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
    flag.width /= 1.5f;
    flag.height /= 1.5f;
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
                envItems[index] = envItems[0];
                framesCounter = 0;
            }

            if ((((framesCounter / (envItems[index].destroy_time * 60)) % envItems[index].destroy_time) == 0) && envItems[index].destroy == true && envItems[index].destroy_time == 1 ) {
                envItems[index] = envItems[0];
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
            DrawTexture(flag,730, -6216, WHITE);
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
                         450, 550, 15, WHITE);
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
    camera.zoom = 1.3f;
    /* END CAMERA */

    /* INIT TRIGGERS */
    triggers[0] = (Trigger) {{(GetScreenWidth() / 1.77f), 0, 20, GetScreenHeight()}, BLUE, true, TRIGGER_TYPE_TAKE_TREASURE};
    triggers[1] = (Trigger) {{750, -6180, 20, 50}, BLUE, true, TRIGGER_TYPE_END_FLAG};
    triggers[2] = (Trigger) {{(0), 400, GetScreenWidth(), 20}, BLUE, true, TRIGGER_TYPE_START_LAVA};
    triggers[3] = (Trigger) {{(0), 250, GetScreenWidth(), 20}, BLUE, true, TRIGGER_TYPE_INCREASE_LAVA_SPEED};
    triggers[4] = (Trigger) {{(0), -6130, GetScreenWidth(), 20}, BLUE, true, TRIGGER_TYPE_LAVA_SHUTDOWN};
    /* END TRIGGERS */

    /* INIT OF THE MAP */
    Texture2D staticBlock = LoadTexture("resources/textures/block_static.png");
    Texture2D crashingBlock1 = LoadTexture("resources/textures/block_crashing1.png");
    Texture2D crashingBlock2 = LoadTexture("resources/textures/block_crashing2.png");
    Texture2D movingBlock = LoadTexture("resources/textures/block_moving.png");

    Texture2D topBackground = LoadTexture("resources/textures/wall_level_top.png");
    Texture2D wallLeft = LoadTexture("resources/textures/wall_left.png");
    Texture2D wallRight = LoadTexture("resources/textures/wall_right.png");
    Texture2D wallBackground = LoadTexture("resources/textures/wall_background.png");
    Texture2D wallBottom = LoadTexture("resources/textures/wall_bottom.png");

    int blockHeight = 20;
    int blockWidth = 100;


    // Broken Block
    envItems[0] = (EnvItem) {staticBlock, {0, 0, 0, 0}, {false, 0}, 0, false, 0, BLANK};

    // Backgrounds
    envItems[1] = (EnvItem) {topBackground,  {0,                  -6438,               screenWidth, 308},          {false, 0}, 0, false, 1, RAYWHITE};
    envItems[2] = (EnvItem) {wallBackground, {100,                -6130,               600, 6830}, {false, 0}, 0, false, 1, RAYWHITE};
    envItems[3] = (EnvItem) {wallBottom,     {0,                  800,                screenWidth, 100},          {false, 0}, 1, false, 1, RAYWHITE};
    envItems[4] = (EnvItem) {wallLeft,       {screenHeight-800, -6130,                100,         100},          {false, 0}, 1, false, 1, RAYWHITE};
    envItems[5] = (EnvItem) {wallRight,      {screenHeight-100, -6130,                100,         100},          {false, 0}, 1, false, 1, RAYWHITE};
    envItems[6] = (EnvItem) {staticBlock, {500, -6438, blockWidth, blockHeight}, {false, 0}, 1, false, 1, BLANK};

    // LEVEL1
    envItems[7] = (EnvItem) {staticBlock,    {180,                700,                blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE}; // 1
    envItems[8] = (EnvItem) {staticBlock,    {320,                600,                blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE};   // 2
    envItems[9] = (EnvItem) {staticBlock,    {500,                500,                blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE};   // 3
    envItems[10] = (EnvItem) {staticBlock,    {600,                500,                blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE};   // 3
    envItems[11] = (EnvItem) {staticBlock,    {400,                370,                blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE};   // 4
    envItems[12] = (EnvItem) {staticBlock,    {250,                240,                blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE};   // 5
    envItems[13] = (EnvItem) {staticBlock,    {100,                110,                blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE};  // 6
    envItems[14] = (EnvItem) {staticBlock,    {250,                -20,                 blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE};   // 7
    envItems[15] = (EnvItem) {staticBlock,    {350,                -20,                 blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE};   // 7
    envItems[16] = (EnvItem) {staticBlock,    {450,                -20,                 blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE};   // 7
    envItems[17] = (EnvItem) {staticBlock,    {100,                -150,                blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE};  // 8
    envItems[18] = (EnvItem) {staticBlock,    {600,                -150,                blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE};  // 9
    envItems[19] = (EnvItem) {staticBlock,    {200,                -280,               blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE}; // 10
    envItems[20] = (EnvItem) {staticBlock,    {500,                -280,               blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE}; // 11
    envItems[21] = (EnvItem) {staticBlock,    {250,                -410,               blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE}; // 12
    envItems[22] = (EnvItem) {staticBlock,    {350,                -410,               blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE}; // 12
    envItems[23] = (EnvItem) {staticBlock,    {450,                -410,               blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE}; // 12
    envItems[24] = (EnvItem) {staticBlock,    {100,                -540,               blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE}; // 13
    envItems[25] = (EnvItem) {staticBlock,    {600,                -540,               blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE}; // 14
    envItems[26] = (EnvItem) {staticBlock,    {100,                -670,               blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE}; // 15
    envItems[27] = (EnvItem) {staticBlock,    {600,                -670,               blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE}; // 16
    envItems[28] = (EnvItem) {staticBlock,    {100,                -800,               blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE}; // 17
    envItems[29] = (EnvItem) {staticBlock,    {600,                -800,               blockWidth,  blockHeight},  {false, 0}, 1, false, 1, RAYWHITE}; // 18

    //LEVEL2
    envItems[30] = (EnvItem) {movingBlock, {200, -930, blockWidth, blockHeight}, {true, 2}, 1, false, 1, RAYWHITE}; // 19
    envItems[31] = (EnvItem) {staticBlock, {600, -1060, blockWidth, blockHeight}, {false, 0}, 1, false, 1, RAYWHITE}; // 20
    envItems[32] = (EnvItem) {movingBlock, {600, -1190, blockWidth, blockHeight}, {true, -2}, 1, false, 1, RAYWHITE}; // 21
    envItems[33] = (EnvItem) {staticBlock, {100, -1320, blockWidth, blockHeight}, {false, 0}, 1, false, 1, RAYWHITE}; // 22
    envItems[34] = (EnvItem) {movingBlock, {100, -1450, blockWidth, blockHeight}, {true,4}, 1, false, 1, RAYWHITE}; // 23
    envItems[35] = (EnvItem) {staticBlock, {350, -1580, blockWidth, blockHeight}, {false, 0}, 1, false, 1, RAYWHITE}; // 24
    envItems[36] = (EnvItem) {movingBlock, {350, -1710, blockWidth, blockHeight}, {true, 2}, 1, false, 1, RAYWHITE}; // 25
    envItems[37] = (EnvItem) {movingBlock, {350, -1840, blockWidth, blockHeight}, {true, -4}, 1, false, 1, RAYWHITE}; // 26
    envItems[38] = (EnvItem) {staticBlock, {250, -1970, blockWidth, blockHeight}, {false, 0}, 1, false, 1, RAYWHITE}; // 27
    envItems[39] = (EnvItem) {staticBlock, {450, -1970, blockWidth, blockHeight}, {false, 0}, 1, false, 1, RAYWHITE}; // 28
    envItems[40] = (EnvItem) {movingBlock, {350, -2100, blockWidth, blockHeight}, {true, 4}, 1, false, 1, RAYWHITE}; // 29
    envItems[41] = (EnvItem) {movingBlock, {450, -2230, blockWidth, blockHeight}, {true, -4}, 1, false, 1, RAYWHITE}; // 30
    envItems[42] = (EnvItem) {movingBlock, {200, -2360, blockWidth, blockHeight}, {true, 4}, 1, false, 1, RAYWHITE}; // 31
    envItems[43] = (EnvItem) {staticBlock, {100, -2490, blockWidth, blockHeight}, {false, 0}, 1, false, 1, RAYWHITE}; // 32
    envItems[44] = (EnvItem) {staticBlock, {300, -2490, blockWidth, blockHeight}, {false, 0}, 1, false, 1, RAYWHITE}; // 33
    envItems[45] = (EnvItem) {staticBlock, {400, -2490, blockWidth, blockHeight}, {false, 0}, 1, false, 1, RAYWHITE}; // 33
    envItems[46] = (EnvItem) {staticBlock, {600, -2490, blockWidth, blockHeight}, {false, 0}, 1, false, 1, RAYWHITE}; // 34

    //LEVEL3
    envItems[47] = (EnvItem) {crashingBlock1, {100, -2620, blockWidth, blockHeight}, {false, 0}, 1, true, 3,RAYWHITE}; // 35
    envItems[48] = (EnvItem) {crashingBlock1, {600, -2620, blockWidth, blockHeight}, {false, 0}, 1, true, 3, RAYWHITE}; //36
    envItems[49] = (EnvItem) {movingBlock, {100, -2750, blockWidth, blockHeight}, {true, 4}, 1, false, 1, RAYWHITE}; //37
    envItems[50] = (EnvItem) {staticBlock, {350, -2880, blockWidth, blockHeight}, {false, 0}, 1, false, 1, RAYWHITE}; //38
    envItems[51] = (EnvItem) {crashingBlock1, {200, -3010, blockWidth, blockHeight}, {false, 0}, 1, true, 3, RAYWHITE}; //39
    envItems[52] = (EnvItem) {crashingBlock1, {500, -3010, blockWidth, blockHeight}, {false, 0}, 1, true, 3, RAYWHITE}; //40
    envItems[53] = (EnvItem) {movingBlock, {350, -3140, blockWidth, blockHeight}, {true, -4}, 1, false, 1, RAYWHITE}; //41
    envItems[54] = (EnvItem) {crashingBlock1, {100, -3270, blockWidth, blockHeight}, {false, 0}, 1, true, 3, RAYWHITE}; //42
    envItems[55] = (EnvItem) {crashingBlock1, {600, -3270, blockWidth, blockHeight}, {false, 0}, 1, true, 3, RAYWHITE}; //43
    envItems[56] = (EnvItem) {staticBlock, {350, -3400, blockWidth, blockHeight}, {false, 0}, 1, false, 1, RAYWHITE}; //44
    envItems[57] = (EnvItem) {crashingBlock1, {200, -3530, blockWidth, blockHeight}, {false, 0}, 1, true, 3, RAYWHITE}; //45
    envItems[58] = (EnvItem) {crashingBlock1, {350, -3660, blockWidth, blockHeight}, {false, 0}, 1, true, 3, RAYWHITE}; //46
    envItems[59] = (EnvItem) {crashingBlock1, {500, -3790, blockWidth, blockHeight}, {false, 0}, 1, true, 3, RAYWHITE}; //47
    envItems[60] = (EnvItem) {crashingBlock1, {350, -3920, blockWidth, blockHeight}, {false, 0}, 1, true, 3, RAYWHITE}; //48
    envItems[61] = (EnvItem) {crashingBlock1, {200, -4050, blockWidth, blockHeight}, {false, 0}, 1, true, 3, RAYWHITE}; //49

    // LEVEL4
    envItems[62] = (EnvItem) {crashingBlock2, {350, -4180, blockWidth, blockHeight}, {false, 0}, 1, true, 1, RAYWHITE}; //50
    envItems[63] = (EnvItem) {crashingBlock1, {350, -4310, blockWidth, blockHeight}, {false, 0}, 1, true, 3, RAYWHITE}; //51
    envItems[64] = (EnvItem) {crashingBlock2, {500, -4440, blockWidth, blockHeight}, {false, 0}, 1, true, 1, RAYWHITE}; //52
    envItems[65] = (EnvItem) {crashingBlock2, {200, -4440, blockWidth, blockHeight}, {false, 0}, 1, true, 1, RAYWHITE}; //53
    envItems[66] = (EnvItem) {crashingBlock1, {100, -4570, blockWidth, blockHeight}, {false, 0}, 1, true, 3, RAYWHITE}; //54
    envItems[67] = (EnvItem) {crashingBlock1, {600, -4570, blockWidth, blockHeight}, {false, 0}, 1, true, 3, RAYWHITE};  //55
    envItems[68] = (EnvItem) {movingBlock, {100, -4700, blockWidth, blockHeight}, {true, 4}, 1, false, 1, RAYWHITE}; //56
    envItems[69] = (EnvItem) {crashingBlock2, {350, -4830, blockWidth, blockHeight}, {false, 0}, 1, true, 1, RAYWHITE}; //57
    envItems[70] = (EnvItem) {crashingBlock2, {350, -4960, blockWidth, blockHeight}, {false, 0}, 1, true, 1, RAYWHITE}; //58
    envItems[71] = (EnvItem) {movingBlock, {600, -5090, blockWidth, blockHeight}, {true, -4}, 1, false, 1, RAYWHITE}; //59
    envItems[72] = (EnvItem) {crashingBlock2, {500, -5220, blockWidth, blockHeight}, {false, 0}, 1, true, 1, RAYWHITE}; //60
    envItems[73] = (EnvItem) {crashingBlock2, {300, -5350, blockWidth, blockHeight}, {false, 0}, 1, true, 1, RAYWHITE}; //61
    envItems[74] = (EnvItem) {crashingBlock2, {150, -5480, blockWidth, blockHeight}, {false, 0}, 1, true, 1, RAYWHITE}; //62
    envItems[75] = (EnvItem) {movingBlock, {350, -5610, blockWidth, blockHeight}, {true, 4}, 1, false, 1, RAYWHITE}; //63
    envItems[76] = (EnvItem) {crashingBlock2, {100, -5740, blockWidth, blockHeight}, {false, 0}, 1, true, 1, RAYWHITE}; //64
    envItems[77] = (EnvItem) {crashingBlock2, {300, -5870, blockWidth, blockHeight}, {false, 0}, 1, true, 1, RAYWHITE}; //65
    envItems[78] = (EnvItem) {crashingBlock2, {500, -6000, blockWidth, blockHeight}, {false, 0}, 1, true, 1, RAYWHITE}; //66

    /* END OF THE MAP */
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
