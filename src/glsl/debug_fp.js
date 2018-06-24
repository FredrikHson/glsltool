print("initing");
plane = generateplane(1);
debug = loadshader("debug.vert", "debug_fp.frag", 0, 0, 0);
image = loadimage("image.png", IMG_BOTTOM);

setdefaultfilter(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);

function loop()
{
    beginpass();
    {
        bindshader(debug);
        bindattribute("in_Position", MESH_FLAG_POSITION);
        bindattribute("in_Uvs", MESH_FLAG_TEXCOORD0);
        bindtexture("image", image);
        var mi = MOUSE_X / 500.0;
        var ma = MOUSE_Y / 500.0;
        //print(mi, ma, 1.0 / (ma - mi));
        setuniformf("minvalue", mi);
        setuniformf("maxvalue", ma);

        if(MOUSE_1 & PRESSED)
        {
            setuniformi("clip", 1);
        }
        else
        {
            setuniformi("clip", 0);
        }

        drawmesh(plane);
        bindshader(-1);
    }
    endpass();
}
