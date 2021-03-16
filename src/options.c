#include "options.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>

glsltool_options options =
{
    INT_MIN, INT_MIN, 512, 512, 0, 0, "renderpath.js", 0, 0, 0
};

void print_help(const struct option* opts)
{
    int i = 0;
    printf("glsltool\n");

    while(opts[i].name != 0)
    {
        if(strcmp(opts[i].name, "string") == 0)
        {
            printf("\t-%c \"<stringid=string>\" --%s=\"<stringid=string>\"\n",
                   opts[i].val,
                   opts[i].name);
        }
        else
        {
            if(opts[i].has_arg)
            {
                printf("\t-%c <%s> --%s=<%s>\n",
                       opts[i].val,
                       opts[i].name,
                       opts[i].name,
                       opts[i].name);
            }
            else
            {
                printf("\t-%c --%s\n",
                       opts[i].val,
                       opts[i].name);
            }
        }

        i++;
    }
}

void handle_optional_variable(char* inputstring)
{
    char* name = strtok(inputstring, "=");
    char* value = strtok(0, "");

    if(options.numoptional_variables == 0)
    {
        options.variables = malloc(sizeof(optional_variable));
    }
    else
    {
        options.variables = realloc(options.variables, sizeof(optional_variable) * (options.numoptional_variables + 1));
    }

    options.variables[options.numoptional_variables].name = malloc(strlen(name) + 1);
    options.variables[options.numoptional_variables].value = malloc(strlen(value) + 1);
    strcpy(options.variables[options.numoptional_variables].name, name);
    strcpy(options.variables[options.numoptional_variables].value, value);
    options.numoptional_variables += 1;
}

int handle_options(int argc, char* argv[])
{
    static const struct option longOpts[] =
    {

        { "x", required_argument, 0, 'x' },
        { "y", required_argument, 0, 'y' },
        { "width", required_argument, 0, 'W' },
        { "height", required_argument, 0, 'H' },
        { "file", required_argument, 0, 'f' },
        { "string", required_argument, 0, 't'},
        { "printfps", no_argument, 0, 'F'},
        { "output", no_argument, 0, 'o' },
        { "transparent", no_argument, 0, 'a'},
        { "help", no_argument, 0, 'h' },
        { 0, 0, 0, 0}
    };
    int c;
    int longIndex = 0;

    while((c = getopt_long(argc, argv, "x:y:W:H:f:t:o:Fah", longOpts, &longIndex)) != -1)
    {
        switch(c)
        {
            case 'x':
                options.x = atoi(optarg);
                break;

            case 'y':
                options.y = atoi(optarg);
                break;

            case 'W':
                options.width = atoi(optarg);
                break;

            case 'H':
                options.height = atoi(optarg);
                break;

            case 'f':
            {
                if(access(optarg, F_OK) != 0)
                {
                    fprintf(stderr, "could not find: %s\n", optarg);
                    exit(1);
                }

                options.inputfile = realpath(optarg, 0);
                char* dir = strdup(options.inputfile);

                if(dir != 0)
                {
                    chdir(dirname(dir));
                    free(dir);
                }

                break;
            }

            case 'F':
            {
                options.fps = 1;
                break;
            }

            case 't':
            {
                handle_optional_variable(optarg);
                break;
            }

            case 'o':
            {
                options.outputfile = strdup(optarg);
                fprintf(stderr, "outputfile:%s\n", options.outputfile);
                fprintf(stderr, "outputfile:%s\n", optarg);
                break;
            }

            case 'a':
            {
                options.transp = 1;
                break;
            }

            case 'h':
                print_help(longOpts);
                return 0;
                break;

            default:
                break;
        }
    }

    return 1;
}

void cleanup_options()
{
    if(options.inputfile)
    {
        free(options.inputfile);
    }

    for(int i = 0; i  < options.numoptional_variables; i ++)
    {
        if(options.variables[i].name)
        {
            free(options.variables[i].name);
        }

        if(options.variables[i].value)
        {
            free(options.variables[i].value);
        }
    }

    if(options.variables)
    {
        free(options.variables);
    }

    if(options.outputfile)
    {
        free(options.outputfile);
    }
}
