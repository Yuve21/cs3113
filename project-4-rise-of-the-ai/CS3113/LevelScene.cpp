/**
* Author: Yuvraj Chandyok
* Assignment: Rise of the AI
* Date due: 11/16/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelScene.h"

LevelScene::LevelScene()
    : Scene{ { 0.0f, 0.0f }, "#000000" }, mNextLevel{ NO_SCENE }
{}

LevelScene::LevelScene(Vector2 origin, const char *bgHexCode, SceneID nextLevel)
    : Scene{ origin, bgHexCode }, mNextLevel{ nextLevel }
{}

LevelScene::~LevelScene() { shutdown(); }

Vector2 LevelScene::tileCentre(int column, int row) const
{
    return {
        mGameState.map->getLeftBoundary() + column * TILE_DIMENSION + TILE_DIMENSION / 2.0f,
        mGameState.map->getTopBoundary()  + row    * TILE_DIMENSION + TILE_DIMENSION / 2.0f
    };
}

void LevelScene::createMap(int columns, unsigned int *levelData)
{
    mGameState.map = new Map(
        columns, LEVEL_HEIGHT,          // map grid cols & rows
        levelData,                      // grid data
        "assets/game/tiles.png",        // texture filepath
        TILE_DIMENSION,                 // tile size
        TILESET_COLUMNS, TILESET_ROWS,  // texture cols & rows
        mOrigin                         // in-game origin
    );
}

void LevelScene::createPlayer(int column, int row)
{
    // facing right: walk frames 0-10; facing left: the mirrored row, 12-22
    std::map<Direction, std::vector<int>> playerAnimationAtlas = {
        { RIGHT, {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10 } },
        { LEFT,  { 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22 } },
    };

    // @see https://opengameart.org/content/platformer-art-deluxe (Kenney, CC0)
    mGameState.player = new Entity(
        tileCentre(column, row),     // position
        { 52.0f, 70.0f },            // scale
        "assets/game/player.png",    // texture file address
        ATLAS,                       // single image or atlas?
        { 2, 12 },                   // atlas dimensions
        playerAnimationAtlas,        // actual atlas
        PLAYER                       // entity type
    );

    mGameState.player->setJumpingPower(JUMPING_POWER);
    mGameState.player->setColliderDimensions({
        mGameState.player->getScale().x * 0.6f,
        mGameState.player->getScale().y * 0.9f
    });
    mGameState.player->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
}

void LevelScene::createFlag(int column, int row)
{
    mGameState.flag = new Entity(
        tileCentre(column, row),            // position
        { TILE_DIMENSION, TILE_DIMENSION }, // scale
        "assets/game/flag.png",             // texture file address
        NONE                                // entity type
    );
}

void LevelScene::createEnemies(int count)
{
    mGameState.enemyCount = count;
    mGameState.enemies    = new Entity[count];
}

void LevelScene::setUpEnemy(int index, AIType type, int column, int row)
{
    Entity *enemy = &mGameState.enemies[index];

    // every enemy sheet is one row of four frames: two facing left, two right
    std::map<Direction, std::vector<int>> enemyAnimationAtlas = {
        { LEFT,  { 0, 1 } },
        { RIGHT, { 2, 3 } },
    };

    switch (type)
    {
    case WANDERER:   // a slime that paces back and forth
        enemy->setTexture("assets/game/slime.png");
        enemy->setScale({ 51.0f, 28.0f });
        enemy->setSpeed(WANDERER_SPEED);
        enemy->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
        break;
    case FOLLOWER:   // a snail that crawls after the player once she is close
        enemy->setTexture("assets/game/snail.png");
        enemy->setScale({ 57.0f, 31.0f });
        enemy->setSpeed(FOLLOWER_SPEED);
        enemy->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
        break;
    case LERPER:     // a fly that glides after the player through the air
        enemy->setTexture("assets/game/fly.png");
        enemy->setScale({ 60.0f, 29.0f });
        enemy->setLerpFactor(FLY_LERP_FACTOR);
        break;
    default:
        break;
    }

    enemy->setTextureType(ATLAS);
    enemy->setSpriteSheetDimensions({ 1, 4 });
    enemy->setAnimationAtlas(enemyAnimationAtlas);
    enemy->setFrameSpeed(Entity::DEFAULT_FRAME_SPEED / 2);
    enemy->setEntityType(NPC);
    enemy->setAIType(type);
    enemy->setAIState(IDLE);
    enemy->setDirection(LEFT);
    enemy->setColliderDimensions({
        enemy->getScale().x * 0.8f,
        enemy->getScale().y * 0.8f
    });
    enemy->setPosition(tileCentre(column, row));

    // slimes pace a short stretch around where they start, so one can never
    // wander all the way back to where the player appears
    if (type == WANDERER) enemy->setPatrolRange(WANDERER_PATROL_RANGE);
}

void LevelScene::update(float deltaTime)
{
    mGameState.player->update(
        deltaTime,      // delta time / fixed timestep
        nullptr,        // player
        mGameState.map, // map
        nullptr,        // collidable entities
        0               // col. entity count
    );

    for (int i = 0; i < mGameState.enemyCount; i++)
    {
        // each enemy's AI decides its movement again every step
        mGameState.enemies[i].resetMovement();
        mGameState.enemies[i].update(
            deltaTime, mGameState.player, mGameState.map, nullptr, 0
        );
    }

    // checked from the player's side: touching any enemy costs a life
    for (int i = 0; i < mGameState.enemyCount; i++)
        if (mGameState.player->isColliding(&mGameState.enemies[i]))
            mGameState.playerDied = true;

    // falling into a pit costs a life too
    if (mGameState.player->getPosition().y >
        mGameState.map->getBottomBoundary() + FALL_MARGIN)
        mGameState.playerDied = true;

    // reaching the flag finishes the level
    if (!mGameState.playerDied &&
        mGameState.player->getPosition().x >= mGameState.flag->getPosition().x)
        mGameState.nextSceneID = mNextLevel;
}

void LevelScene::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    mGameState.flag->render();

    for (int i = 0; i < mGameState.enemyCount; i++)
        mGameState.enemies[i].render();

    mGameState.player->render();
}

void LevelScene::shutdown()
{
    delete mGameState.player;
    mGameState.player = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;
    delete mGameState.flag;
    mGameState.flag = nullptr;
    delete[] mGameState.enemies;
    mGameState.enemies    = nullptr;
    mGameState.enemyCount = 0;
}
