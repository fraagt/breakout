set(SOURCES
        main.cpp
        game.hpp
        game.cpp
        texture2D.hpp
        texture2D.cpp
        shader.hpp
        shader.cpp
        resource_manager.hpp
        resource_manager.cpp
        sprite_renderer.hpp
        sprite_renderer.cpp
        application.hpp
        application.cpp
        mesh.hpp
        mesh.cpp
        mesh_manager.hpp
        mesh_manager.cpp
        game_object.hpp
        game_object.cpp
        game_level.hpp
        game_level.cpp
        key_code.hpp
        ball_object.hpp
        ball_object.cpp
)

# Prepend "src/" to every source file
list(TRANSFORM SOURCES PREPEND "src/")

# Propagate the SOURCES variable to the parent scope
set(SOURCES ${SOURCES} PARENT_SCOPE)