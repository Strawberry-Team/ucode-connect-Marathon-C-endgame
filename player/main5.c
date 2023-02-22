#include "raylib.h"
#include "raymath.h"

#include <stdbool.h>

#define GRAVITY 2.0f
#define JUMP_HEIGHT 50.0f

bool isTextureValid(const Texture2D *texture) {
	return texture->id > 0;
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 720;
    const int screenHeight = 500;
    
    const int scarfySpeed = 5;

    InitWindow(screenWidth, screenHeight, "RayLib - 2D Character Animation");
   
    const char *filename = "111.png";
    Texture2D scarfy = LoadTexture(filename);
    if(!isTextureValid(&scarfy)) {
        
        while (!WindowShouldClose()) {
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText(TextFormat("ERROR: Couldn't load %s.", filename), 20, 20, 20, BLACK);
            EndDrawing();
        }
        return 10;
    }
    
    unsigned numFrames = 3;
    int frameWidth = scarfy.width / numFrames;
    Rectangle frameRec = { 0.0f, 0.0f, (float)frameWidth, (float)scarfy.height };
    Vector2 scarfyPosition = {screenWidth / 2.0f, screenHeight / 2.0f};
    Vector2 scarfyVelocity = {0.0f,0.0f};
    
    unsigned frameDelay = 2;
    unsigned frameDelayCounter = 0;
    unsigned frameIndex = 0;

    bool isJumping = false;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyDown(KEY_RIGHT) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
            scarfyVelocity.x = scarfySpeed;
            if(frameRec.width < 0) {
                frameRec.width = -frameRec.width;
            }
        } else if (IsKeyDown(KEY_LEFT) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
            scarfyVelocity.x = -scarfySpeed;
            if(frameRec.width > 0) {
                frameRec.width = -frameRec.width;
            }
        } else {
            scarfyVelocity.x = 0;
        }

        if (IsKeyPressed(KEY_SPACE) && !isJumping) {
            scarfyVelocity.y = -JUMP_HEIGHT;
            isJumping = true;
        }

        bool scarfyMoving = scarfyVelocity.x != 0.0f || scarfyVelocity.y != 0.0f;
        
        scarfyVelocity.y += GRAVITY;
        scarfyPosition = Vector2Add(scarfyPosition, scarfyVelocity);
        
        if (scarfyPosition.y >= screenHeight - scarfy.height) {
            scarfyVelocity.y = 0;
            scarfyPosition.y = screenHeight - scarfy.height;
            isJumping = false;
        }

        ++frameDelayCounter;
        if(frameDelayCounter > frameDelay) {
            frameDelayCounter = 0;
            
            if(scarfyMoving) {
                ++frameIndex;
                frameIndex %= numFrames;
                frameRec.x = (float) frameWidth * frameIndex;
            }
        }
        //----------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
			
			DrawTextureRec(scarfy, frameRec, scarfyPosition, WHITE);
            

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    // TODO: Unload all loaded resources at this point

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
