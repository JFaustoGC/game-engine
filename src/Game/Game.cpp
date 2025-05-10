#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>

#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"


Game::Game()
{
    isRunning = false;
    registry = std::make_unique<Registry>();
    assetStore = std::make_unique<AssetStore>();
    Logger::Log("Game constructor called");
}

Game::~Game()
{
    Logger::Log("Game destructor called");
}


void Game::Initialize()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        Logger::Log("SDL_Init error");
        return;
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    windowWidth = displayMode.w;
    windowHeight = displayMode.h;

    window = SDL_CreateWindow(NULL,
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              windowWidth,
                              windowHeight,
                              SDL_WINDOW_BORDERLESS);

    if (!window)
    {
        Logger::Err("SDL_CreateWindow error");
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer)
    {
        Logger::Err("SDL_CreateRenderer error");
        return;
    }

    Logger::Log("Game initialized");

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    isRunning = true;
}

void Game::Run()
{
    Setup();
    while (isRunning)
    {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::ProcessInput()
{
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent))
    {
        switch (sdlEvent.type)
        {
            case SDL_QUIT:
                isRunning = false;
                break;

            case SDL_KEYDOWN:
                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
                {
                    isRunning = false;
                }
                break;
        }
    }
}


void Game::Setup()
{
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();

    assetStore->AddTexture(renderer, "tank-image", "../assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "truck-image", "../assets/images/truck-ford-right.png");

    Entity tank = registry->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(10.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(50.0f, 0.0f));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32);

    Entity truck = registry->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(50.0f, 100.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0f, 50.0f));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32);
}


void Game::Update()
{
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME)
    {
        SDL_Delay(timeToWait);
    }

    double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;

    millisecsPreviousFrame = SDL_GetTicks();

    registry->GetSystem<MovementSystem>().Update(deltaTime);
    registry->Update();
}

void Game::Render()
{
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    registry->GetSystem<RenderSystem>().Update(renderer, assetStore);

    SDL_RenderPresent(renderer);
}

void Game::Destroy()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
