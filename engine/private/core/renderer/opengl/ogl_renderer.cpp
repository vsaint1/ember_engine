#include  "core/renderer/opengl/ogl_renderer.h"
#include  "core/engine.h"

void APIENTRY ogl_validation_layer(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{


    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
        "\n[ValidationLayer]\n"
        "Source: %s\nType: %s\nID: %u\nSeverity: %s\nMessage: %s\n\n",
        source, type, id, severity, message);

#if defined(_MSC_VER)
#define DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
#define DEBUG_BREAK() __builtin_trap()
#else
#define DEBUG_BREAK() std::abort()
#endif

    if (type == GL_DEBUG_TYPE_ERROR)
        DEBUG_BREAK();

}


GLuint load_cubemap_from_atlas(const std::string& atlas_path, CubemapOrientation orient = CubemapOrientation::DEFAULT) {
    spdlog::debug("Loading cubemap atlas: {}", atlas_path);

    int W, H, channels;
    unsigned char* pixels = stbi_load(atlas_path.c_str(), &W, &H, &channels, STBI_rgb_alpha);

    if (!pixels) {
        spdlog::error("Failed to load cubemap atlas: {}", atlas_path);
        return 0;
    }

    spdlog::debug("Atlas loaded: {}x{} channels: {}", W, H, channels);

    if (W <= 0 || H <= 0) {
        spdlog::error("Invalid atlas dimensions: {}x{}", W, H);
        stbi_image_free(pixels);
        return 0;
    }

    // Detect layout
    int face_w                                                               = 0, face_h = 0;
    enum Layout { HORIZONTAL, VERTICAL, L_3x2, L_4x3_CROSS, UNKNOWN } layout = UNKNOWN;

    if (W % 6 == 0 && W / 6 == H) {
        layout = HORIZONTAL;
        face_w = W / 6;
        face_h = H;
    } else if (H % 6 == 0 && H / 6 == W) {
        layout = VERTICAL;
        face_w = W;
        face_h = H / 6;
    } else if (W % 3 == 0 && H % 2 == 0 && W / 3 == H / 2) {
        layout = L_3x2;
        face_w = W / 3;
        face_h = H / 2;
    } else if (W % 4 == 0 && H % 3 == 0 && W / 4 == H / 3) {
        layout = L_4x3_CROSS;
        face_w = W / 4;
        face_h = H / 3;
    } else {
        spdlog::error("Unknown atlas layout: {}x{}", W, H);
        stbi_image_free(pixels);
        return 0;
    }

    if (face_w <= 0 || face_h <= 0) {
        spdlog::error("Invalid face dimensions: {}x{}", face_w, face_h);
        stbi_image_free(pixels);
        return 0;
    }

    spdlog::debug("Detected layout: {}, Face size: {}x{}", static_cast<int>(layout), face_w, face_h);

    // Define face rectangles based on layout
    struct Rect {
        int x, y, w, h;
    };
    std::array<Rect, 6> face_rects;

    if (layout == HORIZONTAL) {
        for (int i = 0; i < 6; ++i) {
            face_rects[i] = {i * face_w, 0, face_w, face_h};
        }
    } else if (layout == VERTICAL) {
        for (int i = 0; i < 6; ++i) {
            face_rects[i] = {0, i * face_h, face_w, face_h};
        }
    } else if (layout == L_3x2) {
        face_rects[0] = {0, 0, face_w, face_h}; // +X
        face_rects[1] = {1 * face_w, 0, face_w, face_h}; // -X
        face_rects[2] = {2 * face_w, 0, face_w, face_h}; // +Y
        face_rects[3] = {0, 1 * face_h, face_w, face_h}; // -Y
        face_rects[4] = {1 * face_w, 1 * face_h, face_w, face_h}; // +Z
        face_rects[5] = {2 * face_w, 1 * face_h, face_w, face_h}; // -Z
    } else {
        // L_4x3_CROSS
        face_rects[0] = {2 * face_w, 1 * face_h, face_w, face_h}; // +X
        face_rects[1] = {0, 1 * face_h, face_w, face_h}; // -X
        face_rects[2] = {1 * face_w, 0, face_w, face_h}; // +Y
        face_rects[3] = {1 * face_w, 2 * face_h, face_w, face_h}; // -Y
        face_rects[4] = {1 * face_w, 1 * face_h, face_w, face_h}; // +Z
        face_rects[5] = {3 * face_w, 1 * face_h, face_w, face_h}; // -Z
    }

    // Apply orientation adjustments
    switch (orient) {
    case CubemapOrientation::TOP:
        std::swap(face_rects[2], face_rects[3]); // +Y <-> -Y
        break;
    case CubemapOrientation::BOTTOM:
        std::swap(face_rects[2], face_rects[3]);
        break;
    case CubemapOrientation::FLIP_X:
        std::swap(face_rects[0], face_rects[1]);
        std::swap(face_rects[4], face_rects[5]);
        break;
    case CubemapOrientation::FLIP_Y:
        std::swap(face_rects[2], face_rects[3]);
        std::swap(face_rects[4], face_rects[5]);
        break;
    default:
        break;
    }

    // Validate rectangles
    for (int i = 0; i < 6; ++i) {
        const auto& r = face_rects[i];
        if (r.x < 0 || r.y < 0 || r.x + r.w > W || r.y + r.h > H) {
            spdlog::error("Face rect {} out of bounds: x={} y={} w={} h={} (atlas: {}x{})",
                          i, r.x, r.y, r.w, r.h, W, H);
            stbi_image_free(pixels);
            return 0;
        }
    }

    // Create cubemap texture
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    constexpr int BYTES_PER_PIXEL = 4;
    int pitch                     = W * BYTES_PER_PIXEL;
    std::vector<unsigned char> face_data(face_w * face_h * BYTES_PER_PIXEL);

    // Extract and upload each face
    for (int i = 0; i < 6; ++i) {
        const auto& r = face_rects[i];

        // Copy face data row by row
        for (int y = 0; y < r.h; ++y) {
            int row            = r.y + y;
            unsigned char* src = pixels + (row * pitch) + (r.x * BYTES_PER_PIXEL);
            unsigned char* dst = face_data.data() + (y * r.w * BYTES_PER_PIXEL);
            std::memcpy(dst, src, r.w * BYTES_PER_PIXEL);
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
                     r.w, r.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, face_data.data());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    stbi_image_free(pixels);

    spdlog::info("Loaded cubemap atlas {} ({}x{}) Layout {} Face {}x{} Texture ID: {}",
                 atlas_path, W, H, static_cast<int>(layout), face_w, face_h, texture_id);

    return texture_id;
}


Skybox OpenGLRenderer::create_skybox_from_atlas(const std::string& atlas_path,
                                                CubemapOrientation orient,
                                                float brightness) {
    Skybox sky{};

     constexpr float skybox_vertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    if (sky.VAO == 0) {
        glGenVertexArrays(1, &sky.VAO);
        glGenBuffers(1, &sky.VBO);

        glBindVertexArray(sky.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, sky.VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices),
                     skybox_vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);

        glBindVertexArray(0);

        spdlog::info("Skybox geometry initialized");
    }

    GLuint cubemap = load_cubemap_from_atlas(atlas_path, orient);

    if (cubemap == 0) {
        spdlog::error("Failed to create skybox from atlas - loading failed");
        return sky;
    }

    sky.texture = cubemap;
    spdlog::info("Skybox created from atlas successfully");

    _textures[atlas_path] = cubemap;
    return sky;
}

GLuint OpenGLRenderer::create_gl_texture(const unsigned char* data, int w, int h, int channels) {
    GLuint texID = 0;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    GLenum format = GL_RGB;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texID;
}

OpenGLRenderer::~OpenGLRenderer() {
    OpenGLRenderer::cleanup();
}

bool OpenGLRenderer::initialize(int w, int h, SDL_Window* window) {

    _window = window;

#if defined(SDL_PLATFORM_IOS) || defined(SDL_PLATFORM_ANDROID) || defined(SDL_PLATFORM_EMSCRIPTEN)

    /* GLES 3.0 -> GLSL: 300 */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

#elif defined(SDL_PLATFORM_WINDOWS) || defined(SDL_PLATFORM_LINUX) || defined(SDL_PLATFORM_MACOS)

    /* OPENGL 3.3 -> GLSL: 330*/
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);


#endif

    SDL_GLContext glContext = SDL_GL_CreateContext(GEngine->get_window());

    if (!glContext) {
        spdlog::critical("Failed to create GL context: {}", SDL_GetError());
        return false;
    }


#if defined(SDL_PLATFORM_IOS) || defined(SDL_PLATFORM_ANDROID) || defined(SDL_PLATFORM_EMSCRIPTEN)

    if (!gladLoadGLES2Loader((GLADloadproc) SDL_GL_GetProcAddress)) {
        spdlog::critical("Failed to initialize GLAD (GLES_FUNCTIONS)");
        return false;
    }

#else

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        spdlog::critical("Failed to initialize GLAD (GL_FUNCTIONS)");
        return false;
    }

#endif

    width  = w;
    height = h;

    GLint num_extensions = 0;
    std::vector<std::string_view> extensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
    extensions.reserve(num_extensions);

    bool khr_debug_found = false;

    for (GLuint i = 0; i < num_extensions; i++) {
        const char* ext = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));


        if (SDL_strcasecmp(ext, "GL_KHR_debug") == 0) {
            spdlog::debug("KHR_debug extension supported, enabling validation layers");
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(ogl_validation_layer, nullptr);
            khr_debug_found = true;
            break;
        }
    }

    if (!khr_debug_found) {
        spdlog::warn("KHR_debug extensions not supported, validation layers disabled");
    }

    int major, minor;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);

    const char* glsl_version = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    const char* gl_vendor    = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    const char* gl_renderer  = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

    spdlog::info("GLSL Version: {}", glsl_version);
    spdlog::info("OpenGL Version: {}.{}", major, minor);
    spdlog::info("OpenGL Vendor: {}", gl_vendor);
    spdlog::info("OpenGL Renderer: {}", gl_renderer);

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);

    _default_shader     = std::make_unique<OpenglShader>("shaders/opengl/default.vert", "shaders/opengl/default.frag");
    _default_shader->set_value("USE_IBL",false);

    _shadow_shader      = std::make_unique<OpenglShader>("shaders/opengl/shadow.vert", "shaders/opengl/shadow.frag");
    _environment_shader = std::make_unique<OpenglShader>("shaders/opengl/skybox.vert", "shaders/opengl/skybox.frag");

    FramebufferSpecification spec;
    spec.height      = 4096;
    spec.width       = 4096;
    spec.attachments = {
        {FramebufferTextureFormat::DEPTH_COMPONENT}
    };

    shadow_map_fbo = std::make_shared<OpenGLFramebuffer>(spec);
    _skybox = create_skybox_from_atlas("res/environment_sky.png", CubemapOrientation::DEFAULT, 1.0f);
    return true;
}

GLuint OpenGLRenderer::load_texture_from_file(const std::string& path) {
    if (auto it = _textures.find(path); it != _textures.end())
        return it->second;

    int w, h, channels;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 0);
    if (!data) {
        spdlog::error("Failed to load texture: {}", path);
        return 0;
    }

    GLuint texID = create_gl_texture(data, w, h, channels);
    stbi_image_free(data);

    _textures[path] = texID;
    spdlog::info("Loaded Texture: {}", path);
    return texID;
}

GLuint OpenGLRenderer::load_texture_from_memory(const unsigned char* buffer, size_t size, const std::string& name) {
    std::string key = name.empty() ? "embedded_tex_" + std::to_string(reinterpret_cast<size_t>(buffer)) : name;

    if (auto it = _textures.find(key); it != _textures.end())
        return it->second;

    int w, h, channels;
    unsigned char* data = stbi_load_from_memory(buffer, (int) size, &w, &h, &channels, 0);
    if (!data) {
        spdlog::error("Failed to load texture from memory: {}", key);
        return 0;
    }

    GLuint texID = create_gl_texture(data, w, h, channels);
    stbi_image_free(data);

    _textures[key] = texID;
    spdlog::info("Loaded embedded Texture: {}, Path {}", texID, key);
    return texID;
}

GLuint OpenGLRenderer::load_texture_from_raw_data(const unsigned char* data, int w, int h, int channels, const std::string& name) {
    std::string key = name.empty() ? "raw_" + std::to_string(reinterpret_cast<size_t>(data)) : name;

    if (auto it = _textures.find(key); it != _textures.end())
        return it->second;

    GLuint texID   = create_gl_texture(data, w, h, channels);
    _textures[key] = texID;
    spdlog::info("Loaded raw Texture: {}, Path {}", texID, key);

    return texID;
}

void OpenGLRenderer::begin_shadow_pass() {
    shadow_map_fbo->bind();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    _shadow_shader->activate();
}

void OpenGLRenderer::render_shadow_pass(const Transform3D& transform, const MeshInstance3D& mesh, const glm::mat4& light_space_matrix) {
    glm::mat4 model = transform.get_matrix();

    _shadow_shader->set_value("LIGHT_MATRIX", light_space_matrix, 1);
    _shadow_shader->set_value("MODEL", model, 1);

    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void OpenGLRenderer::end_shadow_pass() {
    shadow_map_fbo->unbind();
    glCullFace(GL_BACK);
    glViewport(0, 0, width, height);
}

void OpenGLRenderer::begin_render_target() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _default_shader->activate();
    glViewport(0, 0, width, height);
}

void OpenGLRenderer::render_entity(const Transform3D& transform, const MeshInstance3D& mesh, const Material& material,
                                   const Camera3D& camera,
                                   const glm::mat4& light_space_matrix, const std::vector<DirectionalLight>& directional_lights,
                                   const std::vector<std::pair<Transform3D, SpotLight>>& spot_lights) {
    glm::mat4 model = transform.get_matrix();

    // TODO:  PASS camera transform directly
    auto camera_query = GEngine->get_world().query<const Transform3D, const Camera3D>();

    const Transform3D& camera_transform = camera_query.first().get<Transform3D>();

    glm::mat4 view       = camera.get_view(camera_transform);
    glm::mat4 projection = camera.get_projection(width, height);

    _default_shader->set_value("MODEL", model);
    _default_shader->set_value("VIEW", view);
    _default_shader->set_value("PROJECTION", projection);
    _default_shader->set_value("LIGHT_MATRIX", light_space_matrix);

    // Camera
    _default_shader->set_value("CAMERA_POSITION_WORLD", camera_transform.position);


    material.bind(_default_shader.get());


    // REFACTORED: Directional lights using DirectionalLight class
    _default_shader->set_value("numDirLights", static_cast<int>(directional_lights.size()));
    if (!directional_lights.empty()) {
        std::vector<glm::vec3> directions;
        std::vector<glm::vec3> colors;
        std::vector<int> castShadows;

        for (const auto& light : directional_lights) {
            directions.push_back(light.direction);
            colors.push_back(light.color * light.intensity);
            castShadows.push_back(light.castShadows ? 1 : 0);
        }

        for (int i = 0; i < static_cast<int>(directional_lights.size()); ++i) {
            _default_shader->set_value(fmt::format("dirLights[{}].direction", i), directions[i]);
            _default_shader->set_value(fmt::format("dirLights[{}].color", i), colors[i]);
            _default_shader->set_value(fmt::format("dirLights[{}].cast_shadows", i), castShadows[i]);
        }

    }

    _default_shader->set_value("numSpotLights", static_cast<int>(spot_lights.size()));
    if (!spot_lights.empty()) {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> directions;
        std::vector<glm::vec3> colors;
        std::vector<float> cutOffs;
        std::vector<float> outerCutOffs;

        for (const auto& [transform, light] : spot_lights) {
            positions.push_back(transform.position);
            directions.push_back(light.direction);
            colors.push_back(light.color * light.intensity);
            cutOffs.push_back(glm::cos(glm::radians(light.cutOff)));
            outerCutOffs.push_back(glm::cos(glm::radians(light.outerCutOff)));
        }

        for (int i = 0; i < static_cast<int>(spot_lights.size()); ++i) {
            _default_shader->set_value(fmt::format("spotLights[{}].position", i), positions[i]);
            _default_shader->set_value(fmt::format("spotLights[{}].direction", i), directions[i]);
            _default_shader->set_value(fmt::format("spotLights[{}].color", i), colors[i]);
            _default_shader->set_value(fmt::format("spotLights[{}].inner_cut_off", i), cutOffs[i]);
            _default_shader->set_value(fmt::format("spotLights[{}].outer_cut_off", i), outerCutOffs[i]);
        }

    }

    glActiveTexture(GL_TEXTURE0 + SHADOW_TEXTURE_UNIT);
    glBindTexture(GL_TEXTURE_2D, shadow_map_fbo->get_depth_attachment_id());
    _default_shader->set_value("SHADOW_MAP", SHADOW_TEXTURE_UNIT);

    glActiveTexture(GL_TEXTURE0 + ENVIRONMENT_TEXTURE_UNIT);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _skybox.texture);
    _default_shader->set_value("ENVIRONMENT_MAP", ENVIRONMENT_TEXTURE_UNIT);

    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void OpenGLRenderer::end_render_target() {

}

void OpenGLRenderer::begin_environment_pass() {
    glDepthFunc(GL_LEQUAL);
    _environment_shader->activate();

}

void OpenGLRenderer::render_environment_pass(const Camera3D& camera) {
    auto camera_query                   = GEngine->get_world().query<const Transform3D, const Camera3D>();
    const Transform3D& camera_transform = camera_query.first().get<Transform3D>();

    glm::mat4 view       = glm::mat4(glm::mat3(camera.get_view(camera_transform)));
    glm::mat4 projection = camera.get_projection(width, height);

    _environment_shader->set_value("VIEW", view);
    _environment_shader->set_value("PROJECTION", projection);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _skybox.texture);
    _environment_shader->set_value("TEXTURE", 0);

    glBindVertexArray(_skybox.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void OpenGLRenderer::end_environment_pass() {
    glDepthFunc(GL_LESS);
}

void OpenGLRenderer::resize(int w, int h) {
    width  = w;
    height = h;
    glViewport(0, 0, width, height);
}

void OpenGLRenderer::cleanup() {
    // Clean up textures
    for (auto& [key, texID] : _textures)
        glDeleteTextures(1, &texID);

    _textures.clear();

    if (_skybox.VAO) {
        glDeleteVertexArrays(1, &_skybox.VAO);
        _skybox.VAO = 0;
    }

    if (_skybox.VBO) {
        glDeleteBuffers(1, &_skybox.VBO);
        _skybox.VBO = 0;
    }



    SDL_GL_DestroyContext(_context);

}

void OpenGLRenderer::swap_chain() {
    SDL_GL_SwapWindow(_window);
}
