#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t program[] = {
    1, 'H',
    1, 'e',
    1, 'l',
    1, 'l',
    1, 'o',
    1, ',',
    1, ' ',
    1, 'W',
    1, 'o',
    1, 'r',
    1, 'l',
    1, 'd',
    1, '!',
    1, '\n',
    0
};

enum Instruction
{
    HALT        = 0x00,
    PRINT_CHAR  = 0x01
};

static uint8_t pc = 0;

void run(uint8_t* program);

int main()
{
    run(program);
}

void run(uint8_t* program)
{
    uint8_t opcode = opcode;
    uint8_t arg = program[pc + 1];

    while (opcode != HALT)
    {
        switch (opcode)
        {
            case PRINT_CHAR:
                putchar(arg);
                break;
            default:
                fprintf(stderr, "error: unknown opcode '0x%02x' at '0x%02x'\n", opcode, pc);
                exit(1);
        }

        pc += 2;
    }
}
