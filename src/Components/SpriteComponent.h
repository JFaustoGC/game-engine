#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H


struct SpriteComponent {
    std::string assetId;
    int width;
    int height;
    SDL_Rect srcRect{};

    explicit SpriteComponent(const std::string &assetId = "", const int width = 0, const int height = 0,
                             const int srcRectX = 0, const int srcRectY = 0) {
        this->assetId = assetId;
        this->width = width;
        this->height = height;
        this->srcRect = {srcRectX, srcRectY, width, height};
    }
};

#endif //SPRITECOMPONENT_H
