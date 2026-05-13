#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t program1[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 10 x INC
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 20
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 30
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 40
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 50
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 60
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 70
    1, 1,                           // 72
    5,                              // PRINT (prints 'H')
    0                               // HALT
};
uint8_t program1_size = sizeof(program1);

uint8_t program2[] = {
    1, 1, 1, 1, 1, 1, 1, 1,     // 8 x INC
    3,                          // RIGHT
    1, 1, 1, 1, 1, 1, 1, 1, 1,  // 9 x INC
    4,                          // LEFT
    2,                          // DEC
    8, 8,                       // JUMP_IF_NOT_ZERO 8 (jumps to RIGHT)
    3,                          // RIGHT
    5,                          // PRINT (prints 'H')
    0                           // HALT
};
uint8_t program2_size = sizeof(program2);

uint8_t tape[256];

typedef struct {
    uint8_t* program;
    uint8_t size;
} Program;

enum Instruction
{
    HALT                = 0x00,
    INC                 = 0x01,
    DEC                 = 0x02,
    RIGHT               = 0x03,
    LEFT                = 0x04,
    PRINT               = 0x05,
    READ                = 0x06,
    JUMP_IF_ZERO        = 0x07,
    JUMP_IF_NOT_ZERO    = 0x08
};

static uint8_t pc;
static int dp;

void run(Program* program);

int main()
{
    Program p1 = {program1, program1_size};
    Program p2 = {program2, program2_size};
    //run(&p1);
    run(&p2);
}

#define ASSERT_TAPE_FULL()                          \
    do {                                            \
        if (dp >= 255) {                            \
            fprintf(stderr, "error: tape full\n");  \
            return;                                 \
        }                                           \
    } while (0)

#define ASSERT_TAPE_EMPTY()                             \
    do {                                                \
        if (dp <= 0) {                                  \
            fprintf(stderr, "error: tape empty\n");     \
            return;                                     \
        }                                               \
    } while (0)

#define ASSERT_PC_OUTBOUND(pc, len)                                                     \
    do {                                                                                \
        if (pc >= len) {                                                                \
            fprintf(stderr, "error: pc value '%02x' out-of-bounds (%02x)\n", pc, len);  \
            return;                                                                     \
        }                                                                               \
    } while (0)

void run(Program* program)
{
    pc = 0;
    dp = 0;

    while (pc < program->size)
    {
        uint8_t opcode = program->program[pc];
        if (opcode == HALT) break;
        
        uint8_t val1;
        uint8_t val2;

        switch (opcode)
        {
            case INC:
                tape[dp] += 1;
                pc +=1;
                break;
            case DEC:
                tape[dp] -= 1;
                pc +=1;
                break;
            case RIGHT:
                ASSERT_TAPE_FULL();
                dp += 1;
                pc += 1;
                break;
            case LEFT:
                ASSERT_TAPE_EMPTY();
                dp -= 1;
                pc += 1;
                break;
            case PRINT:
                putchar(tape[dp]);
                pc += 1;
                break;
            case READ:
                tape[dp] = getchar();
                pc += 1;
                break;
            case JUMP_IF_ZERO:
                if (tape[dp] == 0)
                {
                    ASSERT_PC_OUTBOUND(pc + 1, program->size);
                    pc = program->program[pc + 1];
                } else {
                    pc += 2;
                }
                break;
            case JUMP_IF_NOT_ZERO:
                if (tape[dp] != 0)
                {
                    ASSERT_PC_OUTBOUND(pc + 1, program->size);
                    pc = program->program[pc + 1];
                } else {
                    pc += 2;
                }
                break;
            default:
                fprintf(stderr, "error: unknown opcode '0x%02x' at '0x%02x'\n", opcode, pc);
                exit(1);
        }
    }
}
