#pragma once

#include <vector>
#include <map>
#include "game_object.hpp"

class GameLevel {
public:
    std::vector<GameObject> Bricks;

    GameLevel() = default;

    void Load(const char *file, unsigned int width, unsigned int height);

    void Draw(SpriteRenderer &renderer);

    bool IsCompleted();

    void Reset();

private:
    void init(const std::vector<std::vector<unsigned int>> &tileData,
              unsigned int screenWidth,
              unsigned int screenHeight);
};