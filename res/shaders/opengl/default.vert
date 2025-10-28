layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoord;

// 3,4,5,6 (mat4 = 4 vec4 attributes)
layout (location = 3) in mat4 a_instance_model; // per-instance model matrix

out vec3 POSITION;
out vec3 NORMAL;
out vec2 UV;
out vec4 LIGHT_SPACE_POSITION;

uniform mat4 VIEW;
uniform mat4 PROJECTION;
uniform mat4 LIGHT_MATRIX;

void main() {
    POSITION = vec3(a_instance_model * vec4(a_position, 1.0));
    NORMAL = mat3(transpose(inverse(a_instance_model))) * a_normal;
    UV = a_texCoord;
    LIGHT_SPACE_POSITION = LIGHT_MATRIX * vec4(POSITION, 1.0);
    gl_Position = PROJECTION * VIEW * vec4(POSITION, 1.0);
}