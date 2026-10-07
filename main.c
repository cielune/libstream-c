#include <stdio.h>
#include <string.h>
#include "include/libstream.h"


int main(void)
{
    //write
    struct stream *fileOut= lbs_fopen("demo.txt", "w");
    if (fileOut==NULL)
        return 1;
    lbs_fputc('H', fileOut); lbs_fputc('e', fileOut);
    lbs_fputc('l', fileOut); lbs_fputc('l', fileOut);
    lbs_fputc('o', fileOut); lbs_fputc(' ', fileOut);
    lbs_fputc('G', fileOut); lbs_fputc('i', fileOut);
    lbs_fputc('t', fileOut); lbs_fputc('H', fileOut);
    lbs_fputc('u', fileOut); lbs_fputc('b', fileOut);
    lbs_fputc('\n', fileOut);
    lbs_fclose(fileOut);

    //read
    struct stream *fileIn= lbs_fopen("demo.txt", "r");
    if (fileIn ==NULL)
        return 1;
    int ch;
    while ((ch=lbs_fgetc(fileIn)) != LBS_EOF)
        putchar(ch);
    lbs_fclose(fileIn);
    return 0;
}
