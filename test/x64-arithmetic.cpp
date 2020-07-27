#include "utils.h"
#include "x64.h"
#include <cassert>
#include <cstdio>

int main(int argc, char** argv) {
    jasmine::Object obj;
    {
        using namespace x64;
        writeto(obj);

        sub(r64(RSP), imm(8));
        imul(r64(RDI), m64(RAX, RDX * 8, 64));
        mov(r64(RCX), imm(10));
        mov(r64(RAX), imm(1000));
        cdq();
        idiv(r64(RCX));
        add(r64(RSP), imm(8));
        ret();
    }

    while (obj.code().size()) printf("%02x ", obj.code().read());
    printf("\n");
}