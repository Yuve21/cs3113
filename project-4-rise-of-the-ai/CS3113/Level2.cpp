/**
* Author: Yuvraj Chandyok
* Assignment: Rise of the AI
* Date due: 11/16/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Level2.h"

Level2::Level2() : LevelScene{} {}

Level2::Level2(Vector2 origin, const char *bgHexCode, SceneID nextLevel)
    : LevelScene{ origin, bgHexCode, nextLevel }
{}

void Level2::initialise()
{
    mGameState.nextSceneID = NO_SCENE;
    mGameState.playerDied  = false;

    createMap(LEVEL_COLUMNS, mLevelData);
    createPlayer(PLAYER_START_COL, PLAYER_START_ROW);
    createFlag(FLAG_COL, FLAG_ROW);

    // enemy, AI type, tile column, tile row
    createEnemies(NUMBER_OF_ENEMIES);
    setUpEnemy(0, WANDERER,  6, 8);
    setUpEnemy(1, FOLLOWER, 24, 8);
    setUpEnemy(2, WANDERER, 32, 8);
    setUpEnemy(3, FOLLOWER, 46, 8);
}
