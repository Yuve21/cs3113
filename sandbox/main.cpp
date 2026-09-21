#include "raylib.h"

// Enums
enum AppStatus { TERMINATED, RUNNING };

// Global Constants
constexpr int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 450, FPS = 60;

// Global Variables

// Global Variables
AppStatus gAppStatus = RUNNING;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Transformations");

    SetTargetFPS(FPS);
}

void processInput() {
    if (WindowShouldClose())
        gAppStatus = TERMINATED;
}

void update() {}

void render() {
    BeginDrawing();

    DrawPoly({SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, 3, 100, 0.0f, RED);

    ClearBackground(RAYWHITE);

    EndDrawing();
}

void shutdown() { CloseWindow(); }

int main(void) {
    initialise();

    while (gAppStatus == RUNNING) {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}
