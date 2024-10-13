#include "sprite_renderer.hpp"

void SpriteRenderer::Draw(Texture2D &texture,
                          glm::vec2 position,
                          glm::vec2 size,
                          float rotate,
                          glm::vec3 color) {
    this->shader->Use();

    texture.Bind();
    this->shader->SetInteger("texture0", 0);


    auto model = glm::identity<glm::mat4>();

    auto pos = position + 0.5f * size;

    model = glm::translate(model, glm::vec3(pos, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));
    this->shader->SetMatrix4("model", model);

    this->shader->SetVector3f("color", color);


    this->mesh->draw();
}
