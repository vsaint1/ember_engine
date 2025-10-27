#pragma once
#include  "core/engine.h"


void create_camera_entity(
    const flecs::world& world,
    const glm::vec3& position = glm::vec3(0),
    const glm::vec3& rotation = glm::vec3(0),
    float fov                 = 45.0f,
    float near_plane          = 0.1f,
    float far_plane           = 1000.0f);

void create_model_entity(
    const Model& model,
    const glm::vec3& position = glm::vec3(0),
    const glm::vec3& rotation = glm::vec3(0),
    const glm::vec3& scale    = glm::vec3(1.0f));
