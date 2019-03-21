This vm has:
8 registers(int) and a stack of 255(int) (configurable from vm.h),
labels which have a colon (':') and any characters which are _not spaces_,

25 opcodes, which are:
//Arithmetics
    ADD //adds two values from stack, pushes back result
    SUB //same, substracting
    MUL //same, multiplying
    DIV //same, doing integer division
    MOD //same, but pushes modulus operation
//Logical operators
    CMP {EQ,LT,ST}//compares two values popped by EQual, LargerThan or SmallerThan
    AND //pops two values from stack and pushes logical AND (&&)
    OR // same, with logical OR (||)
    NOT //pops one value and pushes logical not (!)
//Register arithmetics    
    INCR r // increments value of register r by 1
    DECR r // decrements value of register r by 1
//Control flow
    JUMP n // jumps to instruction n
    JUMPF n // pops and if value is false(<=0) jumps to n
    JUMPT n // pops and if value is true (>0) jumps to n
//Stack control    
    LOAD n // pushes n to stack
    POP r //pops value from stack and stores in register r
    PUSH r //pushes value from register r to stack
    PEEK r //stores topmost value of stack in r but does not remove it
 //Basic IO
    PRINTI //pops and prints an integer
    PRINTC // same, but prints as character (33 == '!'), etc
    READI // reads and pushes a value
    READC // same, but reading a single stroke
//Executing control
    DUMP // dumps information about stack and registers
    STOP //stops execution of program. Necesary at program's end
    NOP //No OPeration, does nothing.
    
Warning, if you're gonna modify the MAXREG in vm.h, you'll have to modify void dump(BlackProgram *p); in vm.c!