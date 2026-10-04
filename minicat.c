#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libstream.h"

int isoption(char *option)
{
    if ((strcmp(option, "-") == 0) || (strcmp(option, "-E") == 0)
        || (strcmp(option, "-n") == 0))
    {
        return 1;
    }
    else
        return 0;
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        struct stream *stream = lbs_fdopen(0, O_RDONLY);
        if (stream == NULL)
            return 1;
        lbs_fflush(stream);
        if (lbs_fclose(stream) == -1)
            return 1;
    }

    for (int i = 1; i < argc; i++)
    {
        if (isoption(argv[i]) == 0)
            return 1;
        if (strcmp(argv[i], "-") == 0)
        {
            struct stream *stream = lbs_fdopen(0, O_RDONLY);
            if (stream == NULL)
                return 1;
            lbs_fflush(stream);
            if (lbs_fclose(stream) == -1)
                return 1;
        }
    }

    return 0;
}
