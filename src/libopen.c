#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "libstream.h"
int modegestion(const char *mode)
{
    if (strcmp(mode, "r") == 0)
        return O_RDONLY;
    if (strcmp(mode, "r+") == 0)
        return O_RDWR;
    if (strcmp(mode, "w") == 0)
        return O_WRONLY | O_CREAT | O_TRUNC;
    if (strcmp(mode, "w+") == 0)
        return O_RDWR | O_CREAT | O_TRUNC;
    if (strcmp(mode, "a") == 0)
    {
        /*if (lseek(fd, 0, SEEK_END) == -1)
            return -1;*/
        return O_WRONLY | O_APPEND | O_CREAT;
    }
    if (strcmp(mode, "a+") == 0)
    {
        /*if (lseek(fd, 0, SEEK_END) == -1)
            return -1;*/
        return O_RDWR | O_APPEND | O_CREAT;
    }
    else
        return -1;
}

struct stream *lbs_fdopen(int fd, const char *mode)
{
    struct stream *res = malloc(sizeof(struct stream));
    if (res == NULL || mode == NULL)
        return NULL;
    
    int flagss = modegestion(mode);
    if (flagss == -1)
    {
        free(res);
        return NULL;
    }

    int tty= isatty(fd);
    if (tty ==0 && errno==EBADF)
    { 
        free(res);
        return NULL;
    }

    enum stream_buffering sbm;
    if (tty==1)
        sbm = STREAM_LINE_BUFFERED;
    else
        sbm = STREAM_BUFFERED;
    
    size_t alreadyread = 0;
    size_t bufferedsize = 0;
    enum stream_io_operation siop = STREAM_READING;
    
    res->flags = flagss;
    res->fd = fd;
    res->error = 0;
    res->io_operation = siop;
    res->buffering_mode = sbm;
    res->already_read = alreadyread;
    res->buffered_size = bufferedsize;

    return res;
}

struct stream *lbs_fopen(const char *path, const char *mode)
{
    if (path == NULL || mode == NULL)
        return NULL;
    int tmp = modegestion(mode);
    if (tmp == -1)
        return NULL;
    int fd = open(path, tmp, 0666);
    if (fd == -1)
    {
        return NULL;
    }
    return lbs_fdopen(fd, mode);
}

int lbs_setbufmode(struct stream *stream, enum stream_buffering mode)
{
    if (stream == NULL)
        return 1;
    stream->buffering_mode = mode;
    /*if (stream->buffering_mode==NULL)
        return 1;*/
    return 0;
}
