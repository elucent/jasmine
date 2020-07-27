#include "utils.h"
#include <cstdio>
#include "x64.h"

int main(int argc, char** argv) {
    using namespace x64;
    using namespace jasmine;
    Object obj;
    writeto(obj);
    label(global("main"));
        sub(r64(RSP), imm(12));
        mov(m8(RSP, 0), imm8('h'));
        mov(m8(RSP, 1), imm8('e'));
        mov(m8(RSP, 2), imm8('l'));
        mov(m8(RSP, 3), imm8('l'));
        mov(m8(RSP, 4), imm8('o'));
        mov(m8(RSP, 5), imm8(' '));
        mov(m8(RSP, 6), imm8('w'));
        mov(m8(RSP, 7), imm8('o'));
        mov(m8(RSP, 8), imm8('r'));
        mov(m8(RSP, 9), imm8('l'));
        mov(m8(RSP, 10), imm8('d'));
        mov(m8(RSP, 11), imm8('\n'));
        mov(r64(RAX), imm(0x2000004)); 
        mov(r64(RDI), imm(1));
        mov(r64(RSI), r64(RSP)); 
        mov(r64(RDX), imm(12));
        syscall();
        add(r64(RSP), imm(12));
        ret();
    obj.write("hello");
}

