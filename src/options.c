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
    0, 0, 512, 512, "renderpath.js"
};

void print_help(const struct option* opts)
{
    int i = 0;
    printf("glsltool\n");

    while(opts[i].name != 0)
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

        i++;
    }
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
        { "help", no_argument, 0, 'h' },
        { 0, 0, 0, 0}
    };
    int c;
    int longIndex = 0;

    while((c = getopt_long(argc, argv, "x:y:W:H:f:h", longOpts, &longIndex)) != -1)
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
                options.inputfile = realpath(optarg, 0);
                char* dir = strdup(options.inputfile);

                if(dir != 0)
                {
                    chdir(dirname(dir));
                    free(dir);
                }

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
