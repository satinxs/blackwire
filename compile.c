#include "vm.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* strsep(char** stringp, const char* delim)
{
    char* start = *stringp;
    char* p;
    p = (start != NULL) ? strpbrk(start, delim) : NULL;
    if (p == NULL)
        *stringp = NULL;
    else {
        *p = '\0';
        *stringp = p + 1;
    }
    return start;
}

static int strcount(const char* s, char c)
{
    int i;
    for (i = 0; s[i]; s[i] == c ? i++ : *s++)
        ;
    return i;
}

static void rtrim(char* str)
{
    size_t n = strlen(str);
    while (n > 0 && isspace(str[n - 1]))
        n--;
    str[n] = '\0';
}

static void ltrim(char* str)
{
    size_t n = 0;
    while (str[n] != '\0' && isspace(str[n]))
        n++;
    memmove(str, str + n, strlen(str) - n + 1);
}

static inline void trim(char* str)
{
    rtrim(str);
    ltrim(str);
}

static const char* Tokens[] = {
    "add",
    "sub",
    "mul",
    "div",
    "mod",
    "and",
    "or",
    "not",
    "incr",
    "decr",
    "cmp",
    "jump",
    "jumpf",
    "jumpt",
    "load",
    "pop",
    "push",
    "peek",
    "printi",
    "printc",
    "readi",
    "readc",
    "dump",
    "stop",
    "nop",
    "clock",
};

typedef struct
{
    char* label;
    int line;
} Label;

static int fsize(FILE* f)
{
    int n;
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    rewind(f);
    return n;
}

static inline int getop(const char* s)
{
    int i;
    for (i = 0; i < 26; i++)
        if (strcmp(s, Tokens[i]) == 0)
            return i;
    return -1;
}

static inline int toLines(char* str, char*** L)
{
    char *line, *tmp, *s = strdup(str);
    int l = strcount(s, '\n') + 1;
    int lcount = 0;
    (*L) = malloc(sizeof(char*) * l);
    while ((line = strsep(&s, "\n")) != NULL) {
        tmp = strdup(line);
        trim(tmp);
        if (tmp[0] == '#') {
            free(tmp);
            tmp = strdup("");
        }
        (*L)[lcount++] = tmp;
    }
    free(s);
    return lcount;
}

static inline int getLabels(char** L, int lines, Label** Ls)
{
    int c = 0, i, empty = 0;
    char label[100];
    for (i = 0; i < lines; i++) {
        if (strlen(L[i]) == 0) {
            empty++;
            continue;
        }
        if (L[i][0] == ':') {
            sscanf(L[i], ":%s ", label);
            (*Ls)[c].label = strdup(label);
            (*Ls)[c].line = (i - c) - empty;
            c++;
        }
    }
    return c;
}

static inline int my_getline(Label* Ls, int labelcount, char* s)
{
    int i;
    for (i = 0; i < labelcount; i++)
        if (strcmp(Ls[i].label, s) == 0)
            return Ls[i].line;
    return -1;
}

static inline int getcmp(char* s)
{
    if (strcmp(s, "eq") == 0) {
        return CMP_EQ;
    } else if (strcmp(s, "lt")) {
        return CMP_LT;
    } else if (strcmp(s, "st")) {
        return CMP_ST;
    }
    return CMP_EQ;
}

static inline Instruction* processLines(char** L, int linecount, Label* Ls, int labelcount)
{
    Instruction* prog = (Instruction*)calloc(linecount - labelcount, sizeof(Instruction));
    char op[32], arg[32];
    int o = 0, a = 0, i, p = 0;
    for (i = 0; i < linecount; i++) {
        if (strlen(L[i]) == 0)
            continue;

        sscanf(L[i], "%s %s", op, arg);
        if (op[0] == ':')
            continue;

        o = getop(op);

        if (o == -1) {
            printf("[Error]: Unknown opcode '%s' in line %d.\n", op, i + 1);
            exit(-1);
        }

        if (o == JUMP || o == JUMPF || o == JUMPT) {
            a = my_getline(Ls, labelcount, arg);
            if (a < 0 || a >= (linecount - labelcount)) {
                printf("[Error]: Unknown label '%s' in line %d.\n", arg, i + 1);
                exit(-1);
            }
        } else if (o == CMP)
            a = getcmp(arg);
        else
            a = atoi(arg);

        prog[p].op = o;
        prog[p++].data = a;
    }
    return prog;
}

extern BlackProgram* black_Compile(char* str)
{
    BlackProgram* prog = (BlackProgram*)calloc(1, sizeof(BlackProgram));
    Label* Labels;
    char** Lines;
    int labelcount, linecount;
    linecount = toLines(str, &Lines);
    Labels = (Label*)calloc(strcount(str, ':'), sizeof(Label));
    labelcount = getLabels(Lines, linecount, &Labels);
    prog->prog = processLines(Lines, linecount, Labels, labelcount);
    prog->length = linecount - labelcount;
    int i;
    for (i = 0; i < linecount; i++)
        free(Lines[0]);
    free(Lines);
    for (i = 0; i < labelcount; i++) {
        free(Labels[i].label);
    }
    free(Labels);
    prog->ip = 0;
    prog->sp = -1;
    prog->running = 1;
    return prog;
}

extern BlackProgram* black_CompileFile(char* filename)
{
    FILE* f = fopen(filename, "rb");

    if (f == NULL) {
        printf("Error reading file %s.\n", filename);
        exit(-1);
    }

    size_t size = fsize(f);
    char* str = (char*)malloc(size + 1);
    fread(str, 1, size, f);
    str[size] = 0;

    fclose(f);
    return black_Compile(str);
}

int black_ToFile(BlackProgram* p, char* filename)
{
    FILE* f = fopen(filename, "wb");

    if (f == NULL) {
        printf("Error opening file %s.\n", filename);
        exit(-1);
    }

    fputc('B', f);
    fputc('K', f);
    fputc('C', f);
    for (int i = 0; i < p->length; i++) {
        Instruction* ins = &p->prog[i];
        fputc(ins->op, f);

        int n = ins->data;
        fputc((n >> 24) & 0xFF, f);
        fputc((n >> 16) & 0xFF, f);
        fputc((n >> 8) & 0xFF, f);
        fputc(n & 0xFF, f);
    }

    fflush(f);
    fclose(f);

    return 1;
}