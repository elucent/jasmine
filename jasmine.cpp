#include "obj.h"
#include <cstdio>
#include <cstring>

int print_usage() {
    printf("Usage: jasmine [-e <entry symbol>] <object file>\n");
    return 0;
}

int main(int argc, char** argv) {
    using namespace jasmine;
    if (argc < 2) return print_usage();

    const char* entry = "main";
    const char* path = nullptr;
    
    for (int i = 1; i < argc; i ++) {
        if (!strcmp(argv[i], "-e")) {
            if (i == argc - 1) return print_usage();
            entry = argv[++ i];
        }
        else if (!path) {
            path = argv[i];
        }
        else return print_usage();
    }

    if (!path) print_usage();

    Object obj(path);
    obj.load();
    return obj.find<int()>(global(entry))();
}