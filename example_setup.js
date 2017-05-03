blurbuffer1 = create_rendertarget(1024, 768, RGB, GL_BYTE);
blurbuffer2 = create_rendertarget(1024, 768, RGB, GL_FLOAT);
//screenplane = create_screenplane();
//monkey      = load_model("suzanne.obj");
//diffusetex  = load_image("diffuse.tga");
//blurShader  = load_shader("blur.vert", "blur.frag", 0, 0);

print("initing");
var i = 0;
function loop()
{
    print("hello ", i++);
    //begin_pass(blurbuffer1);
    //{
    //set_texture("diffuse", diffusetex);
    //render_model(monkey);
    //}
    //end_pass();
    //begin_pass(blurbuffer2);
    //{
    //set_shader(blurShader);
    //set_parameter("blur_offset", 0.5, 0);
    //set_texture("tex1", blurbuffer1);
    //}
    //end_pass();
    //begin_pass(blurbuffer1);
    //{
    //set_shader(blurShader);
    //set_parameter("blur_offset", 0, 0.5);
    //set_texture("tex1", blurbuffer1);
    //}
    //end_pass();
    //begin_pass();
    //{
    //blit_Texture(blurbuffer1);
    //}
    //end_pass();
}
