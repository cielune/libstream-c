#ifndef LIBSTREAM_H
#define LIBSTREAM_H

#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

/*
** /!\ DO NOT MODIFY THIS FILE, AS IT WILL BE OVERRIDDEN DURING CORRECTION. /!\
**
** You can add your own functions declarations to OTHER HEADER FILES.
*/

/* the value returned when end of file is reached */
#define LBS_EOF (-1)

/* the size of the buffer */
#define LBS_BUFFER_SIZE 32

/*
** Describes the current operation:
**  - if reading, the buffer contains read-buffered data
**  - if writing, the buffer contains write-buffered data
*/
enum stream_io_operation
{
    STREAM_READING = 0,
    STREAM_WRITING,
};

/*
** Controls when to flush the buffer:
**  - when unbuffered, flush every time a character is written
**  - when buffered, flush when the buffer is full
**  - when line buffered, flush when the buffer is full or when a \n
**    character is written
*/
enum stream_buffering
{
    STREAM_BUFFERED = 0,
    STREAM_LINE_BUFFERED,
    STREAM_UNBUFFERED,
};

struct stream
{
    /* the flags passed to open */
    int flags;

    /*
    ** Initially, this variable is 0.
    ** When a function such as fgetc fails, it is set to 1 to indicate
    ** something went wrong. This is useful to make the difference between
    ** reaching the end of file and read errors while using fgetc and
    ** some others.
    ** It is often referred to as the error indicator.
    */
    int error;

    /* the file descriptor, as returned by open(2) */
    int fd;

    /*
    ** the kind of data stored by the buffer.
    ** The default value shouldn't matter.
    */
    enum stream_io_operation io_operation;

    /*
    ** defines when to flush **output**.
    ** This field does not control input buffering (which is always fully
    ** buffered).
    **
    ** The default value is STREAM_LINE_BUFFERED if isatty(fd), STREAM_BUFFERED
    * otherwise.
    */
    enum stream_buffering buffering_mode;

    /* the amount of used bytes in the buffer */
    size_t buffered_size;

    /*
    ** /!\ This field only makes sense when io_operation is STREAM_READING /!\
    ** the amount of data already read from the buffer by the user.
    */
    size_t already_read;

    /*
    **                   buffer
    **               -------------->
    ** +==============+====================+---------------------+
    ** | already_read | remaining_buffered | unused_buffer_space |
    ** +==============+====================+---------------------+
    **   \_______________________________/
    **             buffered_size
    **
    ** /!\ The buffer can contain either read-buffered or write-buffered data,
    **     depending on the value of io_operation /!\
    */
    char buffer[LBS_BUFFER_SIZE];
};

/*
** These functions are defined in a header for optimization reasons:
** each .c file that includes this header will get its own copy of the
** function's code, thus easily make optimizations.
**
** ``static`` means each compilation unit (.c file) will have its own copy
** of the function without them clashing.
**
** ``inline`` means the content of the function should be "copy pasted"
** where it's called. It also tells the compiler not to complain when the
** function isn't used.
**
** They're just like a macro, except the type of arguments is checked.
*/

static inline size_t stream_remaining_buffered(struct stream *stream)
{
    return stream->buffered_size - stream->already_read;
}

static inline size_t stream_unused_buffer_space(struct stream *stream)
{
    return sizeof(stream->buffer) - stream->buffered_size;
}

/*
** Returns the position fseek should work from.
** It is only needed to handle the append mode, which is an advanced feature.
** No need to handle it from the beginning.
**
** This function is useful for handling a very special case of ftell.
** Consider the following sequence of actions:
**   FILE *f = fopen("test", "a");
**   printf("%ld\n", ftell(f));
**   fputc('a', f);
**   fseek(f, 0, SEEK_SET);
**   fputc('b', f);
**   printf("%ld\n", ftell(f));
**   fclose(f);
**
** This code prints "2", because after each write in append mode, the stream
** seeks to the end of file. ftell thus needs to take it into account.
**
** When to count from the end of file can be deduced from other parameters:
** if the stream contains write buffered data and the file was opened using
** O_APPEND, then ftell must be relative to the end.
**
** To handle the position just after open() in "a" and "a+" modes, a lseek
** does the trick.
*/
static inline int stream_positioning(struct stream *stream)
{
    if (stream->io_operation == STREAM_WRITING && (stream->flags & O_APPEND)
        && stream->buffered_size != 0)
        return SEEK_END;
    return SEEK_CUR;
}

static inline bool stream_readable(struct stream *stream)
{
    int access_mode = stream->flags & O_ACCMODE;
    return (access_mode == O_RDWR) || (access_mode == O_RDONLY);
}

static inline bool stream_writable(struct stream *stream)
{
    int access_mode = stream->flags & O_ACCMODE;
    return (access_mode == O_RDWR) || (access_mode == O_WRONLY);
}

static inline int lbs_ferror(struct stream *stream)
{
    return stream->error;
}

static inline void lbs_clearerr(struct stream *stream)
{
    stream->error = 0;
}

/*
** Initializes a stream structure with the given file descriptor and modes.
** It works just like lbs_fopen() except it takes the file descriptor
** instead of the path.
**
** It can be used by lbs_fopen() to make implementation clearer.
** It will also be useful to initialize streams to already open file
** descriptors, such as stdin, stdout and stderr.
**
** It may fail when calling isatty() and the file descriptor is invalid.
** Returns NULL on failure
*/
struct stream *lbs_fdopen(int fd, const char *mode);

/*
** Opens the file at some path with the flags described by mode, and wrap
** it in a buffered stream. It works just like fopen(3), except:
**  - you only have to handle the modes r, r+, w, w+, a and a+
**  - the initial read position when using the a+ mode is the end of file
**
** Returns NULL on failure
*/
struct stream *lbs_fopen(const char *path, const char *mode);

/*
** Changes the buffering mode of a stream.
** It must be called right after lbs_fopen() or lbs_fdopen().
**
** Returns 0 on success, -1 on failure.
*/
int lbs_setbufmode(struct stream *stream, enum stream_buffering mode);

/*
** Closes the stream and underlying file descriptor, flushing buffered data.
** No other function should be called on the stream after lbs_fclose(), as
** the stream is freed if the function returns successfully.
** Works just like fclose(3).
**
** Returns 0 on success, LBS_EOF on failure.
*/
int lbs_fclose(struct stream *stream);

/*
** Writes a single character to some stream.
** It may cause the stream to flush if the buffer is full or the current
** buffering policy requires it.
** Works just like fputc(3). May set the error indicator.
**
** Returns LBS_EOF on failure.
*/
int lbs_fputc(int c, struct stream *stream);

/*
** Reads a new character from the stream's buffer.
** If the buffer it empty, it should be refilled.
** Works just like fgetc(3). May set the error indicator.
**
** Returns LBS_EOF on failure or end of file.
*/
int lbs_fgetc(struct stream *stream);

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
int lbs_fflush(struct stream *stream);

/*
** Moves the stream position using lseek(2). It needs to flush the buffer's
** content.
** Works just like fseek(3). May set the error indicator on read / write errors.
**
** Returns 0 on success, -1 on failure.
*/
int lbs_fseek(struct stream *stream, long offset, int whence);

/*
** Returns the current position in the file, if it supports it.
** This function must not flush. It must take into account the offsets
** introduced by buffering, as well as the effects of the O_APPEND open(2)
** flag (using the positioning field of the struct stream).
** Works just like ftell(3).
**
** Returns your current offset on success, -1 on failure.
*/
long lbs_ftell(struct stream *stream);

#endif /* ! LIBSTREAM_H */
