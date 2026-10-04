#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libstream.h"

/*
** Flushes the stream's buffer to the underlying file descriptor, making sure
** the stream position is correct. When there is some write data buffered, it
** has to be written. When there is some read data buffered, it has to be
** discarded and the process must seek the file descriptor back to the
** position the user expects.
**
** Works just like fflush(3), except:
**  - lbs_fflush() *DOES NOT* flush all open output streams if stream is NULL.
** May set the error indicator.
**
** Returns 0 on success, LBS_EOF on failure.
*/
int lbs_fflush(struct stream *stream)
{
    if (stream == NULL)
        return LBS_EOF;
    int fd = stream->fd;
    if (fd == -1)
    {
        stream->error = 1;
        return LBS_EOF;
    }
    if (stream->io_operation == STREAM_WRITING) // STREAM_WR
    {
        size_t size = stream->buffered_size;
        size_t total = 0;
        while (total < size)
        {
            ssize_t written = write(fd, stream->buffer + total, size - total);
            if (written == -1)
            {
                stream->error = 1;
                return LBS_EOF;
            }
            total += written;
        }
    }
    else if (stream->io_operation == STREAM_READING) // STREAM_RD
    {
        off_t offset = stream_remaining_buffered(stream);
        if (offset != 0)
        {
            off_t offset_loc = lseek(fd, -offset, SEEK_CUR);
            if (offset_loc == -1)
            {
                stream->error = 1;
                return LBS_EOF;
            }
        }
    }
    stream->buffered_size = 0;
    stream->already_read = 0;
    return 0;
}

int lbs_fclose(struct stream *stream)
{
    if (lbs_fflush(stream) == LBS_EOF)
    {
        return LBS_EOF;
    }
    int fd = stream->fd;
    int clz = close(fd);
    if (clz == -1)
        return LBS_EOF;
    free(stream);
    return 0;
}
