#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INDENT "    "

void die(FILE *f, const char *msg)
{
    printf("%s at offset %ld\n", msg, ftell(f));
    exit(1);
}

void eatSpace(FILE *f)
{
    int comment = 0;
    int c;

    while (!feof(f)) {
        c = fgetc(f);
        if (comment) {
            if (c == '\n')
                comment = 0;
        } else {
            if (c == '#')
                comment = 1;
            else if (!strchr(" \t\n\r", c)) {
                ungetc(c, f);
                return;
            }
        }
    }
}

int main(int argc, char **argv)
{
    char varBuf[64];
    char incBuf[64 + 32];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <bitmap.pgm>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("fopen");
        return 1;
    }

    const char *name = strrchr(argv[1], '/');
    name = name ? name + 1 : argv[1];
    strncpy(varBuf, name, sizeof(varBuf) - 1);
    varBuf[sizeof(varBuf) - 1] = 0;
    for (char *p = varBuf; *p; ++p) {
        if (*p == '.') {
            *p = 0;
            break;
        }
    }

    strcpy(incBuf, "OCHER_ICON_");
    strcat(incBuf, varBuf);
    strcat(incBuf, "_H");
    for (char *p = incBuf; *p; ++p) {
        *p = toupper(*p);
    }

    printf("#ifndef %s\n", incBuf);
    printf("#define %s\n\n", incBuf);
    printf("#include \"ocher/ux/fb/Widgets.h\"\n\n\n");

    char buf[80];
    unsigned int x, y, maxVal;

    if (fread(buf, 1, 2, f) != 2 || buf[0] != 'P' || buf[1] != '5') {
        die(f, "Missing P5 signature");
    }
    eatSpace(f);
    if (fscanf(f, "%u", &x) != 1) {
        die(f, "Missing width");
    }
    eatSpace(f);
    if (fscanf(f, "%u", &y) != 1) {
        die(f, "Missing height");
    }
    eatSpace(f);
    if (fscanf(f, "%u", &maxVal) != 1) {
        die(f, "Missing max value");
    }
    fgetc(f);  // Usually newline

    printf("static const unsigned char _%s[] = {\n" INDENT, varBuf);
    int i = 0;
    while (1) {
        unsigned char c;
        if (fread(&c, 1, 1, f) != 1)
            break;
        if (++i > 16) {
            i = 1;
            printf("\n" INDENT);
        }
        printf("0x%02x, ", c);
    }
    printf("\n};\n");

    printf("Bitmap bmp%s(%u, %u, _%s);\n", varBuf, x, y, varBuf);
    printf("#endif\n");

    fclose(f);

    return 0;
}
