#pragma once
#include  "core/component/components.h"
#include "base_struct.h"


class Renderer {
public:
    virtual ~Renderer() = default;

    virtual bool initialize(int width, int height, SDL_Window* window) = 0;
    virtual void resize(int width, int height) = 0;
    virtual void cleanup() = 0;

    virtual std::shared_ptr<GpuBuffer> allocate_gpu_buffer(GpuBufferType type) = 0;

    virtual std::shared_ptr<GpuVertexLayout> create_vertex_layout(
           const GpuBuffer* vertex_buffer,
           const GpuBuffer* index_buffer,
           const std::vector<VertexAttribute>& attributes,
           uint32_t stride) = 0;

    virtual Uint32 load_texture_from_file(const std::string& path) = 0;
    virtual Uint32 load_texture_from_memory(const unsigned char* buffer, size_t size, const std::string& name = "") = 0;
    virtual Uint32 load_texture_from_raw_data(const unsigned char* data, int width, int height, int channels = 4,
                                              const std::string& name                                        = "") = 0;

    virtual void begin_shadow_pass() = 0;
    virtual void render_shadow_pass(const Transform3D& transform, const MeshInstance3D& mesh, const glm::mat4& lightSpaceMatrix) = 0;
    virtual void end_shadow_pass() = 0;

    virtual void begin_render_target() = 0;
    virtual void render_entity(const Transform3D& transform,
                               const MeshInstance3D& mesh,
                               const Material& material,
                               const Camera3D& camera,
                               const glm::mat4& lightSpaceMatrix,
                               const std::vector<DirectionalLight>& directionalLights,
                               const std::vector<std::pair<Transform3D, SpotLight>>& spotLights) = 0;
    virtual void end_render_target() = 0;

    virtual void begin_environment_pass() =0;
    virtual void render_environment_pass(const Camera3D& camera) =0;
    virtual void end_environment_pass() =0;

    virtual void swap_chain() = 0;

protected:
    SDL_Window* _window = nullptr;

    std::unique_ptr<Shader> _default_shader = nullptr;
    std::unique_ptr<Shader> _shadow_shader  = nullptr;
    std::unique_ptr<Shader> _environment_shader  = nullptr;

    WorldEnvironment* _world_environment = nullptr;

    std::shared_ptr<Framebuffer> shadow_map_fbo = nullptr;

    std::unordered_map<std::string, Uint32> _textures;

    int width = 0, height = 0;

};
