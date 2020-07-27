#include "utils.h"
#include "x64.h"
#include <cstdio>

int main(int argc, char** argv) {
    using namespace x64;

    jasmine::Object obj;
    
    writeto(obj);
    label(jasmine::global("main"));
    mov(r64(RAX), imm(0));
    label(jasmine::local("loop"));
    add(r64(RAX), imm(1));
    cmp(r64(RAX), imm(10));
    jcc(label8(jasmine::local("loop")), LESS);
    ret();

    obj.load();
    int i = obj.find<int()>(jasmine::global("main"))();
    printf("%d\n", i);

    while (obj.code().size()) printf("%02x ", obj.code().read());
    printf("\n");
}