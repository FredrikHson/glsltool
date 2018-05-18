#version 440
in vec3 in_Position;
in vec2 in_Uvs;
out vec2 texcoord;
out vec3 position;


void main(void)
{
    gl_Position = vec4(in_Position, 1.0);
    position = gl_Position.xyz;
    texcoord = in_Uvs;
}
