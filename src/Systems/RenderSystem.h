#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include <SDL2/SDL.h>

class RenderSystem : public System {
public:
    RenderSystem() {
        RequireComponent<SpriteComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update(SDL_Renderer *renderer, const std::unique_ptr<AssetStore> &assetStore) const {
        for (auto entity: GetSystemEntities()) {
            const auto transform = entity.GetComponent<TransformComponent>();
            const auto &sprite = entity.GetComponent<SpriteComponent>();

            auto srcRect = sprite.srcRect;

            auto destRect = SDL_Rect{
                static_cast<int>(transform.position.x),
                static_cast<int>(transform.position.y),
                sprite.width * static_cast<int>(transform.scale.x),
                sprite.height * static_cast<int>(transform.scale.y)
            };

            SDL_RenderCopyEx(renderer, assetStore->GetTexture(sprite.assetId), &srcRect, &destRect, transform.rotation, nullptr, SDL_FLIP_NONE);
        }
    }
};

#endif //RENDERSYSTEM_H
