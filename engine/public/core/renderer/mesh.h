#pragma once

#include "helpers/logging.h"

/*! @brief Vertex struct

    @version 0.0.3
*/
struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
};

/*! @brief Mesh class
    - A mesh is a collection of vertices and indices
    - VAO, VBO and EBO related to the mesh
    - Material (SOON)

    @version 0.0.3
*/
class Mesh {
public:
    Mesh();

    explicit Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices = {});

    ~Mesh();

    void Bind() const;

    void Draw(unsigned int mode = GL_TRIANGLES) const;

    void Update(const std::vector<Vertex>& new_vertices);

    size_t GetVertexCount() const;

private:
    unsigned int VAO, VBO, EBO;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    void Setup();

    bool bIsDirty = false;
};

