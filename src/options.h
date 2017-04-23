
struct glsltool_options
{
    int x, y, width, height;
    char* inputfile;
};
int handle_options(int argc, char* argv[]);

extern struct glsltool_options options;
