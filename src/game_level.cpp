#include <fstream>
#include <sstream>
#include "game_level.hpp"
#include "resource_manager.hpp"


void GameLevel::Load(const char *file, unsigned int width, unsigned int height) {
    this->Bricks.clear();

    unsigned int tileCode;

    std::string line;
    std::ifstream ifstream(file);
    std::vector<std::vector<unsigned int>> tileData;

    if (ifstream) {
        while (std::getline(ifstream, line)) {
            std::istringstream sstream(line);
            std::vector<unsigned int> row;
            while (sstream >> tileCode)
                row.push_back(tileCode);
            tileData.push_back(row);

        }
        if (!tileData.empty())
            this->init(tileData, width, height);
    }
}

void GameLevel::Draw(SpriteRenderer &renderer) {
    for (auto &brick: this->Bricks) {
        if (!brick.destroyed)
            brick.Draw(renderer);
    }
}

bool GameLevel::IsCompleted() {
    for (auto &tile: this->Bricks)
        if (!tile.isSolid && !tile.destroyed)
            return false;
    return true;
}

void GameLevel::Reset() {
    for (auto &brick: this->Bricks) {
        brick.destroyed = false;
    }
}

void GameLevel::init(const std::vector<std::vector<unsigned int>> &tileData,
                     unsigned int width,
                     unsigned int height) {

    unsigned int levelHeight = tileData.size();
    unsigned int levelWidth = tileData[0].size();
    float unit_height = static_cast<float>(height) / static_cast<float>(levelHeight);
    float unit_width = static_cast<float>(width) / static_cast<float>(levelWidth);

    // initialize level tiles based on tileData
    for (unsigned int y = 0; y < levelHeight; ++y) {
        for (unsigned int x = 0; x < levelWidth; ++x) {
            // check block type from level data (2D level array)
            if (tileData[y][x] == 1) // solid
            {
                glm::vec2 pos(unit_width * static_cast<float>(x), unit_height * static_cast<float>(y));
                glm::vec2 size(unit_width, unit_height);
                GameObject go;
                go.position = pos;
                go.size = size;
                go.sprite = &ResourceManager::GetTexture("block_solid");
                go.color = glm::vec3(0.8f, 0.8f, 0.7f);
                go.isSolid = true;
                this->Bricks.emplace_back(go);
            } else if (tileData[y][x] > 1) {
                auto color = glm::one<glm::vec3>(); // original: white
                if (tileData[y][x] == 2)
                    color = glm::vec3(0.2f, 0.6f, 1.0f);
                else if (tileData[y][x] == 3)
                    color = glm::vec3(0.0f, 0.7f, 0.0f);
                else if (tileData[y][x] == 4)
                    color = glm::vec3(0.8f, 0.8f, 0.4f);
                else if (tileData[y][x] == 5)
                    color = glm::vec3(1.0f, 0.5f, 0.0f);

                glm::vec2 pos(unit_width * static_cast<float>(x), unit_height * static_cast<float>(y));
                glm::vec2 size(unit_width, unit_height);
                GameObject go;
                go.position = pos;
                go.size = size;
                go.sprite = &ResourceManager::GetTexture("block");
                go.color = color;

                this->Bricks.emplace_back(go);
            }
        }
    }
}
