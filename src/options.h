#ifndef __OPTIONS_H__
#define __OPTIONS_H__

typedef struct optional_variable
{
    char* name;
    char* value;
} optional_variable;

typedef struct glsltool_options
{
    int x, y, width, height;
    char fps;
    char transp;
    char supersample;
    char* inputfile;
    optional_variable* variables;
    int numoptional_variables;
    char* outputfile;
    int outputframe;
} glsltool_options;

int handle_options(int argc, char* argv[]);
void cleanup_options(void);

extern glsltool_options options;

#endif //__OPTIONS_H__
