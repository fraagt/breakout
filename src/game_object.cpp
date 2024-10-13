#include "game_object.hpp"

GameObject::GameObject()
        : position(0.0f),
          size(1.0f),
          velocity(0.0f),
          color(1.0f),
          rotation(0.0f),
          isSolid(false),
          destroyed(false) {}

void GameObject::Draw(SpriteRenderer &renderer) {
    renderer.Draw(*this->sprite, this->position, this->size, this->rotation, this->color);
}