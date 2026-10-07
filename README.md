# libstream-c
Implementation of buffered I/O stream library in C



## Features
- `lbs_fopen`/ `lbs_fdopen` -> open a file or file descriptor as a buffered stream
- `lbs_fclose`              -> flush and close a stream
- `lbs_fputc`/ `lbs_fgetc`  -> write/read a single character
- `lbs_fflush`              -> flush the stream buffer
- `lbs_fseek`/ `lbs_ftell`  -> seek/tell position 
- `lbs_setbufmode`          -> set buffering mode (buffered, line-buffered, unbuffered)


### Buffering modes
| `STREAM_BUFFERED`         | Flush when buffer is full               
| `STREAM_LINE_BUFFERED`    | Flush on `\n` or when buffer is full    
| `STREAM_UNBUFFERED`       | Flush on every write                    




## Build
```bash
$ make all                      # builds libstream.a, demo and demo-full
$ make clean        
```


## Demo
```bash
./demo                          # writes and reads back "Hello Gihub"
./demo-full                     # full demo with "Hello Github" default text
./demo-full $'Your own text\n'  # full demo with "Hello Github" default text
```


## Implementation
Written in C99, using only low-level POSIX syscalls
