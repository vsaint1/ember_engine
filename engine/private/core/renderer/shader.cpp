#include "core/renderer/shader.h"

/*!

    @brief GLSL shader header (API `OpenGL`)

    @version 0.0.1
*/
#if defined(SDL_PLATFORM_ANDROID) || defined(SDL_PLATFORM_IOS) || defined(SDL_PLATFORM_EMSCRIPTEN)
#define SHADER_HEADER "#version 300 es\nprecision mediump float;\n"
#else
#define SHADER_HEADER "#version 330 core\n"
#endif

Shader::Shader(const std::string& vertex, const std::string& fragment) {

    const auto vertexShaderSrc   = SHADER_HEADER + LoadAssetsFile(vertex);
    const auto fragmentShaderSrc = SHADER_HEADER + LoadAssetsFile(fragment);

    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShaderSrc.c_str());
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc.c_str());

    unsigned int program = glCreateProgram();

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        LOG_CRITICAL("SHADER_PROGARM linking failed: %s", infoLog);
    }

    LOG_INFO("Successfully linked SHADER_PROGARM %d", program);

    // WARN: since our project is simple, we don't need to delete shader's ( it help's with debugging )
    glDeleteShader(vs);
    glDeleteShader(fs);

    this->id = program;
    LOG_INFO("Successfully created SHADER_PROGARM %d", id);
}

unsigned int Shader::CompileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        const char* type_str = type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT";
        LOG_CRITICAL("[%s] - Shader compilation failed: %s", type_str, info_log);
    }

    LOG_INFO("Successfully compiled %s", type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");

    return shader;
}

unsigned int Shader::GetUniformLocation(const std::string& name) const {
    if (uniforms.find(name) != uniforms.end()) {
        return uniforms[name];
    }

    unsigned int location = glGetUniformLocation(id, name.c_str());

    if (location == -1) {
        LOG_WARN("Uniform %s not found", name.c_str());
    }

    uniforms[name] = location;
    return location;
}

bool Shader::IsValid() const {
    int status;

    glGetProgramiv(id, GL_VALIDATE_STATUS, &status);

    char infoLog[512];
    if (!status) {
        glGetProgramInfoLog(id, 512, nullptr, infoLog);
        LOG_CRITICAL("SHADER_PROGARM validation failed: %s", infoLog);
        return false;
    }

    return true;
}

void Shader::Use() const {
    glUseProgram(id);
}

void Shader::Destroy() {
    glDeleteProgram(id);
}

unsigned int Shader::GetID() const {
    return id;
}


void Shader::SetValue(const std::string& name, float value) const {
    unsigned int location = GetUniformLocation(name);
    glUniform1f(location, value);
}

void Shader::SetValue(const std::string& name, int value) const {
    unsigned int location = GetUniformLocation(name);
    glUniform1i(location, value);
}

void Shader::SetValue(const std::string& name, glm::mat4 value) const {
    unsigned int location = GetUniformLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetValue(const std::string& name, glm::vec2 value) const {
    unsigned int location = GetUniformLocation(name);
    glUniform2fv(location, 1, glm::value_ptr(value));
}

void Shader::SetValue(const std::string& name, glm::vec3 value) const {
    unsigned int location = GetUniformLocation(name);
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::SetValue(const std::string& name, glm::vec4 value) const {
    unsigned int location = GetUniformLocation(name);
    glUniform4fv(location, 1, glm::value_ptr(value));
}