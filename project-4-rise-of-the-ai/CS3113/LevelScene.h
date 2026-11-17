#ifndef LEVELSCENE_H
#define LEVELSCENE_H

#include "Scene.h"

constexpr int LEVEL_HEIGHT = 12;   // every level is 12 tiles tall (one screen)

// What all three levels share: the player, the map, the enemies and the flag,
// and how they update, draw and clean up. Each level only fills in its own
// initialise(): its map and where its enemies go.
class LevelScene : public Scene
{
protected:
    SceneID mNextLevel;   // where reaching the flag takes you

    void createMap(int columns, unsigned int *levelData);
    void createPlayer(int column, int row);
    void createFlag(int column, int row);
    void createEnemies(int count);
    void setUpEnemy(int index, AIType type, int column, int row);

    // the centre of a tile, measured from the map's own edges
    Vector2 tileCentre(int column, int row) const;

public:
    static constexpr float TILE_DIMENSION          = 50.0f,
                           ACCELERATION_OF_GRAVITY = 981.0f,
                           JUMPING_POWER           = 550.0f,
                           FALL_MARGIN             = 100.0f,
                           WANDERER_PATROL_RANGE   = 75.0f,  // 1.5 tiles each way
                           FLY_LERP_FACTOR         = 0.7f;   // gentle enough to run under

    static constexpr int   TILESET_COLUMNS = 5,
                           TILESET_ROWS    = 1,
                           WANDERER_SPEED  = 60,
                           FOLLOWER_SPEED  = 80;

    LevelScene();
    LevelScene(Vector2 origin, const char *bgHexCode, SceneID nextLevel);
    ~LevelScene();

    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // LEVELSCENE_H
