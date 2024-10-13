#pragma once

#include <map>
#include "glad/gl.h"
#include "key_code.hpp"
#include "ball_object.hpp"

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game {
public:
    // game state
    GameState State;
    bool IsExit = false;
    std::map<glfwxx::KeyCode, bool> PressedKeys;
    std::map<glfwxx::KeyCode, bool> DownKeys;
    std::map<glfwxx::KeyCode, bool> UpKeys;
    unsigned int Width, Height;

    // constructor/destructor
    Game(unsigned int width, unsigned int height);

    ~Game();

    // initialize game state (load all shaders/textures/levels)
    void Init();

    // game loop
    void ProcessInput(float dt);

    void Update(float dt);

    void Render();

private:
    void do_collisions();
};