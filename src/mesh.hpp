#pragma once


#include <vector>

class Mesh {
public:
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> uvs;
    std::vector<unsigned int> triangles;
    unsigned int VAO, VBO, EBO;

    Mesh();

    void upload();

    void clear();

    void draw();
};