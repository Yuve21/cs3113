/**
* Author: Yuvraj Chandyok
* Assignment: Lunar Lander!
* Date due: 10/30/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

/*
    Controls
        W       thrust up (against gravity)
        A / D   thrust left / right
        Q       quit

    Land on the moving green pad slowly enough and the mission is
    accomplished. Land too hard, hit the pad from the side or from below,
    hit a rock, or leave the screen and the mission fails. Every thruster
    burns fuel; at zero fuel they stop working.

    Assets: "Space Shooter (Redux, plus fonts and sounds)" by Kenney
    (www.kenney.nl), CC0. The thruster sprite sheet is three of its flame
    frames placed side by side; the banners use its Kenvector Future font.
*/

#include "CS3113/Entity.h"

enum GameStatus { READY, PLAYING, ACCOMPLISHED, FAILED };

struct GameState
{
    Entity *ship;
    Entity *flame;
    Entity *pad;
    Entity *rocks;
    Entity *space;

    Entity *fuelSlots;
    Entity *fuelCells;

    Entity *startBanner;
    Entity *accomplishedBanner;
    Entity *failedBanner;

    Sound winSound;
    Sound loseSound;
};

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120;

constexpr char    BG_COLOUR[] = "#2A1E3C";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// ----- the ship ----- //
constexpr Vector2 SHIP_INIT_POS = { 120.0f, 80.0f },
                  SHIP_SIZE     = { 66.0f, 50.0f },
                  SHIP_COLLIDER = { 48.0f, 40.0f };

constexpr float GRAVITY            = 45.0f,   // px/s/s, gentle, like the moon
                THRUST_UP          = 125.0f,  // px/s/s, more than gravity
                THRUST_SIDE        = 80.0f,   // px/s/s
                DRAG               = 2.0f,    // how fast a drift dies out
                SAFE_LANDING_SPEED = 70.0f,   // px/s; any faster is a crash
                LANDING_TOLERANCE  = 4.0f;    // px; how far the ship may already
                                              // sink into the pad and still "land"

// ----- the thruster flame (extra credit) ----- //
constexpr Vector2 FLAME_SIZE       = { 16.0f, 34.0f },
                  FLAME_ATLAS      = { 1, 3 };   // 1 row, 3 frames
constexpr float   FLAME_OVERLAP    = 6.0f;       // tucks the flame under the hull

// ----- fuel ----- //
constexpr float MAX_FUEL       = 100.0f,
                FUEL_BURN_RATE = 10.0f;   // per thruster, per second

// the gauge: one cell per tenth of the tank, emptying from the right
constexpr int     NUMBER_OF_FUEL_CELLS = 10;
constexpr Vector2 FUEL_FIRST_CELL_POS  = { 30.0f, 30.0f },
                  FUEL_CELL_SIZE       = { 24.0f, 24.0f };
constexpr float   FUEL_CELL_SPACING    = 28.0f;

// ----- the moving landing pad ----- //
constexpr Vector2 PAD_INIT_POS = { ORIGIN.x, 470.0f },
                  PAD_SIZE     = { 180.0f, 32.0f };
constexpr int     PAD_SPEED    = 90;
constexpr float   PAD_LEFT_LIMIT  = 220.0f,
                  PAD_RIGHT_LIMIT = 780.0f;

// ----- rocks: touching any of them fails the mission ----- //
constexpr int   NUMBER_OF_GROUND_ROCKS   = 12,
                NUMBER_OF_FLOATING_ROCKS = 2,   // one per FLOATING_ROCK_POSITIONS entry
                NUMBER_OF_ROCKS          = NUMBER_OF_GROUND_ROCKS + NUMBER_OF_FLOATING_ROCKS;
constexpr float GROUND_ROCK_SPACING    = 90.0f,
                ROCK_COLLIDER_RATIO    = 0.75f,   // the rocks are round, the box isn't
                GROUND_ROCK_SHOWING    = 2.0f / 3.0f;   // how much of each ground rock is on screen
constexpr Vector2 GROUND_ROCK_SIZE     = { 110.0f, 92.0f },
                  FLOATING_ROCK_SIZE   = { 80.0f, 68.0f };

const std::vector<Vector2> FLOATING_ROCK_POSITIONS = {
    { 430.0f, 230.0f },
    { 720.0f, 300.0f }
};

const std::vector<const char *> ROCK_FPS = {
    "assets/game/rock1.png", "assets/game/rock2.png",
    "assets/game/rock3.png", "assets/game/rock4.png"
};

const std::vector<const char *> HAZARD_FPS = {
    "assets/game/hazard1.png", "assets/game/hazard2.png"
};

// ----- the background ----- //
constexpr int   SPACE_COLUMNS = 4,
                SPACE_ROWS    = 3,
                NUMBER_OF_SPACE_TILES = SPACE_COLUMNS * SPACE_ROWS;
constexpr float SPACE_TILE_SIZE = 256.0f;

constexpr Vector2 BANNER_SIZE = { 760.0f, 170.0f };

// Global Variables
AppStatus  gAppStatus       = RUNNING;
GameStatus gGameStatus      = READY;
float      gPreviousTicks   = 0.0f,
           gTimeAccumulator = 0.0f,
           gFuel            = MAX_FUEL;
float      gUpThrust        = 0.0f,   // the thrust the player is asking for;
           gSideThrust      = 0.0f;   // update() turns it into acceleration
int        gThrustersFiring = 0;      // how many thrusters are burning fuel
bool       gFiringUp        = false;  // shows the flame under the ship

GameState gState;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isTouching(Entity *a, Entity *b);
void endMission(GameStatus result);

// Function Definitions

// box-to-box, using each entity's collider: they touch when they are closer
// than half their sizes on BOTH axes at once
bool isTouching(Entity *a, Entity *b)
{
    float xDistance = fabs(a->getPosition().x - b->getPosition().x) -
        ((a->getColliderDimensions().x + b->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(a->getPosition().y - b->getPosition().y) -
        ((a->getColliderDimensions().y + b->getColliderDimensions().y) / 2.0f);

    return xDistance < 0.0f && yDistance < 0.0f;
}

void endMission(GameStatus result)
{
    gGameStatus = result;

    if (result == ACCOMPLISHED) PlaySound(gState.winSound);
    else                        PlaySound(gState.loseSound);
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lunar Lander!");
    InitAudioDevice();

    gState.winSound  = LoadSound("assets/game/win.ogg");
    gState.loseSound = LoadSound("assets/game/lose.ogg");

    /*
        ----------- BACKGROUND -----------
    */
    gState.space = new Entity[NUMBER_OF_SPACE_TILES];

    for (int i = 0; i < NUMBER_OF_SPACE_TILES; i++)
    {
        int column = i % SPACE_COLUMNS;
        int row    = i / SPACE_COLUMNS;

        gState.space[i].setTexture("assets/game/space.png");
        gState.space[i].setScale({ SPACE_TILE_SIZE, SPACE_TILE_SIZE });
        gState.space[i].setPosition({
            SPACE_TILE_SIZE / 2.0f + column * SPACE_TILE_SIZE,
            SPACE_TILE_SIZE / 2.0f + row * SPACE_TILE_SIZE
        });
    }

    /*
        ----------- SHIP -----------
    */
    gState.ship = new Entity(
        SHIP_INIT_POS,           // position
        SHIP_SIZE,               // scale
        "assets/game/ship.png",  // texture file address
        PLAYER                   // entity type
    );

    gState.ship->setColliderDimensions(SHIP_COLLIDER);
    gState.ship->setAcceleration({ 0.0f, GRAVITY });

    /*
        ----------- THRUSTER FLAME (sprite sheet) -----------
    */
    std::map<Direction, std::vector<int>> flameAnimationAtlas = {
        { RIGHT, { 0, 1, 2, 1 } },
    };

    gState.flame = new Entity(
        SHIP_INIT_POS,             // position (follows the ship)
        FLAME_SIZE,                // scale
        "assets/game/flame.png",   // texture file address
        ATLAS,                     // single image or atlas?
        FLAME_ATLAS,               // atlas dimensions
        flameAnimationAtlas,       // actual atlas
        EFFECT                     // entity type
    );

    /*
        ----------- MOVING LANDING PAD -----------
    */
    gState.pad = new Entity(
        PAD_INIT_POS,           // position
        PAD_SIZE,               // scale
        "assets/game/pad.png",  // texture file address
        PLATFORM                // entity type
    );

    gState.pad->setSpeed(PAD_SPEED);
    gState.pad->moveRight();

    /*
        ----------- ROCKS -----------
    */
    gState.rocks = new Entity[NUMBER_OF_ROCKS];

    for (int i = 0; i < NUMBER_OF_ROCKS; i++)
    {
        gState.rocks[i].setEntityType(BLOCK);

        if (i < NUMBER_OF_GROUND_ROCKS)
        {
            // a jagged row of rocks along the bottom of the screen
            gState.rocks[i].setTexture(ROCK_FPS[i % ROCK_FPS.size()]);
            gState.rocks[i].setScale(GROUND_ROCK_SIZE);
            gState.rocks[i].setPosition({
                i * GROUND_ROCK_SPACING,
                SCREEN_HEIGHT - GROUND_ROCK_SIZE.y * (GROUND_ROCK_SHOWING - 0.5f)
            });
        }
        else
        {
            // brown rocks floating in the ship's way
            gState.rocks[i].setTexture(
                HAZARD_FPS[(i - NUMBER_OF_GROUND_ROCKS) % HAZARD_FPS.size()]
            );
            gState.rocks[i].setScale(FLOATING_ROCK_SIZE);
            gState.rocks[i].setPosition(
                FLOATING_ROCK_POSITIONS[i - NUMBER_OF_GROUND_ROCKS]
            );
        }

        gState.rocks[i].setColliderDimensions({
            gState.rocks[i].getScale().x * ROCK_COLLIDER_RATIO,
            gState.rocks[i].getScale().y * ROCK_COLLIDER_RATIO
        });
    }

    /*
        ----------- FUEL GAUGE -----------
    */
    // empty slots behind, full cells in front
    gState.fuelSlots = new Entity[NUMBER_OF_FUEL_CELLS];
    gState.fuelCells = new Entity[NUMBER_OF_FUEL_CELLS];

    for (int i = 0; i < NUMBER_OF_FUEL_CELLS; i++)
    {
        Vector2 cellPosition = {
            FUEL_FIRST_CELL_POS.x + i * FUEL_CELL_SPACING,
            FUEL_FIRST_CELL_POS.y
        };

        gState.fuelSlots[i].setTexture("assets/game/fuel_slot.png");
        gState.fuelSlots[i].setScale(FUEL_CELL_SIZE);
        gState.fuelSlots[i].setPosition(cellPosition);

        gState.fuelCells[i].setTexture("assets/game/fuel_cell.png");
        gState.fuelCells[i].setScale(FUEL_CELL_SIZE);
        gState.fuelCells[i].setPosition(cellPosition);
    }

    /*
        ----------- BANNERS -----------
    */
    gState.startBanner = new Entity(
        ORIGIN, BANNER_SIZE, "assets/game/start.png", NONE
    );
    gState.accomplishedBanner = new Entity(
        ORIGIN, BANNER_SIZE, "assets/game/accomplished.png", NONE
    );
    gState.failedBanner = new Entity(
        ORIGIN, BANNER_SIZE, "assets/game/failed.png", NONE
    );

    SetTargetFPS(FPS);
}

void processInput()
{
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

    // the first thruster key starts the mission
    if (gGameStatus == READY &&
        (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_D)))
        gGameStatus = PLAYING;

    // read which thrusters are held; update() applies them every fixed step
    gUpThrust        = 0.0f;
    gSideThrust      = 0.0f;
    gThrustersFiring = 0;
    gFiringUp        = false;

    if (gGameStatus == PLAYING && gFuel > 0.0f)
    {
        if (IsKeyDown(KEY_W))
        {
            gUpThrust = THRUST_UP;
            gThrustersFiring++;
            gFiringUp = true;
        }

        if (IsKeyDown(KEY_A))
        {
            gSideThrust = -1.0f * THRUST_SIDE;
            gThrustersFiring++;
        }
        else if (IsKeyDown(KEY_D))
        {
            gSideThrust = THRUST_SIDE;
            gThrustersFiring++;
        }
    }
}

void update()
{
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
        if (gGameStatus == READY || gGameStatus == PLAYING)
        {
            // the pad patrols back and forth between its limits
            if (gState.pad->getPosition().x >= PAD_RIGHT_LIMIT)
                gState.pad->moveLeft();
            else if (gState.pad->getPosition().x <= PAD_LEFT_LIMIT)
                gState.pad->moveRight();

            gState.pad->update(
                FIXED_TIMESTEP, nullptr, nullptr, 0, nullptr, 0
            );
        }

        if (gGameStatus == PLAYING)
        {
            // an empty tank cuts every thruster, even mid-frame
            if (gFuel <= 0.0f)
            {
                gUpThrust        = 0.0f;
                gSideThrust      = 0.0f;
                gThrustersFiring = 0;
                gFiringUp        = false;
            }

            // only ACCELERATION is ever set on the ship, never movement or
            // velocity. It is worked out every step so the drag below always
            // uses the ship's current speed.
            Vector2 acceleration = { gSideThrust, GRAVITY - gUpThrust };

            // no sideways thrust: drag, as an acceleration against the drift,
            // so the ship glides for a moment and then comes to a stop
            if (gSideThrust == 0.0f)
                acceleration.x = -1.0f * DRAG * gState.ship->getVelocity().x;

            gState.ship->setAcceleration(acceleration);

            // fuel burns for every thruster that is firing
            gFuel -= FUEL_BURN_RATE * gThrustersFiring * FIXED_TIMESTEP;
            if (gFuel < 0.0f) gFuel = 0.0f;

            // how fast we were falling the moment before any touchdown, and
            // whether the ship was still above the pad's surface
            float fallSpeed  = gState.ship->getVelocity().y;
            float shipBottom = gState.ship->getPosition().y + SHIP_COLLIDER.y / 2.0f;
            float padTop     = gState.pad->getPosition().y -
                               gState.pad->getColliderDimensions().y / 2.0f;

            // the pad is the only thing the ship can stand on
            gState.ship->update(
                FIXED_TIMESTEP, nullptr, gState.pad, 1, nullptr, 0
            );

            if (gState.ship->isCollidingBottom())
            {
                // a real landing comes down onto the pad from above, slowly;
                // the pad sliding into the ship from the side is a crash
                if (shipBottom <= padTop + LANDING_TOLERANCE &&
                    fallSpeed <= SAFE_LANDING_SPEED)
                    endMission(ACCOMPLISHED);
                else
                    endMission(FAILED);
            }
            else if (gState.ship->isCollidingTop())
                endMission(FAILED);   // flew into the pad from underneath

            for (int i = 0; i < NUMBER_OF_ROCKS; i++)
                if (gGameStatus == PLAYING &&
                    isTouching(gState.ship, &gState.rocks[i]))
                    endMission(FAILED);

            // lost in space
            Vector2 position = gState.ship->getPosition();
            if (gGameStatus == PLAYING &&
                (position.x < 0.0f || position.x > SCREEN_WIDTH ||
                 position.y < 0.0f || position.y > SCREEN_HEIGHT))
                endMission(FAILED);
        }

        // the flame rides just under the ship and keeps flickering
        gState.flame->setPosition({
            gState.ship->getPosition().x,
            gState.ship->getPosition().y + SHIP_SIZE.y / 2.0f
                + FLAME_SIZE.y / 2.0f - FLAME_OVERLAP
        });
        gState.flame->update(FIXED_TIMESTEP, nullptr, nullptr, 0, nullptr, 0);

        deltaTime -= FIXED_TIMESTEP;
    }

    gTimeAccumulator = deltaTime;

    // the fuel gauge: a cell stays lit while there is fuel left in its tenth
    float fuelPerCell = MAX_FUEL / NUMBER_OF_FUEL_CELLS;

    for (int i = 0; i < NUMBER_OF_FUEL_CELLS; i++)
    {
        if (gFuel > i * fuelPerCell) gState.fuelCells[i].activate();
        else                         gState.fuelCells[i].deactivate();
    }
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    for (int i = 0; i < NUMBER_OF_SPACE_TILES; i++) gState.space[i].render();
    for (int i = 0; i < NUMBER_OF_ROCKS; i++)       gState.rocks[i].render();

    gState.pad->render();

    if (gGameStatus == PLAYING && gFiringUp) gState.flame->render();
    gState.ship->render();

    for (int i = 0; i < NUMBER_OF_FUEL_CELLS; i++)
    {
        gState.fuelSlots[i].render();
        gState.fuelCells[i].render();   // an empty (inactive) cell draws nothing
    }

    if      (gGameStatus == READY)        gState.startBanner->render();
    else if (gGameStatus == ACCOMPLISHED) gState.accomplishedBanner->render();
    else if (gGameStatus == FAILED)       gState.failedBanner->render();

    EndDrawing();
}

void shutdown()
{
    delete gState.ship;
    delete gState.flame;
    delete gState.pad;
    delete[] gState.rocks;
    delete[] gState.space;
    delete[] gState.fuelSlots;
    delete[] gState.fuelCells;
    delete gState.startBanner;
    delete gState.accomplishedBanner;
    delete gState.failedBanner;

    UnloadSound(gState.winSound);
    UnloadSound(gState.loseSound);

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
        render();
    }

    shutdown();

    return 0;
}
