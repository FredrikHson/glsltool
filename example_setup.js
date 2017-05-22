print("initing");
blurbuffer1 = createrendertarget(1024, 768, 1, GL_RGB, GL_BYTE);
blurbuffer2 = createrendertarget(1024, 768, 1, GL_RGB, GL_FLOAT);
//screenplane = create_screenplane();
//monkey      = load_model("suzanne.obj");
diffusetex = loadimage("diffuse.png");
normalmap = loadimage("normal.png");
//blurShader  = load_shader("blur.vert", "blur.frag", 0, 0);

var red = 0.0;
var green = 0.0;
function loop()
{
    red += 0.01;

    if(red > 1)
    {
        red = 0;
    }

    green += 0.05;

    if(green > 1)
    {
        green = 0;
    }

    //print("hello ", i++);
    beginpass(blurbuffer1);
    {
        clear(red, 0, 0);
    }
    endpass();
    beginpass(blurbuffer2);
    {
        clear(red, 0, green);
    }
    endpass();
    //beginPass(blurbuffer2);
    //{
    //set_shader(blurShader);
    //set_parameter("blur_offset", 0.5, 0);
    //set_texture("tex1", blurbuffer1);
    //}
    //endPass();
    //beginPass(blurbuffer1);
    //{
    //set_shader(blurShader);
    //set_parameter("blur_offset", 0, 0.5);
    //set_texture("tex1", blurbuffer1);
    //}
    //endpass();
    beginpass();
    {
        clear(red, green, 0);
        //blit_texture(blurbuffer1);
    }
    endpass();
}
