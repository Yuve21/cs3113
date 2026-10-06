/**
* Author: Yuvraj Chandyok
* Assignment: Project 1 - Simple 2D Scene
* Date due: 10/05/2026
*
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
*
* Theme: Sun Wukong. The Destined One walks a figure-eight while two spirit
* orbs orbit him, spinning and pulsing.
*
* wukong.png is cropped from a Black Myth: Wukong promotional still,
* owned by Game Science. Used here for a non-commercial class project.
* The two orb textures are cropped from "Orbs Collection" by Lamoot
* (opengameart.org/content/orbs-collection), released under CC0.
**/

#include "CS3113/cs3113.h"
#include <math.h>

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 60;

// the background alternates between these two (extra credit)
constexpr char BG_MIST[]  = "#5C6166",   // cold mountain mist
               BG_DUSK[]  = "#6B5F52";   // warm dusk haze

constexpr char WUKONG_FP[] = "assets/game/wukong.png",
               JADE_FP[]   = "assets/game/orb_jade.png",
               AMBER_FP[]  = "assets/game/orb_amber.png";

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr Vector2 WUKONG_SIZE = { 274.0f, 300.0f },
                  JADE_SIZE   = { 74.0f, 74.0f },
                  AMBER_SIZE  = { 90.0f, 90.0f };

// WUKONG walks a figure-eight: y cycles at twice the rate of x
constexpr float WUKONG_SPAN_X = 250.0f,
                WUKONG_SPAN_Y = 70.0f,
                WUKONG_SPEED  = 0.75f;

// JADE orbs a clean circle around him and spins one way
constexpr float JADE_RADIUS = 170.0f,
                JADE_SPEED  = 2.3f,
                JADE_SPIN   = 150.0f;   // degrees per second

// AMBER rides a wider, slower ellipse and spins the other way while pulsing
constexpr float AMBER_RADIUS_X = 215.0f,
                AMBER_RADIUS_Y = 128.0f,
                AMBER_SPEED    = 1.4f,
                AMBER_SPIN     = -95.0f,
                AMBER_PULSE    = 26.0f,
                AMBER_PULSE_SPEED = 3.2f;

// Global Variables
AppStatus gAppStatus = RUNNING;

float gPreviousTicks = 0.0f,
      gElapsed       = 0.0f,    // the one ever-climbing value every pattern reads
      gJadeAngle     = 0.0f,    // each orb's own spin, in degrees
      gAmberAngle    = 0.0f;

Vector2 gWukongPosition = ORIGIN,
        gWukongScale    = WUKONG_SIZE,

        gJadePosition   = ORIGIN,
        gJadeScale      = JADE_SIZE,

        gAmberPosition  = ORIGIN,
        gAmberScale     = AMBER_SIZE;

Texture2D gWukongTexture;
Texture2D gJadeTexture;
Texture2D gAmberTexture;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
void renderObject(const Texture2D *texture, const Vector2 *position,
                  const Vector2 *scale, float rotation);

// Function Definitions
void renderObject(const Texture2D *texture, const Vector2 *position,
                  const Vector2 *scale, float rotation)
{
    Rectangle textureArea = {
        0.0f, 0.0f,
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    Vector2 objectOrigin = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    DrawTexturePro(*texture, textureArea, destinationArea, objectOrigin,
                   rotation, WHITE);
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 1 - Sun Wukong");

    gWukongTexture = LoadTexture(WUKONG_FP);
    gJadeTexture   = LoadTexture(JADE_FP);
    gAmberTexture  = LoadTexture(AMBER_FP);

    SetTargetFPS(FPS);
}

void processInput()
{
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    // delta time
    float ticks     = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    gElapsed += deltaTime;

    // WUKONG - figure-eight. Not up/down, not left/right.
    gWukongPosition.x = ORIGIN.x + WUKONG_SPAN_X * sinf(gElapsed * WUKONG_SPEED);
    gWukongPosition.y = ORIGIN.y + WUKONG_SPAN_Y * sinf(gElapsed * WUKONG_SPEED * 2.0f);

    // JADE ORB - circular orbit, positioned RELATIVE to Wukong
    gJadePosition.x = gWukongPosition.x + JADE_RADIUS * cosf(gElapsed * JADE_SPEED);
    gJadePosition.y = gWukongPosition.y + JADE_RADIUS * sinf(gElapsed * JADE_SPEED);

    // JADE ORB - spins on its own axis (ROTATION)
    gJadeAngle += JADE_SPIN * deltaTime;

    // AMBER ORB - a wider ellipse, slower, half a turn out of phase
    gAmberPosition.x = gWukongPosition.x
                       + AMBER_RADIUS_X * cosf(gElapsed * AMBER_SPEED + 3.14159f);
    gAmberPosition.y = gWukongPosition.y
                       + AMBER_RADIUS_Y * sinf(gElapsed * AMBER_SPEED + 3.14159f);

    // AMBER ORB - spins the other way (ROTATION)
    gAmberAngle += AMBER_SPIN * deltaTime;

    // AMBER ORB - pulses (SCALING)
    gAmberScale.x = AMBER_SIZE.x + AMBER_PULSE * sinf(gElapsed * AMBER_PULSE_SPEED);
    gAmberScale.y = AMBER_SIZE.y + AMBER_PULSE * sinf(gElapsed * AMBER_PULSE_SPEED);
}

void render()
{
    BeginDrawing();

    // EXTRA CREDIT - the background follows a sine pattern too
    if (sinf(gElapsed * 0.8f) > 0.0f) ClearBackground(ColorFromHex(BG_MIST));
    else                              ClearBackground(ColorFromHex(BG_DUSK));

    renderObject(&gAmberTexture,  &gAmberPosition,  &gAmberScale,  gAmberAngle);
    renderObject(&gWukongTexture, &gWukongPosition, &gWukongScale, 0.0f);
    renderObject(&gJadeTexture,   &gJadePosition,   &gJadeScale,   gJadeAngle);

    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gWukongTexture);
    UnloadTexture(gJadeTexture);
    UnloadTexture(gAmberTexture);

    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}
