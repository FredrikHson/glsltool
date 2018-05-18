print("initing");
plane = generateplane(1);
debug = loadshader("debug.vert", "debug.frag", 0, 0, 0);
image = loadimage("image.png");

setdefaultfilter(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);

function loop()
{
    beginpass();
    {
        //cleardepth();
        bindshader(debug);
        bindattribute("in_Position", MESH_FLAG_POSITION);
        bindattribute("in_Uvs", MESH_FLAG_TEXCOORD0);
        bindtexture("image", image);
        drawmesh(plane);
        bindshader(-1);
    }
    endpass();
}
