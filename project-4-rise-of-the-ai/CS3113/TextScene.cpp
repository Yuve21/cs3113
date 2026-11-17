/**
* Author: Yuvraj Chandyok
* Assignment: Rise of the AI
* Date due: 11/16/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "TextScene.h"

TextScene::TextScene()
    : Scene{ { 0.0f, 0.0f }, "#000000" }, mBannerFilepath{ nullptr }
{}

TextScene::TextScene(Vector2 origin, const char *bgHexCode,
                     const char *bannerFilepath)
    : Scene{ origin, bgHexCode }, mBannerFilepath{ bannerFilepath }
{}

TextScene::~TextScene() { shutdown(); }

void TextScene::initialise()
{
    mGameState.nextSceneID = NO_SCENE;
    mGameState.playerDied  = false;

    mGameState.banner = new Entity(
        mOrigin,                          // position
        { BANNER_WIDTH, BANNER_HEIGHT },  // scale
        mBannerFilepath,                  // texture file address
        NONE                              // entity type
    );
}

// nothing moves here; main reads ENTER every frame and switches scenes
void TextScene::update(float /* deltaTime */) {}

void TextScene::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.banner->render();
}

void TextScene::shutdown()
{
    delete mGameState.banner;
    mGameState.banner = nullptr;
}
