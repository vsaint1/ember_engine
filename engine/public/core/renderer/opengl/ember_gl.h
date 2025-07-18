#pragma once

#include "core/ember_core.h"

// TODO: get this based on device?
constexpr int MAX_QUADS         = 10000;
constexpr int MAX_VERTICES      = MAX_QUADS * 4;
constexpr int MAX_INDICES       = MAX_QUADS * 6;

/*
 * DESKTOP - 32
 * WEBGL - 16
 * ANDROID - 32
 * IOS - 32
 */

constexpr int MAX_TEXTURE_SLOTS = 16;

/*
   @brief Opengl Renderer implementation
   - Opengl 3.3
   - Opengl ES 3.0
*/
class OpenglRenderer final : public Renderer {
public:
    OpenglRenderer() = default;

    OpenglShader* GetDefaultShader() override;

    OpenglShader* GetTextShader() override;

    void Initialize() override;

    void Flush() override;

    void FlushText() override;

    void Resize(int view_width, int view_height) override;

    void SetContext(const void* ctx) override;

    void* GetContext() override;

    void Destroy() override;

    Texture LoadTexture(const std::string& file_path) override;

    Font LoadFont(const std::string& file_path, int font_size) override;

    void UnloadFont(const Font& font) override;

    void UnloadTexture(const Texture& texture) override;

    void ClearBackground(const Color& color) override;

    void BeginDrawing(const glm::mat4& view_projection = 0) override;

    void EndDrawing() override;

    void DrawText(const Font& font, const std::string& text, const Transform2D& transform, Color color, float font_size,
                  const ShaderEffect& shader_effect = {}, float kerning = 0.0f) override;

    void DrawTexture(const Texture& texture, const Transform2D& transform, glm::vec2 size,
                     const Color& color = {255, 255, 255, 255}) override;

    void DrawTextureEx(const Texture& texture, const ember::Rectangle& source, const ember::Rectangle& dest,
                       glm::vec2 origin, float rotation,float zIndex = 0.0f, const Color& color = {255, 255, 255, 255} ) override;

    void DrawLine(glm::vec3 start, glm::vec3 end, const Color& color, float thickness) override;

    void DrawRect(const Transform2D& transform, glm::vec2 size, const Color& color, float thickness) override;

    void DrawRectFilled(const Transform2D& transform, glm::vec2 size, const Color& color, float thickness) override;

    void DrawTriangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, const Color& color) override;

    void DrawTriangleFilled(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, const Color& color) override;

    void DrawCircle(glm::vec3 position, float radius, const Color& color, int segments = 32) override;

    void DrawCircleFilled(glm::vec3 position, float radius, const Color& color, int segments = 32) override;

    void BeginMode2D(const Camera2D& camera) override;

    void EndMode2D() override;

    void BeginCanvas() override;

    void EndCanvas() override;

    OpenglShader* default_shader = nullptr;
    OpenglShader* text_shader    = nullptr;

private:


    float BindTexture(Uint32 slot = 0) override ;

    void Submit(const Transform2D& transform, glm::vec2 size, glm::vec4 color, Uint32 slot = UINT32_MAX) override;

    glm::mat4 Projection;

    GLuint _textVAO = 0, _textVBO = 0, _textEBO =0;
    Vertex* _textBuffer = nullptr;
    int _textQuadCount = 0;
    int _textIndexCount = 0;
    unsigned int _currentFontTextureID = 0;


    GLuint VAO = 0, VBO = 0, EBO = 0;
    Vertex* _buffer                                       = nullptr;

    // OPENGL/ES HACK FIX
    Uint32 _textureArrayBuffer = 0;

    std::array<unsigned int, MAX_TEXTURE_SLOTS> _textures = std::array<unsigned int, MAX_TEXTURE_SLOTS>();
    int _textureCount                                     = 0;
    int _quadCount                                        = 0;
    int _indexCount                                       = 0;


    SDL_GLContext context = nullptr;
};
