#include "utils.h"
#include <cassert>

int main(int argc, char** argv) {
    // test empty vector
    vector<int> ints;
    assert(ints.size() == 0);

    // test simple pushing behavior
    ints.push(1);
    ints.push(2);
    ints.push(3);
    assert(ints.size() == 3);
    assert(ints[0] == 1);
    assert(ints[1] == 2);
    assert(ints[2] == 3);
    
    // test negative indexing
    assert(ints[-1] == 3);
    assert(ints[-2] == 2);
    assert(ints[-3] == 1);

    // test element removal
    ints.pop();
    assert(ints.size() == 2);
    assert(ints[-1] == 2);
    assert(ints[0] = 1);

    // test clear
    ints.clear();
    assert(ints.size() == 0);
}