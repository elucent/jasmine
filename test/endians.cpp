#include <cassert>
#include <cstdio>
#include <cstring>
#include "utils.h"

int main(int argc, char** argv) {
    // test two-byte endianness
    union {
        u16 n;
        u8 arr[2];
    } twobytes;
    twobytes.n = little_endian<u16>(51342ul);
    assert(twobytes.arr[0] == 0x8e);
    assert(twobytes.arr[1] == 0xc8);
    twobytes.n = big_endian<u16>(51342ul);
    assert(twobytes.arr[0] == 0xc8);
    assert(twobytes.arr[1] == 0x8e);

    // test four-byte endianness
    union {
        u32 n;
        u8 arr[4];
    } fourbytes;
    fourbytes.n = little_endian<u32>(479912201ul);
    assert(fourbytes.arr[0] == 0x09);
    assert(fourbytes.arr[1] == 0xe1);
    assert(fourbytes.arr[2] == 0x9a);
    assert(fourbytes.arr[3] == 0x1c);
    fourbytes.n = big_endian<u32>(479912201ul);
    assert(fourbytes.arr[0] == 0x1c);
    assert(fourbytes.arr[1] == 0x9a);
    assert(fourbytes.arr[2] == 0xe1);
    assert(fourbytes.arr[3] == 0x09);

    // test eight-byte endianness
    union {
        u64 n;
        u8 arr[8];
    } eightbytes;
    eightbytes.n = little_endian<u64>(6933819001501799241ul);
    assert(eightbytes.arr[0] == 0x49);
    assert(eightbytes.arr[1] == 0x53);
    assert(eightbytes.arr[2] == 0x5e);
    assert(eightbytes.arr[3] == 0x19);
    assert(eightbytes.arr[4] == 0xa5);
    assert(eightbytes.arr[5] == 0xdf);
    assert(eightbytes.arr[6] == 0x39);
    assert(eightbytes.arr[7] == 0x60);
    eightbytes.n = big_endian<u64>(6933819001501799241ul);
    assert(eightbytes.arr[0] == 0x60);
    assert(eightbytes.arr[1] == 0x39);
    assert(eightbytes.arr[2] == 0xdf);
    assert(eightbytes.arr[3] == 0xa5);
    assert(eightbytes.arr[4] == 0x19);
    assert(eightbytes.arr[5] == 0x5e);
    assert(eightbytes.arr[6] == 0x53);
    assert(eightbytes.arr[7] == 0x49);
}