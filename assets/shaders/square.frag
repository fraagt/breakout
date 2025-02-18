#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture0;
uniform vec3 color;

void main() {

    FragColor = vec4(color, 1.0) * texture(texture0, TexCoord);
}
