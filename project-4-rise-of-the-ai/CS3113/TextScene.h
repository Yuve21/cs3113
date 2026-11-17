#ifndef TEXTSCENE_H
#define TEXTSCENE_H

#include "Scene.h"

// A scene that is just a coloured background with one text image on it:
// the menu, "You Win" and "You Lose" are each one of these.
class TextScene : public Scene
{
private:
    const char *mBannerFilepath;

public:
    static constexpr float BANNER_WIDTH  = 800.0f,
                           BANNER_HEIGHT = 220.0f;

    TextScene();
    TextScene(Vector2 origin, const char *bgHexCode, const char *bannerFilepath);
    ~TextScene();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // TEXTSCENE_H
