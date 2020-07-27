#include "utils.h"
#include "x64.h"
#include <cstdio>

int main(int argc, char** argv) {
    using namespace x64;

    jasmine::Object obj;
    
    writeto(obj);
    label(jasmine::global("main"));
    mov(r64(RAX), imm(0x2000004)); 
    mov(r64(RDI), imm(1));
    mov(r64(RSI), imm((i64)"hello world\n")); 
    mov(r64(RDX), imm(12));
    syscall(); ret();

    obj.load();
    obj.find<void()>(jasmine::global("main"))();
}