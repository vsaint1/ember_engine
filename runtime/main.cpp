#include <SDL3/SDL_main.h>
#include "core/engine.h"

int main(int argc, char* argv[]) {


    if (!GEngine->initialize(1280, 720, "Golias Engine - Window")) {
        spdlog::error("Engine initialization failed, exiting");
        return -1;
    }

    auto& ecs = GEngine->get_world();

    // Create camera
    auto camera =
        ecs.entity("MainCamera").set<Transform3D>({.position = {0, 2, 20}, .rotation = {-0.4f, 0, 0}}).add<Camera3D>();

    // Create directional light (sun) with shadows
    auto dirLight = ecs.entity()
                       .set(Transform3D{})
                       .set(DirectionalLight{glm::vec3(1, -2.5, 1), glm::vec3(1.0f, 0.95f, 0.8f), 2.0f, true});

    // Create spot lights (no shadows)
    auto spotLight1 = ecs.entity()
                         .set(Transform3D{glm::vec3(5, 5, 5)})
                         .set(SpotLight{glm::vec3(-1, -1, -1), glm::vec3(1.0f, 0.3f, 0.3f), 30.0f, 12.5f, 17.5f});

    auto spotLight2 = ecs.entity()
                         .set(Transform3D{glm::vec3(-5, 5, 5)})
                         .set(SpotLight{glm::vec3(1, -1, -1), glm::vec3(0.3f, 0.3f, 1.0f), 50.0f, 12.5f, 17.5f});


    // Model carModel = ObjectLoader::load_model("res://sprites/obj/Car2.obj");
    //
    //
    // for (int i = 0; i < 100000; ++i) {
    //     float x = static_cast<float>((rand() % 200) - 100);
    //     float z = static_cast<float>((rand() % 200) - 100);
    //     create_model_entity("res://sprites/obj/Car2.obj", glm::vec3(x, 0, z), glm::vec3(0, static_cast<float>(rand() % 360), 0), glm::vec3(0.1f));
    // }


    // Model damagedHelmet = ObjectLoader::load_model("res://sprites/obj/DamagedHelmet.glb");
    // create_model_entity("res://sprites/obj/DamagedHelmet.glb", glm::vec3(10, 0, -5));

    Model sponza = ObjectLoader::load_model("res://sprites/obj/sponza/sponza.glb");
    create_model_entity("res://sprites/obj/sponza/sponza.glb", glm::vec3(0, 0, 0));
    //
    // Model nagonford = ObjectLoader::load_model("res://sprites/obj/nagonford/Nagonford_Animated.glb");
    // create_model_entity("res://sprites/obj/nagonford/Nagonford_Animated.glb", glm::vec3(0, 0, 0));
    //
    // create_mesh_entity("Cylinder", "res://models/cylinder.obj", glm::vec3(0, 0, -15), glm::vec3(0), glm::vec3(1.0f), Material{
    //     .albedo = glm::vec3(0, 1.0f, 0), .metallic = 0.5f, .roughness = 0.5f});
    //
    // create_mesh_entity("Torus", "res://models/torus.obj", glm::vec3(0, 0, 5), glm::vec3(0), glm::vec3(1.0f), Material{
    //     .albedo = glm::vec3(1.f, 0.f, 1.0), .metallic = 1.f, .roughness = 0.1, .emissive = glm::vec3(1, 0, 0), .emissive_strength = 1.0f});
    //
    // create_mesh_entity("Cone", "res://models/cone.obj", glm::vec3(0, 0, 15), glm::vec3(0), glm::vec3(1.0f), Material{
    //     .albedo = glm::vec3(1.0f, 1.0f, 0)});
    //
    // create_mesh_entity("BlenderMonkey", "res://models/blender_monkey.obj", glm::vec3(-10, 0, 10), glm::vec3(0), glm::vec3(1.0f), Material{
    //     .albedo = glm::vec3(0.0f, 1.0f, 1.0)});
    //
    // create_mesh_entity("Plane", "res://models/plane.obj", glm::vec3(0, -1, 0), glm::vec3(0), glm::vec3(10.0f), Material{
    //     .albedo = glm::vec3(0.5f, 0.5f, 0.5f), .metallic = 0.0f, .roughness = 1.0f});
    //
    //
    // create_mesh_entity("Cube", "res://models/cube.obj", glm::vec3(15, 0, 0), glm::vec3(0), glm::vec3(2.0f), Material{
    //     .albedo = glm::vec3(0.7f, 0.3f, 0.2f), .metallic = 0.2f, .roughness = 0.8f});
    //
    //
    // create_mesh_entity("Red Cube", "res://models/cube.obj", glm::vec3(3, 0, 0), glm::vec3(0), glm::vec3(1.5f), Material{
    //     .albedo = glm::vec3(0.8f, 0.1f, 0.1f), .metallic = 0.0f, .roughness = 0.3f});
    //
    // MeshInstance3D sphereMesh = ObjectLoader::load_mesh("res://models/sphere.obj");
    //
    // create_mesh_entity("Metallic Sphere", "res://models/sphere.obj", glm::vec3(-3, 0, 0), glm::vec3(0), glm::vec3(1.5f), Material{
    //     .albedo = glm::vec3(0.1f, 0.8f, 0.1f), .metallic = 0.9f, .roughness = 0.1f});
    //
    // create_mesh_entity("SmallCube", "res://models/cube.obj", glm::vec3(-3, 0, 0), glm::vec3(0), glm::vec3(0.5f), Material{
    //     .albedo = glm::vec3(0.2f, 0.2f, 1.0f), .metallic = 0.3f, .roughness = 0.7f});
    //
    // create_mesh_entity("Ground", "res://models/cube.obj", glm::vec3(0, -2, 0), glm::vec3(0), glm::vec3(1000.0f, 0.1f, 1000.0f), Material{
    //     .albedo = glm::vec3(0.6f, 0.8f, 0.6f), .metallic = 0.0f, .roughness = 0.9f});

    GEngine->run();

    return 0;
}
