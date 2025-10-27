#pragma once
#include "core/api/engine_api.h"


void create_model_entity(
    const Model& model,
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale) {

    const std::string base_name = std::filesystem::path(model.path).stem().string();


    for (size_t i = 0; i < model.meshes.size(); ++i) {
        const auto& mesh = model.meshes[i];
        std::string entity_name;

        if (!mesh.name.empty())
            entity_name = mesh.name;
        else
            entity_name = base_name + "_Mesh_" + std::to_string(i);

        GEngine->get_world().entity(entity_name.c_str())
               .set(Transform3D{position, rotation, scale})
               .set(mesh)
               .set(model.materials[i]);
    }
}
