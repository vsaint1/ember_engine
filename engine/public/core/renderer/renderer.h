#pragma once
#include  "core/component/components.h"
#include "base_struct.h"

struct RenderBatch {
    const MeshInstance3D* mesh;
    const Material* material;
    std::vector<glm::mat4> model_matrices;

    void clear() {
        model_matrices.clear();
    }
};

struct PairHash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

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
    virtual void begin_frame() = 0;

    virtual void begin_shadow_pass() = 0;
    virtual void render_shadow_pass(const glm::mat4& light_space_matrix) = 0;
    virtual void end_shadow_pass() = 0;

    virtual void begin_render_target() = 0;
    virtual void render_main_target(const Camera3D& camera,
                                    const Transform3D& camera_transform,
                                    const glm::mat4& light_space_matrix,
                                    const std::vector<DirectionalLight>& directional_lights,
                                    const std::vector<std::pair<Transform3D, SpotLight>>& spot_lights) = 0;
    virtual void end_render_target() = 0;

    virtual void begin_environment_pass() =0;
    virtual void render_environment_pass(const Camera3D& camera) =0;
    virtual void end_environment_pass() =0;

    virtual void add_to_render_batch(const Transform3D& transform,
                                     const MeshRef& mesh_ref, const MaterialRef& mat_ref) = 0;

    virtual void add_to_shadow_batch(const Transform3D& transform,const MeshRef& mesh_ref) = 0;


    virtual void swap_chain() = 0;

    std::unordered_map<std::string, Uint32> _textures;
    std::unordered_map<std::string, std::vector<MeshInstance3D>> _meshes;
    std::unordered_map<std::string, std::vector<Material>> _materials;

protected:
    SDL_Window* _window = nullptr;

    std::unique_ptr<Shader> _default_shader     = nullptr;
    std::unique_ptr<Shader> _shadow_shader      = nullptr;
    std::unique_ptr<Shader> _environment_shader = nullptr;

    WorldEnvironment* _world_environment = nullptr;

    std::shared_ptr<Framebuffer> shadow_map_fbo = nullptr;

    int width = 0, height = 0;

    std::shared_ptr<GpuBuffer> instance_buffer;

    std::unordered_map<std::pair<const MeshInstance3D*, const Material*>,
                       RenderBatch,
                       PairHash> render_batches;

    std::unordered_map<const MeshInstance3D*, RenderBatch> shadow_batches;

    size_t max_instances = 1000;

};
