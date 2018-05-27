#version 440
uniform sampler2D image;
out vec4 color;
in vec2 texcoord;

void main()
{
    color=texture(image,texcoord);
    //color=vec4(texcoord,1,0);
}
