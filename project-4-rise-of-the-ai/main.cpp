/**
* Author: Yuvraj Chandyok
* Assignment: Rise of the AI
* Date due: 11/16/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

/*
    ALIEN ESCAPE

    Controls
        ENTER     start (menu) / back to the menu (win or lose screen)
        A / D     walk left / right
        W         jump
        1 / 2 / 3 jump straight to level 1, 2 or 3 (debugging)
        Q         quit

    Three lives for the whole game. Touching an enemy or falling into a pit
    costs one and restarts the level; reach the flag to clear it.

    Assets (all CC0):
        art    - "Platformer Art Deluxe" by Kenney (www.kenney.nl)
        music  - "Retro Game Music Pack" by Juhani Junkala
        sounds - "The Essential Retro Video Game Sound Effects Collection"
                 by Juhani Junkala
        font   - Kenvector Future by Kenney, used for the text images
*/

#include "CS3113/TextScene.h"
#include "CS3113/Level1.h"
#include "CS3113/Level2.h"
#include "CS3113/Level3.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120,
              MAX_LIVES     = 3;

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f,
                MUSIC_VOLUME   = 0.4f;

constexpr Vector2 HEART_SIZE      = { 40.0f, 34.0f },
                  FIRST_HEART_POS = { 40.0f, 35.0f };
constexpr float   HEART_SPACING   = 48.0f;

// Global Variables
AppStatus gAppStatus       = RUNNING;
float     gPreviousTicks   = 0.0f,
          gTimeAccumulator = 0.0f;

Camera2D gCamera = {};

Scene                     *gCurrentScene   = nullptr;
SceneID                    gCurrentSceneID = NO_SCENE;
SceneID                    gRequestedScene = NO_SCENE; // set by a key press
std::map<SceneID, Scene *> gScenes         = {};

int gLives = MAX_LIVES;   // for the whole game, not per level

Entity *gFullHearts  = nullptr;
Entity *gEmptyHearts = nullptr;

Music gMusic;
Sound gJumpSound,
      gHurtSound,
      gLevelClearSound,
      gWinSound,
      gLoseSound;

// Function Declarations
void switchToScene(SceneID id);
Vector2 cameraTargetFor(GameState state);
void handleSceneChanges();
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(SceneID id)
{
    if (gCurrentScene) gCurrentScene->shutdown();

    gCurrentSceneID = id;
    gCurrentScene   = gScenes[id];
    gCurrentScene->initialise();

    // jump straight to where the camera belongs; no panning in from elsewhere
    gCamera.target = cameraTargetFor(gCurrentScene->getState());
}

// levels: follow the player sideways, but stop at the map's edges so the
// camera never shows past the end of the level. Text scenes: the centre.
Vector2 cameraTargetFor(GameState state)
{
    if (state.player == nullptr) return ORIGIN;

    float leftmost  = state.map->getLeftBoundary()  + ORIGIN.x;
    float rightmost = state.map->getRightBoundary() - ORIGIN.x;
    float cameraX   = state.player->getPosition().x;

    if (cameraX < leftmost)  cameraX = leftmost;
    if (cameraX > rightmost) cameraX = rightmost;

    return { cameraX, ORIGIN.y };
}

// what happened this frame decides which scene comes next
void handleSceneChanges()
{
    SceneID   next  = gRequestedScene;
    GameState state = gCurrentScene->getState();

    if (next == NO_SCENE && state.playerDied)
    {
        gLives--;

        if (gLives <= 0)
        {
            next = LOSE;
            PlaySound(gLoseSound);
        }
        else
        {
            next = gCurrentSceneID;   // same level again, from the start
            PlaySound(gHurtSound);
        }
    }
    else if (next == NO_SCENE && state.nextSceneID != NO_SCENE)
    {
        next = state.nextSceneID;
        PlaySound(next == WIN ? gWinSound : gLevelClearSound);
    }

    gRequestedScene = NO_SCENE;

    if (next != NO_SCENE) switchToScene(next);
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Alien Escape");
    InitAudioDevice();

    gMusic = LoadMusicStream("assets/game/music.ogg");
    SetMusicVolume(gMusic, MUSIC_VOLUME);
    PlayMusicStream(gMusic);

    gJumpSound       = LoadSound("assets/game/jump.wav");
    gHurtSound       = LoadSound("assets/game/hurt.wav");
    gLevelClearSound = LoadSound("assets/game/level_clear.wav");
    gWinSound        = LoadSound("assets/game/win.wav");
    gLoseSound       = LoadSound("assets/game/lose.wav");

    /*
        ----------- SCENES -----------
    */
    gScenes[MENU]    = new TextScene(ORIGIN, "#163B4D", "assets/game/menu.png");
    gScenes[LEVEL_1] = new Level1(ORIGIN, "#D0F4F7", LEVEL_2);
    gScenes[LEVEL_2] = new Level2(ORIGIN, "#A8DDF0", LEVEL_3);
    gScenes[LEVEL_3] = new Level3(ORIGIN, "#3A2F4F", WIN);
    gScenes[WIN]     = new TextScene(ORIGIN, "#1F3D2A", "assets/game/win.png");
    gScenes[LOSE]    = new TextScene(ORIGIN, "#3D1F24", "assets/game/lose.png");

    /*
        ----------- LIVES (drawn on top of the level, not in it) -----------
    */
    gFullHearts  = new Entity[MAX_LIVES];
    gEmptyHearts = new Entity[MAX_LIVES];

    for (int i = 0; i < MAX_LIVES; i++)
    {
        Vector2 heartPosition = {
            FIRST_HEART_POS.x + i * HEART_SPACING, FIRST_HEART_POS.y
        };

        gFullHearts[i].setTexture("assets/game/heart_full.png");
        gFullHearts[i].setScale(HEART_SIZE);
        gFullHearts[i].setPosition(heartPosition);

        gEmptyHearts[i].setTexture("assets/game/heart_empty.png");
        gEmptyHearts[i].setScale(HEART_SIZE);
        gEmptyHearts[i].setPosition(heartPosition);
    }

    gCamera.offset   = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom     = 1.0f;

    switchToScene(MENU);

    SetTargetFPS(FPS);
}

void processInput()
{
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

    // ENTER: the menu starts a new game; the end screens go back to the menu.
    // Both put the lives back to full.
    if (IsKeyPressed(KEY_ENTER))
    {
        if (gCurrentSceneID == MENU)
        {
            gLives          = MAX_LIVES;
            gRequestedScene = LEVEL_1;
        }
        else if (gCurrentSceneID == WIN || gCurrentSceneID == LOSE)
        {
            gLives          = MAX_LIVES;
            gRequestedScene = MENU;
        }
    }

    // debugging: jump straight to a level
    if (IsKeyPressed(KEY_ONE))   gRequestedScene = LEVEL_1;
    if (IsKeyPressed(KEY_TWO))   gRequestedScene = LEVEL_2;
    if (IsKeyPressed(KEY_THREE)) gRequestedScene = LEVEL_3;

    // a debug jump out of the lose screen still needs a life to play with
    if (gRequestedScene != NO_SCENE && gRequestedScene != MENU && gLives <= 0)
        gLives = MAX_LIVES;

    Entity *player = gCurrentScene->getState().player;
    if (player == nullptr) return;   // text scenes have nobody to move

    player->resetMovement();

    if      (IsKeyDown(KEY_A)) player->moveLeft();
    else if (IsKeyDown(KEY_D)) player->moveRight();

    if (IsKeyPressed(KEY_W) && player->isCollidingBottom())
    {
        player->jump();
        PlaySound(gJumpSound);
    }

    // to avoid faster diagonal speed
    if (GetLength(player->getMovement()) > 1.0f) player->normaliseMovement();
}

void update()
{
    UpdateMusicStream(gMusic);   // the music keeps looping across every scene

    // Delta time
    float ticks     = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    // Fixed timestep
    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;

        Vector2 cameraTarget = cameraTargetFor(gCurrentScene->getState());
        panCamera(&gCamera, &cameraTarget);
    }

    gTimeAccumulator = deltaTime;
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCamera);

    gCurrentScene->render();

    EndMode2D();

    // the hearts are drawn after the camera, so they stay put on screen
    if (gCurrentScene->getState().player)
        for (int i = 0; i < MAX_LIVES; i++)
        {
            if (i < gLives) gFullHearts[i].render();
            else            gEmptyHearts[i].render();
        }

    EndDrawing();
}

void shutdown()
{
    for (std::pair<const SceneID, Scene *> &entry : gScenes)
        delete entry.second;

    gScenes.clear();

    delete[] gFullHearts;
    delete[] gEmptyHearts;

    UnloadMusicStream(gMusic);
    UnloadSound(gJumpSound);
    UnloadSound(gHurtSound);
    UnloadSound(gLevelClearSound);
    UnloadSound(gWinSound);
    UnloadSound(gLoseSound);

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        handleSceneChanges();
        render();
    }

    shutdown();

    return 0;
}
