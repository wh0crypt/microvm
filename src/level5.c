#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char program1[] = "++++++++[>+++++++++<-]>.";
size_t program1_size = (sizeof(program1) / sizeof(program1[0])) - 1;

char program2[] = "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.";
size_t program2_size = (sizeof(program2) / sizeof(program2[0])) - 1;

uint8_t tape[256];
size_t  match[256];

typedef struct
{
    char*   program;
    size_t  size;
} Program;

typedef enum
{
    HALT                = 0x00,
    INC                 = 0x01,
    DEC                 = 0x02,
    RIGHT               = 0x03,
    LEFT                = 0x04,
    PRINT               = 0x05,
    READ                = 0x06,
    JUMP_IF_ZERO        = 0x07,
    JUMP_IF_NOT_ZERO    = 0x08,
    UNK                 = 0xFF
} OpCodes;

static size_t pc;
static int dp;

void run(Program* program);
uint8_t get_op_from_char(char c);
void build_mapping_table(Program* program);

int main()
{
    Program p1 = {program1, program1_size};
    Program p2 = {program2, program2_size};
    //run(&p1);
    run(&p2);
}

#define ASSERT_TAPE_FULL()                          \
    do                                              \
    {                                               \
        if (dp >= 255)                              \
        {                                           \
            fprintf(stderr, "error: tape full\n");  \
            return;                                 \
        }                                           \
    } while (0)

#define ASSERT_TAPE_EMPTY()                             \
    do                                                  \
    {                                                   \
        if (dp <= 0)                                    \
        {                                               \
            fprintf(stderr, "error: tape empty\n");     \
            return;                                     \
        }                                               \
    } while (0)

#define ASSERT_PC_OUTBOUND(pc, len)                                                         \
    do                                                                                      \
    {                                                                                       \
        if (pc >= len)                                                                      \
        {                                                                                   \
            fprintf(stderr, "error: pc value '%02lx' out-of-bounds (%02lx)\n", pc, len);    \
            return;                                                                         \
        }                                                                                   \
    } while (0)

void run(Program* program)
{
    pc = 0;
    dp = 0;

    build_mapping_table(program);

    while (pc < program->size)
    {
        uint8_t opcode = get_op_from_char(program->program[pc]);
        if (opcode == HALT) break;
        
        uint8_t val1;
        uint8_t val2;

        switch (opcode)
        {
            case INC:
                tape[dp] += 1;
                pc += 1;
                break;
            case DEC:
                tape[dp] -= 1;
                pc += 1;
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
                    ASSERT_PC_OUTBOUND(match[pc] + 1, program->size);
                    pc = match[pc] + 1;
                }
                else pc += 1;
                break;
            case JUMP_IF_NOT_ZERO:
                if (tape[dp] != 0)
                {
                    ASSERT_PC_OUTBOUND(match[pc] + 1, program->size);
                    pc = match[pc] + 1;
                }
                else pc += 1;
                break;
            default:
                fprintf(stderr, "error: unknown opcode '0x%02x' at '0x%02lx'\n", opcode, pc);
                exit(1);
        }
    }
}

uint8_t get_op_from_char(char c)
{
    switch (c)
    {
        case '+':   return INC;
        case '-':   return DEC;
        case '>':   return RIGHT;
        case '<':   return LEFT;
        case '.':   return PRINT;
        case ',':   return READ;
        case '[':   return JUMP_IF_ZERO;
        case ']':   return JUMP_IF_NOT_ZERO;
        default:    return UNK;
    }
}

void build_mapping_table(Program* program)
{
    size_t last_open = 0;
    size_t depth = 0;
    size_t opens[program->size];

    for (size_t i = 0; i < program->size; ++i)
    {
        if (program->program[i] == '[')
        {
            opens[depth] = i;
            last_open = i;
            depth += 1;
        }

        if (program->program[i] == ']')
        {
            if (depth == 0) fprintf(stderr, "error: unmatched ']' at '%02lx' ('%c')\n", i, program->program[i]);
            depth -= 1;
            size_t open = opens[depth];
            match[i] = open;
            match[open] = i;
        }
    }

    if (depth != 0) fprintf(stderr, "error: unmatched ']' at '%02lx' ('%c')\n", last_open, program->program[last_open]);
}
