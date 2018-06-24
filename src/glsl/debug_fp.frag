#version 440
uniform sampler2D image;
out vec4 color;
in vec2 texcoord;
uniform float minvalue;
uniform float maxvalue;
uniform bool clip;

void main()
{
    color = texture(image, texcoord);
    float mi = minvalue;
    float ma = maxvalue;

    if(mi > ma)
    {
        ma = minvalue;
        mi = maxvalue;
    }

    float scaling = 1.0 / (ma - mi);
    color = (color - mi) * scaling;

    if(clip)
    {
        color = min(vec4(1.0), max(vec4(0.0), color));
    }
    else
    {
        color = fract(color);
    }
}
