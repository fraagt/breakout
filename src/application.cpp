#include <iostream>
#include "application.hpp"

#include "glad/gl.h"
#include "glfw/glfw3.h"
#include "game.hpp"

// GLFW function declarations
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// The Width of the screen
const unsigned int SCREEN_WIDTH = 800;
// The height of the screen
const unsigned int SCREEN_HEIGHT = 600;

GLFWwindow *pWindow;
Game *Breakout;

Application::Application() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    glfwWindowHint(GLFW_RESIZABLE, false);

    pWindow = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
    if (pWindow == nullptr) {
        std::cout << "Failed to create GLFW m_window" << '\n';
        glfwTerminate();
        throw std::runtime_error("Window Creation Failed");
    }

    glfwMakeContextCurrent(pWindow);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoaderLoadGL()) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        throw std::runtime_error("GLAD Initialization Failed");
    }

    glfwSetKeyCallback(pWindow, key_callback);
    glfwSetFramebufferSizeCallback(pWindow, framebuffer_size_callback);

    // initialize game
    // ---------------
    Breakout = new Game(SCREEN_WIDTH, SCREEN_HEIGHT);
    Breakout->Init();

    // OpenGL configuration
    // --------------------
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Application::~Application() {
    delete Breakout;

    glfwDestroyWindow(pWindow);
    glfwTerminate();
}

void Application::run() {
    // deltaTime variables
    // -------------------
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(pWindow)) {
        // calculate delta time
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        for (auto &downKeyPair: Breakout->DownKeys) {
            downKeyPair.second = false;
        }

        for (auto &upKeyPair: Breakout->UpKeys) {
            upKeyPair.second = false;
        }

        glfwPollEvents();

        // manage user input
        // -----------------
        Breakout->ProcessInput(deltaTime);

        // update game state
        // -----------------
        Breakout->Update(deltaTime);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Breakout->Render();

        glfwSwapBuffers(pWindow);


        if (Breakout->IsExit) {
            glfwSetWindowShouldClose(pWindow, true);
        }
    }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    auto keyCode = static_cast<glfwxx::KeyCode>(key);

    if (action == GLFW_PRESS) {
        Breakout->PressedKeys[keyCode] = true;
        Breakout->DownKeys[keyCode] = true;
    } else if (action == GLFW_RELEASE) {
        Breakout->PressedKeys[keyCode] = false;
        Breakout->UpKeys[keyCode] = true;
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}