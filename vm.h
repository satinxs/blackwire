#ifndef BLACKVM
#define BLACKVM
#define BVM_VERSION "0.1.1"

#define MAXREG 9
#define MAXSTACK 256

#define ER_DUMP -1
#define STACK_FULL 0
#define UNKNOWN_OPCODE 1
#define INVALID_JMP 2
#define STACK_EMPTY 3

typedef struct {
    char op;
    int data;
} Instruction;

typedef struct BlackProgram {
    Instruction *prog;
    int length;
    //Runtime variables:
    int reg[MAXREG];
    int ip, sp, running;
    int stack[MAXSTACK];
} BlackProgram;

enum {
    CMP_EQ,
    CMP_LT,
    CMP_ST,
};

enum {
    ADD, SUB, MUL, DIV, MOD,
    AND, OR, NOT, INCR, DECR,
    CMP, JUMP, JUMPF, JUMPT,
    LOAD, POP, PUSH, PEEK,
    PRINTI, PRINTC, READI, READC,
    DUMP, STOP, NOP,
    CLOCK
};

extern void black_Destroy(BlackProgram *p);
extern int black_Step(BlackProgram *p);
extern int black_Run(BlackProgram *p);
extern BlackProgram *black_CompileFile(char *filename);
extern BlackProgram *black_Compile(char *str);
extern void black_Dump(BlackProgram *p);
int black_ToFile(BlackProgram *p, char *filename);

#endif