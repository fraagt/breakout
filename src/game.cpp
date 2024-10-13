#include <iostream>
#include "game.hpp"
#include "resource_manager.hpp"
#include "sprite_renderer.hpp"
#include "mesh_manager.hpp"
#include "game_level.hpp"
#include "ball_object.hpp"

void load_textures();

void load_shaders();

void load_meshes();

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;

Collision check_collision(BallObject &ball, GameObject &object);

Direction VectorDirection(glm::vec2 target);

struct GameData {
    SpriteRenderer *renderer = nullptr;
    std::vector<GameLevel> levels;
    unsigned int level = 0;
    GameObject *background = nullptr;
    GameObject *player = nullptr;
    BallObject *ball = nullptr;

    ~GameData() {
        delete renderer;
        delete background;
        delete player;
    }
};

GameData *data = new GameData();

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// Initial velocity of the player paddle
const glm::vec2 PLAYER_SPEED(500.0f, 0.0f);

// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;

std::vector<float> quadVertices = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f
};

std::vector<float> quadNormals = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f
};

std::vector<float> quadUVs = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
};

std::vector<unsigned int> quadTriangles = {
        0, 1, 2,
        0, 2, 3
};

Game::Game(unsigned int width, unsigned int height)
        : State(GAME_ACTIVE), Width(width), Height(height) {

}

Game::~Game() {
    ResourceManager::Clear();
    MeshManager::Clear();

    delete data;
}

void Game::Init() {
    load_textures();
    load_shaders();
    load_meshes();

    auto meshes = MeshManager::Meshes;


    auto projection = glm::ortho(
            0.0f,
            static_cast<float>(this->Width),
            static_cast<float>(this->Height),
            0.0f,
            -1.0f,
            1.0f);

    auto shader = ResourceManager::GetShader("square");
    shader.Use();
    shader.SetMatrix4("projection", projection);

    auto renderer = new SpriteRenderer;
    renderer->shader = &ResourceManager::GetShader("square");
    renderer->mesh = &MeshManager::Get("quad");
    data->renderer = renderer;

    std::string levelPaths[] = {
            "assets/levels/one.lvl",
            "assets/levels/two.lvl",
            "assets/levels/three.lvl",
            "assets/levels/four.lvl"
    };

    for (const auto &levelPath: levelPaths) {
        GameLevel gameLevel;
        gameLevel.Load(levelPath.c_str(), this->Width, this->Height / 2);
        data->levels.emplace_back(gameLevel);
    }

    auto backgroundGO = new GameObject;
    backgroundGO->sprite = &ResourceManager::GetTexture("background");
    backgroundGO->size = glm::vec2(this->Width, this->Height);
    data->background = backgroundGO;

    auto playerGO = new GameObject;
    playerGO->sprite = &ResourceManager::GetTexture("paddle");
    playerGO->size = PLAYER_SIZE;
    playerGO->position = glm::vec2(
            static_cast<float>(this->Width) / 2.0f - PLAYER_SIZE.x / 2.0f,
            static_cast<float>(this->Height) - PLAYER_SIZE.y
    );
    data->player = playerGO;

    auto ballGO = new BallObject;
    ballGO->position = playerGO->position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
                                                      -BALL_RADIUS * 2.0f);
    ballGO->Radius = BALL_RADIUS;
    ballGO->velocity = INITIAL_BALL_VELOCITY;
    ballGO->sprite = &ResourceManager::GetTexture("smiley_face");
    ballGO->size = glm::vec2(BALL_RADIUS * 2.0f, BALL_RADIUS * 2.0f);
    ballGO->IsStuck = true;
    data->ball = ballGO;
}

void Game::Update(float dt) {
    data->ball->Move(dt, this->Width);

    do_collisions();

    if (data->ball->position.y >= static_cast<float>(this->Height)) {
        data->levels[data->level].Reset();
        data->player->size = PLAYER_SIZE;
        data->player->position = glm::vec2(static_cast<float>(this->Width) / 2.0f - PLAYER_SIZE.x / 2.0f,
                                           static_cast<float>(this->Height) - PLAYER_SIZE.y);
        data->ball->Reset(data->player->position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)),
                          INITIAL_BALL_VELOCITY);
    }
}

void Game::ProcessInput(float dt) {
    if (this->State == GAME_ACTIVE) {
        auto direction = glm::zero<glm::vec2>();

        if (this->PressedKeys[glfwxx::KeyCode::A]) {
            direction.x += -1.0f;
        }
        if (this->PressedKeys[glfwxx::KeyCode::D]) {
            direction.x += 1.0f;
        }

        auto deltaMove = direction * PLAYER_SPEED * dt;
        auto newPosition = data->player->position + deltaMove;

        auto left_border = 0.0f;
        auto right_border = static_cast<float>(this->Width) - data->player->size.x;
        if (newPosition.x >= left_border && newPosition.x <= right_border) {
            data->player->position = newPosition;
            if (data->ball->IsStuck)
                data->ball->position += deltaMove;
        }

        if (this->DownKeys[glfwxx::KeyCode::Space]) {
            data->ball->IsStuck = false;
        }
    }

    if (DownKeys[glfwxx::KeyCode::Escape]) {
        this->IsExit = true;
    }
}

void Game::Render() {
    data->background->Draw(*data->renderer);


    if (this->State == GAME_ACTIVE) {
        data->player->Draw(*data->renderer);

        data->levels[data->level].Draw(*data->renderer);

        data->ball->Draw(*data->renderer);
    }
}

void Game::do_collisions() {
    if (data->ball->IsStuck)
        return;

    std::vector<GameObject *> bricks;
    for (auto &brick: data->levels[data->level].Bricks) {
        if (!brick.destroyed)
            bricks.push_back(&brick);
    }

    for (auto brick: bricks) {
        auto collision = check_collision(*data->ball, *brick);

        if (std::get<0>(collision)) {
            if (!brick->isSolid)
                brick->destroyed = true;

            auto dir = std::get<1>(collision);
            auto diff_vector = std::get<2>(collision);

            if (dir == LEFT || dir == RIGHT) {
                data->ball->velocity.x = -data->ball->velocity.x;
                auto penetration = data->ball->Radius - std::abs(diff_vector.x);
                penetration *= dir == LEFT ? 1 : -1;
                data->ball->position.x += penetration;
            } else {
                data->ball->velocity.y = -data->ball->velocity.y;

                float penetration = data->ball->Radius - std::abs(diff_vector.y);
                penetration *= dir == UP ? -1 : 1;
                data->ball->position.y += penetration;
            }
        }


    }

    auto ballPlayerCollision = check_collision(*data->ball, *data->player);
    if (std::get<0>(ballPlayerCollision)) {
        auto centerBoard = data->player->position.x + data->player->size.x / 2.0f;
        auto distance = (data->ball->position.x + data->ball->Radius) - centerBoard;
        auto ration = distance / (data->player->size.x / 2.0f);
        auto strength = 2.0f;
        auto oldVelocity = data->ball->velocity;
        data->ball->velocity.x = INITIAL_BALL_VELOCITY.x * ration * strength;
        data->ball->velocity.y = -1.0f * std::abs(data->ball->velocity.y);
        data->ball->velocity = glm::normalize(data->ball->velocity) * glm::length(oldVelocity);
    }
}

void load_textures() {
    ResourceManager::LoadTexture("assets/textures/block.png",
                                 true,
                                 "block");
    ResourceManager::LoadTexture("assets/textures/block_solid.png",
                                 true,
                                 "block_solid");
    ResourceManager::LoadTexture("assets/textures/background.jpg",
                                 false,
                                 "background");
    ResourceManager::LoadTexture("assets/textures/paddle.png",
                                 true,
                                 "paddle");
    ResourceManager::LoadTexture("assets/textures/smiley_face.png",
                                 true,
                                 "smiley_face");
}

void load_shaders() {
    ResourceManager::LoadShader("assets/shaders/square.vert",
                                "assets/shaders/square.frag",
                                nullptr,
                                "square");
}

void load_meshes() {
    MeshManager::Load(quadVertices,
                      quadNormals,
                      quadUVs,
                      quadTriangles,
                      "quad");
}

Collision check_collision(BallObject &ball, GameObject &object) {

    glm::vec2 center(ball.position + ball.Radius);

    glm::vec2 aabb_half_extents(object.size * 0.5f);
    glm::vec2 aabb_center(object.position + aabb_half_extents);

    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    glm::vec2 closest = aabb_center + clamped;

    difference = closest - center;

    if (glm::length(difference) <= ball.Radius)
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

Direction VectorDirection(glm::vec2 target) {
    glm::vec2 compass[] = {
            glm::vec2(0.0f, 1.0f),    // up
            glm::vec2(1.0f, 0.0f),    // right
            glm::vec2(0.0f, -1.0f),    // down
            glm::vec2(-1.0f, 0.0f)    // left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++) {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max) {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction) best_match;
}
