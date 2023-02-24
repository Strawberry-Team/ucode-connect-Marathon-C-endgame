
#include "../inc/main.h"

#define G 680
#define PLAYER_JUMP_SPD 520.0f
#define PLAYER_HOR_SPD 220.0f
#define PLAYER_JUMP_LIMIT 2
#define LAVA_SPEED 25
#define LAVA_SPEED_MULTIPLIER 1.3
#define NUM_FRAMES 3

const int screenWidth = 800;
const int screenHeight = 800;
typedef enum GameScreen {START = 0, GAMEPLAY, ENDING, WIN, EXIT } GameScreen;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------



    InitWindow(screenWidth, screenHeight, "Endgame");
    Texture2D end_player1 = LoadTexture("resources/textures/end_player1.png");        // Texture loading
    Texture2D end_player2 = LoadTexture("resources/textures/end_player2.png");
    Texture2D end_player3 = LoadTexture("resources/textures/end_player3.png");
    Texture2D end_player4 = LoadTexture("resources/textures/end_player4.png");
    Texture2D end_player_temp = end_player1;
    int status_load = 1;
    int framesCounterforLoad = 0;
    Texture2D win_player1 = LoadTexture("resources/textures/win_player1.png");        // Texture loading
    Texture2D win_player2 = LoadTexture("resources/textures/win_player2.png");
    Texture2D win_player3 = LoadTexture("resources/textures/win_player3.png");
    Texture2D win_player4 = LoadTexture("resources/textures/win_player4.png");
    Texture2D win_player_temp = win_player1;

    InitAudioDevice();
    Music StartMusic = LoadMusicStream("resources/textures/StartMusic.mp3");
    Music MainMusic = LoadMusicStream("resources/textures/MainMusic.mp3");
    Music WaitingMusic = LoadMusicStream("resources/textures/WaitingMusic.mp3");
    Music GameOver = LoadMusicStream("resources/textures/GameOver.mp3");
    Music Win = LoadMusicStream("resources/textures/Win.mp3");
    Music temp_music = StartMusic;
    PlayMusicStream(temp_music);
    float volume = 0.5f;

    bool turn_music = false;
    float timePlayed = 0.0f;

    GameScreen currentScreen = START;
    Texture2D start_title = LoadTexture("resources/textures/start_title.png");
    Texture2D title_intro = LoadTexture("resources/textures/title_intro.png");
    Texture2D s_button = LoadTexture("resources/textures/buttons/button_start.png"); // Load button texture
    Texture2D e_button = LoadTexture("resources/textures/buttons/button_exit.png");   // Load button texture
    Texture2D r_button = LoadTexture("resources/textures/buttons/button_retry.png"); // Load button texture
    Texture2D b_button = LoadTexture("resources/textures/buttons/button_back.png");   // Load button texture
    Texture2D e_end_button = LoadTexture("resources/textures/buttons/button_exit.png");   // Load button texture
    float frameHeight = (float)s_button.height / NUM_FRAMES;
    float e_frameHeight = (float)e_button.height / NUM_FRAMES;
    float r_frameHeight = (float)r_button.height / NUM_FRAMES;
    float b_frameHeight = (float)b_button.height / NUM_FRAMES;
    float e_end_frameHeight = (float)e_end_button.height / NUM_FRAMES;
    Rectangle sourceRec = {0, 0, (float)s_button.width, frameHeight};
    Rectangle e_sourceRec = {0, 0, (float)e_button.width, e_frameHeight};
    Rectangle r_sourceRec = {0, 0, (float)r_button.width, r_frameHeight};
    Rectangle b_sourceRec = {0, 0, (float)b_button.width, b_frameHeight};
    Rectangle e_end_sourceRec = {0, 0, (float)e_end_button.width, e_end_frameHeight};
    // Define button bounds on screen
   // Rectangle startBounds = {screenWidth / 8.0f - s_button.width / 2.0f + 500, screenHeight / 5.0f - s_button.height / NUM_FRAMES / 2.0f , (float)s_button.width, frameHeight};
    Rectangle startBounds = {500, 100 , (float)s_button.width, frameHeight};
    //Rectangle exitBounds = {screenWidth / 8.0f - e_button.width / 2.0f + 500, screenHeight / 2.0f - e_button.height / NUM_FRAMES / 2.0f - 100, (float)e_button.width, e_frameHeight};
    Rectangle exitBounds = {500, 250, (float)e_button.width, e_frameHeight};
    Rectangle retryBounds = {screenWidth / 6.0f - r_button.width / 2.0f, 700 , (float)r_button.width, r_frameHeight};
    Rectangle backBounds = {screenWidth / 2.0f - b_button.width / 2.0f, 700, (float)b_button.width, b_frameHeight};
    Rectangle exit_endBounds = {screenWidth / 1.2f - b_button.width / 2.0f, 700, (float)e_end_button.width, e_end_frameHeight};
    //-----------------------------------------------------------------------------
    int btnState = 0;       // Button state: 0-NORMAL, 1-MOUSE_HOVER, 2-PRESSED
    bool btnAction = false; // Button action should be activated

    int e_btnState = 0;       // Button state: 0-NORMAL, 1-MOUSE_HOVER, 2-PRESSED
    bool e_btnAction = false; // Button action should be activated

    int r_btnState = 0;       // Button state: 0-NORMAL, 1-MOUSE_HOVER, 2-PRESSED
    bool r_btnAction = false; // Button action should be activated

    int b_btnState = 0;       // Button state: 0-NORMAL, 1-MOUSE_HOVER, 2-PRESSED
    bool b_btnAction = false; // Button action should be activated

    int e_end_btnState = 0;       // Button state: 0-NORMAL, 1-MOUSE_HOVER, 2-PRESSED
    bool e_end_btnAction = false; // Button action should be activated

    //----------------------------------------------
    Vector2 mousePoint = {0.0f, 0.0f};
    int button_status = 0;
    int lettersCount = 0;




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
    Rectangle playerRect;
    //Rectangle playerRect = {player.position.x - frameRec.width, player.position.y - frameRec.height, frameRec.width, frameRec.height};

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
        switch(currentScreen)
        {
            case START:
            {
                UpdateMusicStream(temp_music);
                if (IsKeyPressed(KEY_P)) {
                    turn_music = !turn_music;

                    if (turn_music) SetMasterVolume(0);
                    else SetMasterVolume(volume);
                }
                if (IsKeyDown(KEY_MINUS) && volume > 0.0f) volume -= 0.1f;
                else if (IsKeyDown(KEY_EQUAL) && volume < 1.0f) volume += 0.1f;
                SetMusicVolume(temp_music, volume);
               // if (IsKeyPressed(KEY_ENTER)){ //|| IsGestureDetected(GESTURE_TAP)) {
               //     currentScreen = GAMEPLAY;
               // }
                mousePoint = GetMousePosition();
                btnAction = false;
                e_btnAction = false;

                // Check button state
                if (CheckCollisionPointRec(mousePoint, startBounds))
                {
                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                    {
                        btnState = 2;
                    }
                    else
                    {
                        btnState = 1;
                    }

                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                    {
                        btnAction = true;
                        button_status = 1;
                    }
                }
                else if (CheckCollisionPointRec(mousePoint, exitBounds))
                {
                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                    {
                        e_btnState = 2;
                    }
                    else
                    {
                        e_btnState = 1;
                    }

                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                    {
                        e_btnAction = true;
                        button_status = 2;
                    }
                }
                else
                {
                    btnState = 0;
                    e_btnState = 0;
                }

                if (btnAction && button_status == 1)
                {
                    StopMusicStream(temp_music);
                    temp_music = WaitingMusic;
                    PlayMusicStream(temp_music);
                    currentScreen = GAMEPLAY;
                    // TODO: Any desired action
                }
                else if (e_btnAction && button_status == 2)
                {
                    currentScreen = EXIT;
                }

                // Calculate button frame rectangle to draw depending on button state
                sourceRec.y = btnState * frameHeight;
                e_sourceRec.y = e_btnState * e_frameHeight;

                // Press enter to change to GAMEPLAY screen
                // if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                // {
                //     currentScreen = GAMEPLAY;
                // }
            } break;
            case GAMEPLAY:
            {
                UpdateMusicStream(temp_music);
                if (IsKeyPressed(KEY_P)) {
                    turn_music = !turn_music;

                    if (turn_music) SetMasterVolume(0);
                    else SetMasterVolume(volume);
                }
                if (IsKeyDown(KEY_MINUS) && volume > 0.0f) volume -= 0.1f;
                else if (IsKeyDown(KEY_EQUAL) && volume < 1.0f) volume += 0.1f;
                if (IsKeyDown(KEY_N)) {
                    StopMusicStream(temp_music);
                    temp_music = MainMusic;
                    PlayMusicStream(temp_music);
                }
                SetMusicVolume(temp_music, volume);
        // Update the timer variables
        float currentFrameTime = GetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // Update the position of the lava object
        lava.y -= LAVA_SPEED * deltaTime * lavaSpeedMultiplier;

        playerRect.x = player.position.x - frameRec.width;
        playerRect.y = player.position.y - frameRec.height;
        playerRect.width = frameRec.width;
        playerRect.height =frameRec.height;

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
            }  break;
            case ENDING:
            {
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
                r_btnAction = false;
                b_btnAction = false;
                e_end_btnAction = false;
                // Check button state
                if (CheckCollisionPointRec(mousePoint, retryBounds))
                {
                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                    {
                        r_btnState = 2;
                    }
                    else
                    {
                        r_btnState = 1;
                    }

                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                    {
                        r_btnAction = true;
                        button_status = 3;
                    }
                }
                else if (CheckCollisionPointRec(mousePoint, backBounds))
                {
                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                    {
                        b_btnState = 2;
                    }
                    else
                    {
                        b_btnState = 1;
                    }

                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                    {
                        b_btnAction = true;
                        button_status = 4;
                    }
                }
                else if (CheckCollisionPointRec(mousePoint, exit_endBounds))
                {
                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                    {
                        e_end_btnState = 2;
                    }
                    else
                    {
                        e_end_btnState = 1;
                    }

                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                    {
                        e_end_btnAction = true;
                        button_status = 5;
                    }
                }
                else
                {
                    r_btnState = 0;
                    b_btnState = 0;
                    e_end_btnState = 0;
                }

                if (r_btnAction && button_status == 3)
                {
                    StopMusicStream(temp_music);
                    temp_music = WaitingMusic;
                    PlayMusicStream(temp_music);
                    currentScreen = GAMEPLAY;
                    // TODO: Any desired action
                }
                else if (b_btnAction && button_status == 4)
                {
                    StopMusicStream(temp_music);
                    temp_music = StartMusic;
                    PlayMusicStream(temp_music);
                    currentScreen = START;
                }
                else if (e_end_btnAction && button_status == 5)
                {
                    currentScreen = EXIT;
                }

                // Calculate button frame rectangle to draw depending on button state
                r_sourceRec.y = r_btnState * r_frameHeight;
                b_sourceRec.y = b_btnState * b_frameHeight;
                e_end_sourceRec.y = e_end_btnState * e_end_frameHeight;
                if (button_status == 3 || button_status == 4)
                {
                    //lava.x = 100;
                    lava.y = 1000;
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
                if(status_load != 6) {
                    framesCounterforLoad++;
                    //UpdateMusicStream(temp_music);
                     //Every two seconds (120 frames) a new random value is generated
                    if (((framesCounterforLoad / 30) % 2) == 1 && status_load == 1) {
                        end_player_temp = end_player1;
                        status_load = 2;
                        framesCounterforLoad = 0;
                    } else if (((framesCounterforLoad / 30) % 2) == 1 && status_load == 2) {
                        end_player_temp = end_player2;
                        status_load = 3;
                        framesCounterforLoad = 0;
                    } else if (((framesCounterforLoad / 30) % 2) == 1 && status_load == 3) {
                        end_player_temp = end_player3;
                        status_load = 4;
                        framesCounterforLoad = 0;
                    } else if (((framesCounterforLoad / 30) % 2) == 1 && status_load == 4) {
                        end_player_temp = end_player4;
                        status_load = 5;
                        framesCounterforLoad = 0;
                    } else if (((framesCounterforLoad / 30) % 2) == 1 && status_load == 5)
                        status_load = 6;
                }

            } break;
            case WIN:
            {
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
                r_btnAction = false;
                b_btnAction = false;
                e_end_btnAction = false;
                // Check button state
                if (CheckCollisionPointRec(mousePoint, retryBounds))
                {
                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                    {
                        r_btnState = 2;
                    }
                    else
                    {
                        r_btnState = 1;
                    }

                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                    {
                        r_btnAction = true;
                        button_status = 3;
                    }
                }
                else if (CheckCollisionPointRec(mousePoint, backBounds))
                {
                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                    {
                        b_btnState = 2;
                    }
                    else
                    {
                        b_btnState = 1;
                    }

                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                    {
                        b_btnAction = true;
                        button_status = 4;
                    }
                }
                else if (CheckCollisionPointRec(mousePoint, exit_endBounds))
                {
                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                    {
                        e_end_btnState = 2;
                    }
                    else
                    {
                        e_end_btnState = 1;
                    }

                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                    {
                        e_end_btnAction = true;
                        button_status = 5;
                    }
                }
                else
                {
                    r_btnState = 0;
                    b_btnState = 0;
                    e_end_btnState = 0;
                }

                if (r_btnAction && button_status == 3)
                {
                    StopMusicStream(temp_music);
                    temp_music = WaitingMusic;
                    PlayMusicStream(temp_music);
                    currentScreen = GAMEPLAY;
                    // TODO: Any desired action
                }
                else if (b_btnAction && button_status == 4)
                {
                    StopMusicStream(temp_music);
                    temp_music = StartMusic;
                    PlayMusicStream(temp_music);
                    currentScreen = START;
                }
                else if (e_end_btnAction && button_status == 5)
                {
                    currentScreen = EXIT;
                }

                // Calculate button frame rectangle to draw depending on button state
                r_sourceRec.y = r_btnState * r_frameHeight;
                b_sourceRec.y = b_btnState * b_frameHeight;
                e_end_sourceRec.y = e_end_btnState * e_end_frameHeight;
                if (button_status == 3 || button_status == 4)
                {
                    //lava.x = 100;
                    lava.y = 1000;
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
                if(status_load != 6) {
                    framesCounterforLoad++;
                    //UpdateMusicStream(temp_music);
                    //Every two seconds (120 frames) a new random value is generated
                    if (((framesCounterforLoad / 30) % 2) == 1 && status_load == 1) {
                        win_player_temp = win_player1;
                        status_load = 2;
                        framesCounterforLoad = 0;
                    } else if (((framesCounterforLoad / 30) % 2) == 1 && status_load == 2) {
                        win_player_temp = win_player2;
                        status_load = 3;
                        framesCounterforLoad = 0;
                    } else if (((framesCounterforLoad / 30) % 2) == 1 && status_load == 3) {
                        win_player_temp = win_player3;
                        status_load = 4;
                        framesCounterforLoad = 0;
                    } else if (((framesCounterforLoad / 30) % 2) == 1 && status_load == 4) {
                        win_player_temp = win_player4;
                        status_load = 5;
                        framesCounterforLoad = 0;
                    } else if (((framesCounterforLoad / 30) % 2) == 1 && status_load == 5)
                        status_load = 6;
                }
            }
                break;
            case EXIT:
            {
                return 0;
            }
                break;
            default: break;
        }
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        switch(currentScreen)
        {
            case START: {
                DrawTexture(start_title,0 , 0 , WHITE );
                //DrawText("TITLE SCREEN", 20, 20, 40, DARKGREEN);
               // DrawText("PRESS ENTER or TAP to JUMP to GAMEPLAY SCREEN", 120, 220, 20, DARKGREEN);
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
                DrawTextureRec(s_button, sourceRec, (Vector2){startBounds.x, startBounds.y}, WHITE); // Draw button frame
                DrawTextureRec(e_button, e_sourceRec, (Vector2){exitBounds.x, exitBounds.y}, WHITE); // Draw button frame
                // DrawText("ENTER SPACE TO START GAME", 290, 220, 20, GRAY);

            } break;
            case GAMEPLAY:
            {
                UpdateMusicStream(temp_music);
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
            StopMusicStream(temp_music);
            temp_music = GameOver;
            PlayMusicStream(temp_music);
            currentScreen = ENDING;;
        }
                mousePoint = GetMousePosition();
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) ) {
            StopMusicStream(temp_music);
                    temp_music = Win;
                    PlayMusicStream(temp_music);
                    currentScreen = WIN;;
        }

        //DrawRectangleRec(playerRect, RED);
        DrawRectangleRec(lava, RED);



        DrawTextureRec(scarfy, frameRec, (Vector2) { playerRect.x, playerRect.y}, WHITE);
        //DrawTextureRec(scarfy, frameRec, scarfyPosition, WHITE);

                EndMode2D();
            }  break;
            case ENDING:
            {
                DrawTexture(title_intro,0 , 0 , WHITE );
                //DrawRectangle(0, 0, screenWidth, screenHeight, SKYBLUE);
               // DrawText("TITLE SCREEN", 20, 20, 40, DARKGREEN);
                //DrawText("PRESS ENTER or TAP to JUMP to GAMEPLAY SCREEN", 120, 220, 20, DARKGREEN);
                DrawTexture(end_player_temp, screenWidth/2 - end_player_temp.width/2 , screenHeight/2 - end_player_temp.height/2 + 103, WHITE);
                DrawText(TextSubtext("Game Over", 0, lettersCount), GetScreenWidth()/2 - 250, GetScreenHeight()/2 - 200, 100, BLACK);
                DrawTextureRec(r_button, r_sourceRec, (Vector2){retryBounds.x, retryBounds.y}, WHITE); // Draw button frame
                DrawTextureRec(b_button, b_sourceRec, (Vector2){backBounds.x, backBounds.y}, WHITE); // Draw button frame
                DrawTextureRec(e_end_button, e_end_sourceRec, (Vector2){exit_endBounds.x, exit_endBounds.y}, WHITE); // Draw button frame
            } break;
            case WIN:
            {
                DrawTexture(title_intro,0 , 0 , WHITE );
                //DrawRectangle(0, 0, screenWidth, screenHeight, SKYBLUE);
                // DrawText("TITLE SCREEN", 20, 20, 40, DARKGREEN);
                //DrawText("PRESS ENTER or TAP to JUMP to GAMEPLAY SCREEN", 120, 220, 20, DARKGREEN);
                DrawTexture(win_player_temp, screenWidth/2 - win_player_temp.width/2 , screenHeight/2 - win_player_temp.height/2 + 103, WHITE);
                DrawText(TextSubtext("You Win", 0, lettersCount), GetScreenWidth()/2 - 220, GetScreenHeight()/2 - 200, 100, BLACK);
                DrawTextureRec(r_button, r_sourceRec, (Vector2){retryBounds.x, retryBounds.y}, WHITE); // Draw button frame
                DrawTextureRec(b_button, b_sourceRec, (Vector2){backBounds.x, backBounds.y}, WHITE); // Draw button frame
                DrawTextureRec(e_end_button, e_end_sourceRec, (Vector2){exit_endBounds.x, exit_endBounds.y}, WHITE); // Draw button frame
            } break;
            case EXIT:
            {
                UnloadMusicStream(StartMusic);
                UnloadMusicStream(MainMusic); // Unload music stream buffers from RAM
                UnloadMusicStream(GameOver); // Unload music stream buffers from RAM
                CloseAudioDevice();     // Close audio device (music streaming is automatically stopped)
                EndDrawing();
                CloseWindow(); // Close window and OpenGL context
                return 0;
            }
            default: break;
        }
        //EndMode2D();
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    UnloadMusicStream(StartMusic);
    UnloadMusicStream(MainMusic); // Unload music stream buffers from RAM
    UnloadMusicStream(GameOver); // Unload music stream buffers from RAM
    CloseAudioDevice();     // Close audio device (music streaming is automatically stopped)
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

    if (!hitObstacle) {
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
