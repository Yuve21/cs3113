// A teaching demo, NOT the exercise. It shows one idea:
// a value that only ever counts up can drive something that oscillates forever.
#include "raylib.h"
#include <math.h>

constexpr int   SCREEN_W = 700, SCREEN_H = 400, FPS = 60;
constexpr float CENTRE = 80.0f;   // the size it sits at when not scaled
constexpr float SWING  = 40.0f;   // how far it grows/shrinks either side
constexpr float SPEED  = 0.05f;   // how fast it pulses

float gTime = 0.0f;               // ONLY ever goes up. Never reversed, never reset.
float gRadius = CENTRE;

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "sine demo - watch the numbers");
    SetTargetFPS(FPS);

    while (!WindowShouldClose()) {
        // ---- the whole idea, two lines ----
        gTime  += SPEED;                            // 1. climb, forever
        gRadius = CENTRE + SWING * sinf(gTime);     // 2. rebuild from scratch each frame
        // -----------------------------------

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV({SCREEN_W / 2.0f, SCREEN_H / 2.0f}, gRadius, ColorFromNormalized({0.97f, 0.51f, 0.47f, 1.0f}));

        DrawText(TextFormat("gTime   = %.2f   (always climbing)", gTime), 20, 20, 20, DARKGRAY);
        DrawText(TextFormat("sinf()  = %+.3f  (bounces -1 to +1)", sinf(gTime)), 20, 50, 20, DARKGRAY);
        DrawText(TextFormat("gRadius = %.2f   (%.0f +/- %.0f)", gRadius, CENTRE, SWING), 20, 80, 20, DARKGRAY);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
