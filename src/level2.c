#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t program1[] = {
    2, 'H',
    4,
    2, 'i',
    4,
    0
};

uint8_t program2[] = {
    2, 65,
    2, 2,
    3,
    4,
    0
};

uint8_t stack[256];

enum Instruction
{
    HALT        = 0x00,
    PRINT_CHAR  = 0x01,
    PUSH        = 0x02,
    ADD         = 0x03,
    PRINT_TOP   = 0x04
};

static uint8_t pc;
static uint8_t sp;

void run(uint8_t* program);

int main()
{
    run(program1);
    run(program2);
}

#define ASSERT_STACK_FULL()                         \
    do {                                            \
        if (sp >= 255) {                            \
            fprintf(stderr, "error: stack full\n"); \
            return;                                 \
        }                                           \
    } while (0)

#define ASSERT_STACK_EMPTY()                            \
    do {                                                \
        if (sp <= 0) {                                  \
            fprintf(stderr, "error: stack empty\n");    \
            return;                                     \
        }                                               \
    } while (0)

void run(uint8_t* program)
{
    pc = 0;
    sp = 0;

    while (1)
    {
        uint8_t opcode = program[pc];
        if (opcode == HALT) break;
        uint8_t arg = program[pc + 1];
        
        uint8_t val1;
        uint8_t val2;

        switch (opcode)
        {
            case PRINT_CHAR:
                putchar(arg);
                pc += 2;
                break;
            case PUSH:
                ASSERT_STACK_FULL();
                sp += 1;
                stack[sp] = arg;
                pc += 2;
                break;
            case ADD:
                ASSERT_STACK_EMPTY();
                val1 = stack[sp];
                sp -= 1;
                ASSERT_STACK_EMPTY();
                val2 = stack[sp];
                sp -= 1;
                ASSERT_STACK_FULL();
                sp += 1; 
                stack[sp] = val1 + val2;
                pc += 1;
                break;
            case PRINT_TOP:
                ASSERT_STACK_EMPTY();
                val1 = stack[sp];
                sp -= 1;
                putchar(val1);
                pc += 1;
                break;
            default:
                fprintf(stderr, "error: unknown opcode '0x%02x' at '0x%02x'\n", opcode, pc);
                exit(1);
        }
    }
}
