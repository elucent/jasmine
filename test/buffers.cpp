#include <cassert>
#include <cstdio>
#include <cstring>
#include "utils.h"

int main(int argc, char** argv) {
    // test empty buffer
    buffer b;
    assert(b.size() == 0);
    assert(b.peek() == '\0');

    // test simple FIFO read/write behavior
    b.write('a');
    b.write('b');
    b.write('c');
    assert(b.size() == 3);
    assert(b.peek() == 'a');
    assert(b.read() == 'a');
    assert(b.read() == 'b');
    assert(b.read() == 'c');
    assert(b.size() == 0);
    assert(b.peek() == '\0');

    // test large buffer (requires both wraparound and realloc)
    for (u8 i = 32; i < 128; i ++) b.write(i);
    assert(b.size() == 96);
    assert(b.peek() == ' ');
    
    // test copy ctor
    buffer b2(b);
    for (u8 i = 32; i < 64; i ++) assert(b.read() == i);
    assert(b2.peek() == ' ');
    assert(b.peek() == '@');
    assert(b2.size() == 96);
    assert(b.size() == 64);

    // test assignment operator
    b2 = b;
    assert(b2.peek() == '@');
    assert(b2.size() == 64);
    for (u8 i = 64; i < 128; i ++) assert(b.read() == i);
    assert(b2.peek() == '@');
    assert(b.peek() == '\0');
    assert(b2.size() == 64);
    assert(b.size() == 0);

    // test rotate buffer
    for (u8 i = 64; i < 96; i ++) b2.write(b2.read());
    assert(b2.size() == 64);
    assert(b2.peek() == '`');
    for (u8 i = 96; i < 128; i ++) assert(b2.read() == i);
    for (u8 i = 64; i < 96; i ++) assert(b2.read() == i);

    // test serialize
    b.write<i8>(100);
    b.write<u16>(200);
    b.write<i32>(-300);
    b.write<u64>(400);
    i64 result = ((b.read<i8>() + b.read<u16>()) / b.read<i32>()) * b.read<u64>();
    assert(result == -400);
}