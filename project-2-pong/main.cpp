/**
* Author: Yuvraj Chandyok
* Assignment: Project 2 - Pong Variations
* Date due: 10/14/2026
*
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
*
* Theme: Sun Wukong. The Destined One (golden staff, left) duels
* Erlang Shen (iron staff, right) over spirit orbs. First to 3 wins.
*
* Controls
*   1 / 2 / 3   number of spirit orbs in play (the first press starts the duel)
*   W / S       the Destined One
*   UP / DOWN   Erlang Shen (2-player mode only)
*   T           toggle 1-player / 2-player
*   Q           quit
*
* wukong.png is cropped from a Black Myth: Wukong promotional still,
* owned by Game Science. Used here for a non-commercial class project.
* The two orb textures are cropped from "Orbs Collection" by Lamoot
* (opengameart.org/content/orbs-collection), released under CC0.
**/

#include "CS3113/cs3113.h"

// Enums
enum GameStatus { READY, PLAYING, WUKONG_WINS, ERLANG_WINS };
enum GameMode   { TWO_PLAYER, ONE_PLAYER };

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 60,
              MAX_BALLS     = 3,
              WIN_SCORE     = 3;

constexpr char BG_COLOUR[] = "#5C6166";   // cold mountain mist, from project 1

constexpr char WUKONG_FP[]      = "assets/game/wukong.png",
               GOLD_STAFF_FP[]  = "assets/game/staff_gold.png",
               IRON_STAFF_FP[]  = "assets/game/staff_iron.png",
               JADE_FP[]        = "assets/game/orb_jade.png",
               AMBER_FP[]       = "assets/game/orb_amber.png",
               START_FP[]       = "assets/game/start.png",
               WUKONG_WIN_FP[]  = "assets/game/win_wukong.png",
               ERLANG_WIN_FP[]  = "assets/game/win_erlang.png";

constexpr Vector2 ORIGIN       = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  PADDLE_SIZE  = { 22.0f, 180.0f },
                  BALL_SIZE    = { 40.0f, 40.0f },
                  PIP_SIZE     = { 24.0f, 24.0f },
                  MURAL_SIZE   = { 380.0f, 416.0f },
                  BANNER_SIZE  = { 760.0f, 190.0f };

constexpr float PADDLE_MARGIN = 50.0f,    // gap between a staff and its wall
                PADDLE_SPEED  = 450.0f,
                AUTO_SPEED    = 300.0f,   // slower than a player, so 1P is winnable
                AUTO_DEAD_ZONE = 10.0f,    // stops Erlang's staff jittering in 1-player mode
                BALL_SPEED    = 380.0f,
                BALL_SPIN     = 120.0f,   // degrees per second
                BOUNCE_ANGLE  = 0.9f,     // how much a staff's edge steers the orb
                PIP_GAP       = 32.0f,
                PIP_TOP       = 30.0f,

                // dragging the window pauses the game, so the next frame's delta
                // time can be over a second and an orb would skip right past a
                // staff. Capped, an orb moves at most 380 * 0.05 = 19px a frame.
                MAX_DELTA_TIME = 0.05f;

// each orb leaves the centre at its own angle so they never stack on each other
const std::vector<float> SERVE_ANGLES = { 0.35f, -0.6f, 0.8f };

constexpr Vector2 WUKONG_INIT_POS = { PADDLE_MARGIN, ORIGIN.y },
                  ERLANG_INIT_POS = { SCREEN_WIDTH - PADDLE_MARGIN, ORIGIN.y };

constexpr int MURAL_ALPHA = 45,   // faint, so the mural stays in the background
              EMPTY_ALPHA = 60;   // a score pip for a point not yet earned

// Global Variables
AppStatus  gAppStatus     = RUNNING;
GameStatus gGameStatus    = READY;     // orbs wait until someone picks 1, 2 or 3
GameMode   gGameMode      = TWO_PLAYER;
float      gPreviousTicks = 0.0f;

Vector2 gWukongPosition = WUKONG_INIT_POS,
        gWukongMovement = { 0.0f, 0.0f },
        gErlangPosition = ERLANG_INIT_POS,
        gErlangMovement = { 0.0f, 0.0f };

// every orb that could ever be on screen exists from the start;
// only the first gBallCount are updated and rendered
std::vector<Vector2> gBallPositions = { ORIGIN, ORIGIN, ORIGIN };
std::vector<Vector2> gBallMovements = { { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f } };
std::vector<float>   gBallAngles    = { 0.0f, 0.0f, 0.0f };
int                  gBallCount     = 1;

int gWukongScore = 0,
    gErlangScore = 0;

Texture2D gWukongTexture;
Texture2D gGoldStaffTexture;
Texture2D gIronStaffTexture;
Texture2D gJadeTexture;
Texture2D gAmberTexture;
Texture2D gStartTexture;
Texture2D gWukongWinTexture;
Texture2D gErlangWinTexture;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
void renderObject(const Texture2D *texture, const Vector2 *position,
                  const Vector2 *scale, float rotation, Color tint);
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA,
                 const Vector2 *positionB, const Vector2 *scaleB);
Color fadedWhite(int alpha);
void serveBall(int index, float direction);
void setBallCount(int count);
void clampPaddle(Vector2 *position);
void updateStaffs(float deltaTime);
void updateOrbs(float deltaTime);

// Function Definitions
void renderObject(const Texture2D *texture, const Vector2 *position,
                  const Vector2 *scale, float rotation, Color tint)
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
                   rotation, tint);
}

// box-to-box: two boxes overlap when they are closer than half their sizes
// on BOTH axes at once
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA,
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) -
                      ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) -
                      ((scaleA->y + scaleB->y) / 2.0f);

    return xDistance < 0.0f && yDistance < 0.0f;
}

// WHITE, but see-through. Built the same way ColorFromHex builds its colours.
Color fadedWhite(int alpha)
{
    return Color{ static_cast<unsigned char>(255),
                  static_cast<unsigned char>(255),
                  static_cast<unsigned char>(255),
                  static_cast<unsigned char>(alpha) };
}

// put an orb back in the middle, heading left (-1.0f) or right (1.0f)
void serveBall(int index, float direction)
{
    gBallPositions[index] = ORIGIN;
    gBallMovements[index] = { direction, SERVE_ANGLES[index] };
    Normalise(&gBallMovements[index]);
}

void setBallCount(int count)
{
    // orbs that are joining the game start fresh from the centre;
    // orbs already in play keep going
    for (int i = gBallCount; i < count; i++)
        serveBall(i, (i % 2 == 0) ? 1.0f : -1.0f);

    gBallCount = count;

    // picking how many orbs is also what starts the duel
    gGameStatus = PLAYING;
}

// a staff can't leave the top or bottom of the screen
void clampPaddle(Vector2 *position)
{
    float top    = PADDLE_SIZE.y / 2.0f;
    float bottom = SCREEN_HEIGHT - PADDLE_SIZE.y / 2.0f;

    if (position->y < top)    position->y = top;
    if (position->y > bottom) position->y = bottom;
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 2 - Wukong Pong");

    gWukongTexture    = LoadTexture(WUKONG_FP);
    gGoldStaffTexture = LoadTexture(GOLD_STAFF_FP);
    gIronStaffTexture = LoadTexture(IRON_STAFF_FP);
    gJadeTexture      = LoadTexture(JADE_FP);
    gAmberTexture     = LoadTexture(AMBER_FP);
    gStartTexture     = LoadTexture(START_FP);
    gWukongWinTexture = LoadTexture(WUKONG_WIN_FP);
    gErlangWinTexture = LoadTexture(ERLANG_WIN_FP);

    for (int i = 0; i < MAX_BALLS; i++)
        serveBall(i, (i % 2 == 0) ? 1.0f : -1.0f);

    SetTargetFPS(FPS);
}

void processInput()
{
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

    gWukongMovement = { 0.0f, 0.0f };
    gErlangMovement = { 0.0f, 0.0f };

    // staffs and mode work before the duel starts too;
    // once someone has won, the only thing left to do is quit
    if (gGameStatus == READY || gGameStatus == PLAYING)
    {
        // 1 - 3 orbs, any time. The first press starts the duel.
        if (IsKeyPressed(KEY_ONE))   setBallCount(1);
        if (IsKeyPressed(KEY_TWO))   setBallCount(2);
        if (IsKeyPressed(KEY_THREE)) setBallCount(3);

        // T switches between 2-player and 1-player
        if (IsKeyPressed(KEY_T))
            gGameMode = (gGameMode == TWO_PLAYER) ? ONE_PLAYER : TWO_PLAYER;

        // THE DESTINED ONE - W / S
        if      (IsKeyDown(KEY_W)) gWukongMovement.y = -1.0f;
        else if (IsKeyDown(KEY_S)) gWukongMovement.y =  1.0f;

        // ERLANG SHEN - arrows, but ONLY in 2-player mode.
        // In 1-player mode updateStaffs() drives him and these keys do nothing.
        if (gGameMode == TWO_PLAYER)
        {
            if      (IsKeyDown(KEY_UP))   gErlangMovement.y = -1.0f;
            else if (IsKeyDown(KEY_DOWN)) gErlangMovement.y =  1.0f;
        }
    }
}

void updateStaffs(float deltaTime)
{
    // ERLANG SHEN in 1-player mode: follows the closest orb that is
    // heading his way, and drifts back to the middle when none are
    if (gGameMode == ONE_PLAYER)
    {
        float targetY         = ORIGIN.y;
        float closestDistance = SCREEN_WIDTH;

        for (int i = 0; i < gBallCount; i++)
        {
            float distance = gErlangPosition.x - gBallPositions[i].x;

            if (gBallMovements[i].x > 0.0f && distance > 0.0f &&
                distance < closestDistance)
            {
                closestDistance = distance;
                targetY         = gBallPositions[i].y;
            }
        }

        if      (targetY < gErlangPosition.y - AUTO_DEAD_ZONE) gErlangMovement.y = -1.0f;
        else if (targetY > gErlangPosition.y + AUTO_DEAD_ZONE) gErlangMovement.y =  1.0f;
    }

    float erlangSpeed = (gGameMode == ONE_PLAYER) ? AUTO_SPEED : PADDLE_SPEED;

    gWukongPosition.y += PADDLE_SPEED * gWukongMovement.y * deltaTime;
    gErlangPosition.y += erlangSpeed  * gErlangMovement.y * deltaTime;

    clampPaddle(&gWukongPosition);
    clampPaddle(&gErlangPosition);
}

void updateOrbs(float deltaTime)
{
    // only the orbs in play. The loop also stops the moment someone wins,
    // so two orbs scoring in the same frame can't crown two winners.
    for (int i = 0; i < gBallCount && gGameStatus == PLAYING; i++)
    {
        gBallPositions[i].x += BALL_SPEED * gBallMovements[i].x * deltaTime;
        gBallPositions[i].y += BALL_SPEED * gBallMovements[i].y * deltaTime;
        gBallAngles[i]      += BALL_SPIN * deltaTime;

        // bounce off the top and bottom walls
        float top    = BALL_SIZE.y / 2.0f;
        float bottom = SCREEN_HEIGHT - BALL_SIZE.y / 2.0f;

        if (gBallPositions[i].y < top)
        {
            gBallPositions[i].y = top;
            gBallMovements[i].y = fabs(gBallMovements[i].y);
        }
        else if (gBallPositions[i].y > bottom)
        {
            gBallPositions[i].y = bottom;
            gBallMovements[i].y = -1.0f * fabs(gBallMovements[i].y);
        }

        // bounce off the staffs. Only when the orb is heading INTO the staff,
        // so it can't get stuck bouncing back and forth inside it.
        // Where it hits decides the new angle: the edges send it off steeper.
        if (gBallMovements[i].x < 0.0f &&
            isColliding(&gBallPositions[i], &BALL_SIZE, &gWukongPosition, &PADDLE_SIZE))
        {
            gBallPositions[i].x = gWukongPosition.x + (PADDLE_SIZE.x + BALL_SIZE.x) / 2.0f;
            gBallMovements[i].x = 1.0f;
            gBallMovements[i].y = BOUNCE_ANGLE * (gBallPositions[i].y - gWukongPosition.y)
                                  / (PADDLE_SIZE.y / 2.0f);
            Normalise(&gBallMovements[i]);
        }
        else if (gBallMovements[i].x > 0.0f &&
                 isColliding(&gBallPositions[i], &BALL_SIZE, &gErlangPosition, &PADDLE_SIZE))
        {
            gBallPositions[i].x = gErlangPosition.x - (PADDLE_SIZE.x + BALL_SIZE.x) / 2.0f;
            gBallMovements[i].x = -1.0f;
            gBallMovements[i].y = BOUNCE_ANGLE * (gBallPositions[i].y - gErlangPosition.y)
                                  / (PADDLE_SIZE.y / 2.0f);
            Normalise(&gBallMovements[i]);
        }

        // past a staff: point to the other side, and the orb is served
        // back toward whoever just missed it
        if (gBallPositions[i].x + BALL_SIZE.x / 2.0f < 0.0f)
        {
            gErlangScore++;
            if (gErlangScore >= WIN_SCORE) gGameStatus = ERLANG_WINS;
            serveBall(i, -1.0f);
        }
        else if (gBallPositions[i].x > SCREEN_WIDTH + BALL_SIZE.x / 2.0f)
        {
            gWukongScore++;
            if (gWukongScore >= WIN_SCORE) gGameStatus = WUKONG_WINS;
            serveBall(i, 1.0f);
        }
    }
}

void update()
{
    // delta time
    float ticks     = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    if (deltaTime > MAX_DELTA_TIME) deltaTime = MAX_DELTA_TIME;

    // before the start the staffs can warm up but the orbs wait in the centre;
    // after game over everything freezes where it is
    if (gGameStatus == READY || gGameStatus == PLAYING) updateStaffs(deltaTime);
    if (gGameStatus == PLAYING)                         updateOrbs(deltaTime);
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    // faint mural of the Destined One behind the arena
    renderObject(&gWukongTexture, &ORIGIN, &MURAL_SIZE, 0.0f, fadedWhite(MURAL_ALPHA));

    renderObject(&gGoldStaffTexture, &gWukongPosition, &PADDLE_SIZE, 0.0f, WHITE);
    renderObject(&gIronStaffTexture, &gErlangPosition, &PADDLE_SIZE, 0.0f, WHITE);

    // only the orbs in play; amber and jade take turns
    for (int i = 0; i < gBallCount; i++)
    {
        if (i % 2 == 0)
            renderObject(&gAmberTexture, &gBallPositions[i], &BALL_SIZE,
                         gBallAngles[i], WHITE);
        else
            renderObject(&gJadeTexture,  &gBallPositions[i], &BALL_SIZE,
                         gBallAngles[i], WHITE);
    }

    // score pips: amber for the Destined One on the left, jade for Erlang
    // on the right. A faded pip is a point not yet won.
    for (int i = 0; i < WIN_SCORE; i++)
    {
        Vector2 wukongPip = { ORIGIN.x - PIP_GAP * (i + 1), PIP_TOP };
        Vector2 erlangPip = { ORIGIN.x + PIP_GAP * (i + 1), PIP_TOP };

        renderObject(&gAmberTexture, &wukongPip, &PIP_SIZE, 0.0f,
                     (i < gWukongScore) ? WHITE : fadedWhite(EMPTY_ALPHA));
        renderObject(&gJadeTexture,  &erlangPip, &PIP_SIZE, 0.0f,
                     (i < gErlangScore) ? WHITE : fadedWhite(EMPTY_ALPHA));
    }

    // start prompt, and the EXTRA CREDIT endgame banner
    if (gGameStatus == READY)
        renderObject(&gStartTexture, &ORIGIN, &BANNER_SIZE, 0.0f, WHITE);
    else if (gGameStatus == WUKONG_WINS)
        renderObject(&gWukongWinTexture, &ORIGIN, &BANNER_SIZE, 0.0f, WHITE);
    else if (gGameStatus == ERLANG_WINS)
        renderObject(&gErlangWinTexture, &ORIGIN, &BANNER_SIZE, 0.0f, WHITE);

    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gWukongTexture);
    UnloadTexture(gGoldStaffTexture);
    UnloadTexture(gIronStaffTexture);
    UnloadTexture(gJadeTexture);
    UnloadTexture(gAmberTexture);
    UnloadTexture(gStartTexture);
    UnloadTexture(gWukongWinTexture);
    UnloadTexture(gErlangWinTexture);

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
