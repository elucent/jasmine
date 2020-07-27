#include "utils.h"
#include <cstdio>
#include "x64.h"

int main(int argc, char** argv) {
    using namespace x64;
    using namespace jasmine;
    Object obj;
    writeto(obj);
    label(global("factorial"));
        cmp(r64(RDI), imm(0));
        jcc(label8(local(".basecase")), EQUAL);
        push(r64(RDI));
        sub(r64(RDI), imm(1));
        call(label8(global("factorial")));
        pop(r64(RDI));
        imul(r64(RAX), r64(RDI));
        ret();
    label(local(".basecase"));
        mov(r64(RAX), imm(1));
        ret();

    obj.load();
    int i = obj.find<int(int)>(jasmine::global("factorial"))(10);
    printf("%d\n", i);
}

