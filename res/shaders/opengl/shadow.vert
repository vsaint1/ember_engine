layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoord;

// 3,4,5,6 (mat4 = 4 vec4 attributes)
layout (location = 3) in mat4 a_instance_model; // per-instance model matrix

uniform mat4 LIGHT_MATRIX;

void main() {
    vec3 WORLD_POSITION = vec3(a_instance_model * vec4(a_position, 1.0));
    gl_Position = LIGHT_MATRIX * vec4(WORLD_POSITION, 1.0);
}