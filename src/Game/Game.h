#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>

#include "../AssetStore/AssetStore.h"
#include "../ECS/ECS.h"

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game {
private:
    bool isRunning;
    int millisecsPreviousFrame = 0;
    SDL_Window *window;
    SDL_Renderer *renderer;

    std::unique_ptr<Registry> registry;
    std::unique_ptr<AssetStore> assetStore;

public:
    Game();

    ~Game();

    void Initialize();

    void Run();

    void Setup();

    void ProcessInput();

    void Update();

    void Render();

    void Destroy();

    int windowWidth;
    int windowHeight;
};

#endif
