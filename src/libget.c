#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "libstream.h"

int lbs_fputc(int c, struct stream *stream)
{
    //pas les perm pour ecrire
    if (stream_writable(stream) ==false)
    {
        stream->error=1;
        return LBS_EOF;
    }


    // convert depuis val ascii
    char ch = c;
    // pas bonne ope
    if (stream->io_operation == STREAM_READING)
    {
        int tmp = lbs_fflush(stream);
        if (tmp != 0)
        {
            stream->error = 1;
            return -1;
        }
        stream->io_operation = STREAM_WRITING;
        stream->buffered_size=0;
    }

    // buff big back plein :^]
    if (stream->buffered_size == LBS_BUFFER_SIZE)
    {
        int tmp = lbs_fflush(stream);
        if (tmp != 0)
        {
            stream->error = 1;
            return -1;
        }
    }

    stream->buffer[stream->buffered_size] = c;
    stream->buffered_size++;

    // buff mode unbuff
    if (stream->buffering_mode == STREAM_UNBUFFERED)
    {
        int tmp = lbs_fflush(stream);
        if (tmp != 0)
        {
            stream->error = 1;
            return -1;
        }
    }

    // buff mode linebuff and \n
    if (stream->buffering_mode == STREAM_LINE_BUFFERED && ch == '\n')
    {
        int tmp = lbs_fflush(stream);
        if (tmp != 0)
        {
            stream->error = 1;
            return -1;
        }
    }
    return c;
}

/*
** Reads a new character from the stream's buffer.
** If the buffer it empty, it should be refilled.
** Works just like fgetc(3). May set the error indicator.
**
** Returns LBS_EOF on failure or end of file.
*/
int lbs_fgetc(struct stream *stream)
{
	if (!stream_readable(stream))
	{
		stream->error=1;
		return LBS_EOF;
	}
    
    if (stream->io_operation == STREAM_WRITING)
    {
        int tmp = lbs_fflush(stream);
        if (tmp != 0)
        {
            stream->error = 1;
            return LBS_EOF;
        }
        stream->io_operation = STREAM_READING;
        stream->buffered_size = 0;
    }

    if (stream_remaining_buffered(stream) == 0)
    {
        int total = 0;
        while (total < LBS_BUFFER_SIZE)
        {
            ssize_t n= read(stream->fd, stream->buffer + total,
                        LBS_BUFFER_SIZE - total);
            if (n < 0)
            {
                stream->error = 1;
                return LBS_EOF;
            }
            if (n == 0)
                break;
            total += n;
        }
        stream->buffered_size = total;
        stream->already_read = 0;
        if (total==0)
            return LBS_EOF;
    }
    unsigned char c;
    memcpy(&c,stream->buffer + stream->already_read,1);
    int res=c;
    stream->already_read++;
    return res;
}

int lbs_fseek(struct stream *stream, long offset, int whence)
{

    if (lbs_fflush(stream) == EOF)
    {
        stream->error = 1;
        return -1;
    }
    off_t offset_loc = lseek(stream->fd, offset, whence);
    if (offset_loc == -1)
    {
        stream->error = 1;
        return -1;
    }
    stream->buffered_size = 0;
    stream->already_read = 0;
    return 0;
}

/*
** Returns the current position in the file, if it supports it.
** This function must not flush. It must take into account the offsets
** introduced by buffering, as well as the effects of the O_APPEND open(2)
** flag (using the positioning field of the struct stream).
** Works just like ftell(3).
**
** Returns your current offset on success, -1 on failure.
*/
long lbs_ftell(struct stream *stream)
{
    off_t pos = lseek(stream->fd, 0, stream_positioning(stream));
    if (pos == -1)
    {
        stream->error = 1;
        return -1;
    }
    if (stream->io_operation == STREAM_READING)
    {
        pos -= (stream->buffered_size) - (stream->already_read);
    }
    else if (stream->io_operation == STREAM_WRITING)
    {
        pos += stream->buffered_size;
    }
    return pos;
}
