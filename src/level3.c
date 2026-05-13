#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t program1[] = {
    2, 65,
    4,
    5, 0,
    0
};
uint8_t program1_size = sizeof(program1);

uint8_t program2[] = {
    2, 3,
    2, 65,
    4,
    2, 255,
    3,
    6, 12,
    5, 2,
    0
};
uint8_t program2_size = sizeof(program2);

uint8_t stack[256];

typedef struct {
    uint8_t* program;
    uint8_t size;
} Program;

enum Instruction
{
    HALT            = 0x00,
    PRINT_CHAR      = 0x01,
    PUSH            = 0x02,
    ADD             = 0x03,
    PRINT_TOP       = 0x04,
    JUMP            = 0x05,
    JUMP_IF_ZERO    = 0x06
};

static uint8_t pc;
static uint8_t sp;

void run(Program* program);

int main()
{
    Program p1 = {program1, program1_size};
    Program p2 = {program2, program2_size};
    //run(&p1);
    run(&p2);
}

#define ASSERT_STACK_FULL()                         \
    do                                              \
    {                                               \
        if (sp >= 255)                              \
        {                                           \
            fprintf(stderr, "error: stack full\n"); \
            return;                                 \
        }                                           \
    } while (0)

#define ASSERT_STACK_EMPTY()                            \
    do                                                  \
    {                                                   \
        if (sp <= 0)                                    \
        {                                               \
            fprintf(stderr, "error: stack empty\n");    \
            return;                                     \
        }                                               \
    } while (0)

#define ASSERT_PC_OUTBOUND(pc, len)                                                     \
    do                                                                                  \
    {                                                                                   \
        if (pc >= len)                                                                  \
        {                                                                               \
            fprintf(stderr, "error: pc value '%02x' out-of-bounds (%02x)\n", pc, len);  \
            return;                                                                     \
        }                                                                               \
    } while (0)

void run(Program* program)
{
    pc = 0;
    sp = 0;

    while (pc < program->size)
    {
        uint8_t opcode = program->program[pc];
        if (opcode == HALT) break;
        
        uint8_t val1;
        uint8_t val2;

        switch (opcode)
        {
            case PRINT_CHAR:
                ASSERT_PC_OUTBOUND(pc + 1, program->size);
                putchar(program->program[pc + 1]);
                pc += 2;
                break;
            case PUSH:
                ASSERT_PC_OUTBOUND(pc + 1, program->size);
                ASSERT_STACK_FULL();
                sp += 1;
                stack[sp] = program->program[pc + 1];
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
            case JUMP:
                val1 = program->program[pc + 1];
                ASSERT_PC_OUTBOUND(val1, program->size);
                pc = val1;
                break;
            case JUMP_IF_ZERO:
                ASSERT_STACK_EMPTY();
                ASSERT_PC_OUTBOUND(pc + 1, program->size);
                if (stack[sp] == 0)
                {
                    val1 = program->program[pc + 1];
                    ASSERT_PC_OUTBOUND(val1, program->size);
                    pc = val1;
                } else pc += 2;
                break;
            default:
                fprintf(stderr, "error: unknown opcode '0x%02x' at '0x%02x'\n", opcode, pc);
                exit(1);
        }
    }
}
