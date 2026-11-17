/**
* Author: Yuvraj Chandyok
* Assignment: Rise of the AI
* Date due: 11/16/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Level1.h"

Level1::Level1() : LevelScene{} {}

Level1::Level1(Vector2 origin, const char *bgHexCode, SceneID nextLevel)
    : LevelScene{ origin, bgHexCode, nextLevel }
{}

void Level1::initialise()
{
    mGameState.nextSceneID = NO_SCENE;
    mGameState.playerDied  = false;

    createMap(LEVEL_COLUMNS, mLevelData);
    createPlayer(PLAYER_START_COL, PLAYER_START_ROW);
    createFlag(FLAG_COL, FLAG_ROW);

    // enemy, AI type, tile column, tile row
    createEnemies(NUMBER_OF_ENEMIES);
    setUpEnemy(0, WANDERER,  6, 8);
    setUpEnemy(1, WANDERER, 22, 8);
    setUpEnemy(2, WANDERER, 36, 8);
}
