#ifndef __OPTIONS_H__
#define __OPTIONS_H__

typedef struct glsltool_options
{
    int x, y, width, height;
    char* inputfile;
} glsltool_options;

int handle_options(int argc, char* argv[]);

extern glsltool_options options;

#endif //__OPTIONS_H__
