#pragma once
#include "core/api/engine_api.h"


void create_mesh_entity(
    const char* name,
    const char* path,
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale,
    const Material& material) {

    auto renderer = GEngine->get_renderer();

    if (renderer->_meshes.find(path) == renderer->_meshes.end()) {
        MeshInstance3D mesh        = ObjectLoader::load_mesh(path);
        renderer->_meshes[path]    = {mesh};
        renderer->_materials[path] = {material};
    }

    GEngine->get_world().entity(name)
           .set(Transform3D{position, rotation, scale})
           .set(MeshRef{&renderer->_meshes[path][0]})
           .set(MaterialRef{&renderer->_materials[path][0]});


}


void create_model_entity(
    const char* path,
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale) {


    auto renderer = GEngine->get_renderer();

    auto mesh_it = renderer->_meshes.find(path);
    auto mat_it  = renderer->_materials.find(path);

    if (mesh_it == renderer->_meshes.end() ||
        mat_it == renderer->_materials.end()) {
        spdlog::error("Model not loaded: {}", path);
        return;
    }

    const auto& meshes    = mesh_it->second;
    const auto& materials = mat_it->second;



    for (size_t i = 0; i < meshes.size(); ++i) {
        GEngine->get_world().entity()
               .set(Transform3D{position, rotation, scale})
               .set(MeshRef{&meshes[i]})
               .set(MaterialRef{&materials[i]});
    }
}
