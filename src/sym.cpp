#include "sym.h"
#include <cstring>

namespace jasmine {
    struct SymbolTable {
        vector<const char*> symbol_names;
        map<const char*, Symbol> symbols;
    
        SymbolTable() {}
        ~SymbolTable() {
            for (const char* str : symbol_names) delete[] str;
        }

        Symbol enter(const char* name, SymbolLinkage linkage) {
            auto it = symbols.find(name);
            if (it == symbols.end()) {
                u32 len = strlen(name) + 1;
                char* symbol_name = new char[len];
                memcpy(symbol_name, name, len);
                symbol_names.push(symbol_name);
                return symbols[symbol_name] = { symbol_names.size() - 1, linkage };
            }
            else return it->second;
        }
    };

    static SymbolTable table;

    bool operator==(Symbol a, Symbol b) {
        return a.id == b.id;
    }
    
    Symbol global(const char* name) {
        return table.enter(name, GLOBAL_SYMBOL);
    }

    Symbol local(const char* name) {
        return table.enter(name, LOCAL_SYMBOL);
    }

    const char* name(Symbol symbol) {
        return table.symbol_names[symbol.id];
    }
}