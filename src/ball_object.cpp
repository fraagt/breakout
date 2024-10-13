#include "ball_object.hpp"

glm::vec2 BallObject::Move(float dt, unsigned int window_width) {
    if (!this->IsStuck) {
        this->position += this->velocity * dt;

        if (this->position.x <= 0.0f) {
            this->velocity.x -= this->velocity.x;
            this->position.x = 0.0f;
        } else if ((this->position.x + this->size.x) >= static_cast<float>(window_width)) {
            this->velocity.x = -this->velocity.x;
            this->position.x = static_cast<float>(window_width) - this->size.x;
        }

        if (this->position.y <= 0.0f) {
            this->velocity.y = -this->velocity.y;
            this->position.y = 0.0f;
        }
    }

    return this->position;
}

void BallObject::Reset(glm::vec2 position, glm::vec2 velocity) {
    this->position = position;
    this->velocity = velocity;
    this->IsStuck = true;
}
