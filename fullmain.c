#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "include/libstream.h"

static void demo_write(const char *path, const char *content)
{
    struct stream *out = lbs_fopen(path, "w");
    if (out == NULL)
    {
        fprintf(stderr, "Failed to open %s for writing\n", path);
        return;
    }
    size_t i = 0;
    while (content[i] != '\0')
    {
        lbs_fputc(content[i], out);
        i++;
    }
    lbs_fclose(out);
}

static void demo_read(const char *path)
{
    struct stream *in = lbs_fopen(path, "r");
    if (in == NULL)
    {
        fprintf(stderr, "Failed to open %s for reading\n", path);
        return;
    }
    printf("--- Reading back ---\n");
    int c;
    while ((c = lbs_fgetc(in)) != LBS_EOF)
        putchar(c);
    lbs_fclose(in);
}

static void demo_seek_tell(const char *path)
{
    struct stream *s = lbs_fopen(path, "r");
    if (s == NULL)
        return;

    printf("--- Seek & Tell ---\n");
    printf("Initial position: %ld\n", lbs_ftell(s));
    lbs_fseek(s, 5, SEEK_SET);
    printf("After seek(5): %ld\n", lbs_ftell(s));

    int c = lbs_fgetc(s);
    printf("Char at position 5: '%c'\n", c);
    printf("After fgetc: %ld\n", lbs_ftell(s));

    lbs_fclose(s);
}

static void demo_append(const char *path, const char *extra)
{
    struct stream *app = lbs_fopen(path, "a");
    if (app == NULL)
        return;
    printf("--- Append ---\n");
    size_t i = 0;
    while (extra[i] != '\0')
    {
        lbs_fputc(extra[i], app);
        i++;
    }
    lbs_fclose(app);
}

int main(int argc, char *argv[])
{
    const char *content = argc > 1 ? argv[1] : "Hello from libstream!\n";
    const char *path = "fulldemo.txt";

    printf("=== libstream demo ===\n\n");

    printf("--- Writing ---\n");
    printf("Writing: %s", content);
    demo_write(path, content);

    demo_read(path);
    putchar('\n');

    demo_seek_tell(path);
    putchar('\n');

    demo_append(path, "...appended!\n");
    demo_read(path);

    return 0;
}
