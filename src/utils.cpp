#include "utils.h"
#include <cstdlib>

void* operator new[](unsigned long n) {
    return malloc(n);
}

void* operator new(unsigned long n, void* p) {
    return p;
}

void operator delete[](void* ptr) {
    free(ptr);
}

buffer::buffer():
    _start(0), _end(0), _capacity(32), _data(new u8[_capacity]) {
    // starts with empty (uninitialized) buffer of 32 bytes
}

buffer::~buffer() {
    delete[] _data;
}

buffer::buffer(const buffer& other):
    _start(other._start), _end(other._end), 
    _capacity(other._capacity), _data(new u8[_capacity]) {

    // copies memory from start to end
    for (u64 i = _start; i != _end; i = (i + 1) & (_capacity - 1))
        _data[i] = other._data[i];
}

buffer& buffer::operator=(const buffer& other) {
    if (this != &other) {
        // free existing allocation
        delete[] _data;
        
        // copy from other
        _start = other._start;
        _end = other._end;
        _capacity = other._capacity;
        _data = new u8[_capacity];
        for (u64 i = _start; i != _end; i = (i + 1) & (_capacity - 1))
            _data[i] = other._data[i];
    }
    return *this;
}

u8 buffer::peek() const {
    // empty buffer returns null char
    if (_start == _end) return '\0';

    // return next byte
    return _data[_start];
}

u8 buffer::read() {
    // empty buffer returns null char
    if (_start == _end) return '\0';

    // read next byte
    u8 byte = _data[_start];
    _start = (_start + 1) & (_capacity - 1);
    return byte;
}

void buffer::read(char* buffer, u64 length) {
    for (u64 i = 0; i < length; i ++)
        buffer[i] = read();
}

void buffer::write(u8 byte) {
    u64 new_end = (_end + 1) & (_capacity - 1);
    if (new_end == _start) {
        // hold onto old buffer
        u64 old_start = _start, old_end = _end, old_capacity = _capacity;
        u8* old_data = _data;

        // grow and clear buffer
        _start = 0;
        _end = 0;
        _capacity *= 2;
        _data = new u8[_capacity];

        // copy from old buffer
        while (old_start != old_end) {
            write(old_data[old_start]);
            old_start = (old_start + 1) & (old_capacity - 1);
        }

        // free old buffer
        delete[] old_data;
    }

    _data[_end] = byte;
    _end = (_end + 1) & (_capacity - 1);
}

void buffer::write(const char* string, u64 length) {
    for (u64 i = 0; i < length; i ++)
        write((u8)string[i]);
}

u64 buffer::size() const {
    u64 end = _end;
    if (end < _start) end += _capacity; // account for wraparound
    return end - _start;
}

void buffer::clear() {
    _start = _end;
}

u64 rotl(u64 u, u64 n) {
	return (u << n) | ((u >> (64 - n)) & ~(-1 << n));
}

u64 rotr(u64 u, u64 n) {
	return (u >> n) | ((u << (64 - n)) & (-1 & ~(-1 << n)));
}

u64 raw_hash(const void* t, uint64_t size) {
	u64 h = 13830991727719723691ul;

	const u64* words = (const u64*)t;
    u32 i = 0;
	for (; i < size / 8; ++ i) {
	    uint64_t u = *words * 4695878395758459391ul;
		h -= u;
	    h ^= (h >> 23);
	    ++ words;
	}
	
	const u8* bytes = (const u8*)words;
	i *= 8;
	for (; i < size; ++ i) {
	    uint64_t u = *bytes * 4695878395758459391ul;
		h -= u;
	    h ^= (h >> 23);
	    ++ bytes;
	}
	return h ^ (h << 37);
}

template<>
bool equals(const char* const& a, const char* const& b) {
    const char *a_ptr = a, *b_ptr = b;
    while (*a_ptr && *a_ptr == *b_ptr) ++ a_ptr, ++ b_ptr;
    return *a_ptr == *b_ptr;
}

template<>
u64 hash(const char* const& s) {
    u32 size = 0;
    const char* sptr = s;
    while (*sptr) ++ sptr, ++ size;
    return raw_hash((const u8*)s, size);
}

#if defined(__APPLE__) || defined(__linux__)
    #include "sys/mman.h"

    void* alloc_exec(u64 size) {
        return mmap(nullptr, size, PROT_READ | PROT_EXEC | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    }

    void protect_exec(void* exec, u64 size) {
        mprotect(exec, size, PROT_READ | PROT_EXEC);
    }

    void free_exec(void* exec, u64 size) {
        munmap(exec, size);
    }
#endif