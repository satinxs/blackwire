#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void dump(BlackProgram* p)
{
    int i = 0;
    printf("   Registers:\n");
    printf("   %d %d %d\n", p->reg[0], p->reg[1], p->reg[2]);
    printf("   %d %d %d\n", p->reg[3], p->reg[4], p->reg[5]);
    printf("   %d %d %d\n", p->reg[6], p->reg[7], p->reg[8]);
    printf("   Stack: [");
    i = p->sp;
    while (i > -1)
        printf("%d, ", p->stack[i--]);
    printf("]\n");

    printf("ip=%d, sp=%d\n", p->ip, p->sp);
}

static void vm_error(int code, int extra, BlackProgram* p)
{
    switch (code) {
    case ER_DUMP:
        printf("Dumping:\n");
        dump(p);
        return;
        break;
    case STACK_FULL:
        printf("Trying to push to a full stack!\n");
        break;
    case STACK_EMPTY:
        printf("Trying to pop from an empty stack.\n");
        break;
    case UNKNOWN_OPCODE:
        printf("Unknown opcode %d!\n", extra);
        break;
    default:
        printf("Unkown error! :C\n\n");
    }
    dump(p);
}

static inline int pop(BlackProgram* p)
{
    if (p->sp < 0) {
        vm_error(STACK_EMPTY, 0, p);
        exit(-1);
    }
    return p->stack[p->sp--];
}

static inline void push(BlackProgram* p, int n)
{
    if (p->sp == MAXSTACK - 1) {
        vm_error(STACK_FULL, 0, p);
        exit(-1);
    }
    p->stack[++p->sp] = n;
}

extern int black_Step(BlackProgram* p)
{
    int tmp;
    Instruction* op;
    if (p->running && p->ip < p->length && p->ip > -1) {
        op = &p->prog[p->ip];
        p->ip++;

        switch (op->op) {
        //Arithmetic
        case ADD:
            tmp = pop(p);
            push(p, pop(p) + tmp);
            break;
        case SUB:
            tmp = pop(p);
            push(p, pop(p) - tmp);
            break;
        case MUL:
            tmp = pop(p);
            push(p, pop(p) * tmp);
            break;
        case DIV:
            tmp = pop(p);
            push(p, pop(p) / tmp);
            break;
        case MOD:
            tmp = pop(p);
            push(p, pop(p) % tmp);
            break;
        //Logical operators
        case AND:
            tmp = pop(p);
            push(p, pop(p) && tmp);
            break;
        case OR:
            tmp = pop(p);
            push(p, pop(p) || tmp);
            break;
        case NOT:
            push(p, !pop(p));
            break;
        //Register Arithmetic
        case INCR:
            p->reg[op->data]++;
            break;
        case DECR:
            p->reg[op->data]--;
            break;
        //Stack Operations
        case LOAD:
            push(p, op->data);
            break;
        case POP:
            p->reg[op->data] = pop(p);
            break;
        case PUSH:
            push(p, p->reg[op->data]);
            break;
        case PEEK:
            p->reg[op->data] = pop(p);
            p->sp++;
            break;

        //Control Flow?
        case CMP:
            tmp = pop(p);
            if (op->data == CMP_LT)
                push(p, tmp > pop(p));
            else if (op->data == CMP_ST)
                push(p, tmp < pop(p));
            else //Falta una guarda!
                push(p, tmp == pop(p));
            break;

        case JUMP:
            p->ip = op->data;
            break;
        case JUMPF:
            if (pop(p) <= 0)
                p->ip = op->data;
            break;
        case JUMPT:
            if (pop(p) > 0)
                p->ip = op->data;
            break;

        //Basic IO
        case PRINTI:
            printf("%d", pop(p));
            break;
        case PRINTC:
            printf("%c", pop(p));
            break;
        case READI:
            scanf(" %d", &tmp);
            push(p, tmp);
            break;
        case READC:
            push(p, getchar());
            break;

        case NOP:
            break;
        case STOP:
            p->running = 0;
            if (p->sp >= 0)
                return pop(p);
            else
                return 0;
            break;
        case DUMP:
            vm_error(ER_DUMP, op->op, p);
            break;
        case CLOCK: {
            clock_t c = clock();
            int r = (int)((double)c / (double)CLOCKS_PER_SEC * 1000);
            printf("%d <- clock\n", r);
            push(p, r);
            break;
        }
        default:
            p->running = 0;
            vm_error(UNKNOWN_OPCODE, op->op, p);
            return -1;
        }

    } else
        p->running = 0;
    return 1;
}

extern void black_Dump(BlackProgram* p)
{
    dump(p);
}

extern int black_Run(BlackProgram* p)
{
    int r = 0;
    while (p->running)
        r = black_Step(p);
    return r;
}

extern void black_Destroy(BlackProgram* p)
{
    free(p->prog);
    free(p);
}
