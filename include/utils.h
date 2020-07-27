#ifndef JASMINE_UTIL_H
#define JASMINE_UTIL_H

#include <cstdint>

void* operator new[](unsigned long n);
void* operator new(unsigned long n, void* p);
void operator delete[](void* ptr);

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

enum class EndianOrder : u32 {
    UTIL_LITTLE_ENDIAN = 0x03020100ul,
    UTIL_BIG_ENDIAN = 0x00010203ul,
    UTIL_PDP_ENDIAN = 0x01000302ul,     
    UTIL_HONEYWELL_ENDIAN = 0x02030001ul
};

static const union { 
    u8 bytes[4]; 
    u32 value; 
} host_order = { { 0, 1, 2, 3 } };

template<typename T>
T flip_endian(T value) {
    T result = 0;
    constexpr const i64 bits = sizeof(T) * 8;
    for (i64 i = bits - 8; i >= 0; i -= 8) {
        result |= ((value >> i) & 0xff) << ((bits - 8) - i); 
    }
    return result;
}

template<typename T>
T little_endian(T value) {
    if ((EndianOrder)host_order.value == EndianOrder::UTIL_LITTLE_ENDIAN)
        return value;
    return flip_endian(value);
}

template<typename T>
T big_endian(T value) {
    if ((EndianOrder)host_order.value == EndianOrder::UTIL_BIG_ENDIAN)
        return value;
    return flip_endian(value);
}

template<typename T>
T from_big_endian(T value) {
    if ((EndianOrder)host_order.value == EndianOrder::UTIL_BIG_ENDIAN)
        return value;
    return flip_endian(value);
}

template<typename T>
T from_little_endian(T value) {
    if ((EndianOrder)host_order.value == EndianOrder::UTIL_LITTLE_ENDIAN)
        return value;
    return flip_endian(value);
}

class buffer {
    u64 _start;
    u64 _end;
    u64 _capacity;
    u8* _data;
public:
    buffer();
    ~buffer();
    buffer(const buffer& other);
    buffer& operator=(const buffer& other);

    u8 peek() const;
    u8 read();
    void read(char* buffer, u64 length);
    void write(u8 byte);
    void write(const char* string, u64 length);
    u64 size() const;
    void clear();

    template<typename T>
    T read() {
        // serializes object from lowest to highest address
        static u8 buffer[sizeof(T)];
        for (u32 i = 0; i < sizeof(T); i ++) buffer[i] = buffer::read();
        return *(T*)buffer;
    }
    
    template<typename T>
    void write(const T& value) {
        // deserializes object, assuming first byte is lowest address
        const u8* data = (const u8*)&value;
        for (u32 i = 0; i < sizeof(T); i ++) write(data[i]);
    }

    template<typename... Args>
    void write(u8 b, Args... args) {
        write(b);
        write(args...);
    }
};

template<typename T>
class vector {
    u8* data;
    u32 _size, _capacity;

    void free(u8* array) {
        T* tptr = (T*)array;
        for (u32 i = 0; i < _size; i ++) tptr[i].~T();
        delete[] array;
    }

    void init(u32 size) {
        _size = 0, _capacity = size;
        data = new u8[_capacity * sizeof(T)];
    }

    void copy(const T* ts, u32 n) {
        _size = 0;
        T* tptr = (T*)data;
        for (u32 i = 0; i < n; i ++) {
            new(tptr + i) T(ts[i]);
            ++ _size;
        }
    }

    void destruct(u32 i) {
        T* tptr = (T*)data;
        tptr[i].~T();
    }

    void grow() {
        u8* old = data;
        u32 oldsize = _size;
        init(_capacity * 2);
        copy((const T*)old, oldsize);
        free(old);
    }

public:
    vector() {
        init(16);
    }

    ~vector() {
        free(data);
    }

    vector(const vector& other) {
        init(other._capacity);
        copy((const T*)other.data, other._size);
    }

    vector& operator=(const vector& other) {
        if (this != &other) {
            free(data);
            init(other._capacity);
            copy((const T*)other.data, other._size);
        }
        return *this;
    }

    void push(const T& t) {
        while (_size + 1 >= _capacity) grow();
        T* tptr = (T*)data;
        new(tptr + _size) T(t);
        ++ _size;
    }
    
    void pop() {
        -- _size;
        destruct(_size);
    }

    void clear() {
        for (u32 i = 0; i < _size; ++ i) destruct(i);
        _size = 0;
    }

    const T& operator[](i64 i) const {
        if (i < 0) i += _size;
        return ((T*)data)[i];
    }

    T& operator[](i64 i) {
        if (i < 0) i += _size;
        return ((T*)data)[i];
    }

    const T* begin() const {
        return (const T*)data;
    }

    T* begin() {
        return (T*)data;
    }

    const T* end() const {
        return (const T*)data + _size;
    }

    T* end() {
        return (T*)data + _size;
    }

    u32 size() const {
        return _size;
    }

    u32 capacity() const {
        return _capacity;
    }
};
template<typename T, typename U>
struct pair {
	T first;
	U second;
    bool operator==(const pair& other) const {
        return first == other.first && second == other.second;
    }
};

template<typename T>
bool equals(const T& a, const T& b) {
    return a == b;
}

template<>
bool equals(const char* const& a, const char* const& b);

template<typename T>
bool key_equals(const T& a, const T& b) {
    return equals(a.first, b.first);
}

u64 rotl(u64 u, u64 n);
u64 rotr(u64 u, u64 n);
u64 raw_hash(const void* t, uint64_t size);

template<typename T>
u64 hash(const T& t) {
    return raw_hash((const u8*)&t, sizeof(T));
}

template<>
u64 hash(const char* const& s);

template<typename T>
u64 key_hash(const T& a) {
    return hash(a.first);
}

template<typename T>
class set {
    enum bucket_status {
        EMPTY, GHOST, FILLED
    };

    struct bucket {
        u8 data[sizeof(T)];
        bucket_status status;

        bucket(): status(EMPTY) {
            //
        }

        ~bucket() {
            if (status == FILLED) (*(T*)data).~T();
        }

        bucket(const bucket& other): status(other.status) {
            if (status == FILLED) new(data) T(*(T*)other.data);
        }

        bucket& operator=(const bucket& other) {
            if (this != &other) {
                evict();
                status = other.status;
                if (status == FILLED) new(data) T(*(T*)other.data);
            }
            return *this;
        }

        inline const T& value() const {
            return *(const T*)data;
        }

        inline T& value() {
            return *(T*)data;
        }

        inline void fill(const T& value) {
            if (status != FILLED) {
                status = FILLED, new(data) T(value);
            }
            else this->value() = value;
        }

        inline void evict() {
            if (status == FILLED) {
                (*(T*)data).~T();
                status = GHOST;
            }
        }

        inline void clear() {
            if (status == FILLED) (*(T*)data).~T();
            status = EMPTY;
        }
    };

    bucket* data;
    u32 _size, _capacity, _mask;
    bool (*equals)(const T&, const T&);
    u64 (*hash)(const T&);

    u32 log2(u32 capacity) {
        u32 ans = 0;
        while (capacity > 1) capacity = capacity >> 2, ++ ans;
        return ans;
    }

    void init(u32 size) {
        _size = 0, _capacity = size;
        data = new bucket[size];
    }

    void swap(T& a, T& b) {
        T t = a;
        a = b;
        b = t;
    }

    void free() {
        delete[] data;
    }

    void copy(const bucket* bs) {
        for (u32 i = 0; i < _capacity; ++ i) {
            data[i] = bs[i];
        }
    }

    void grow() {
        bucket* old = data;
        u32 oldsize = _capacity;
        init(_capacity * 2);
        _mask = (_mask << 1) | 1;
        for (u32 i = 0; i < oldsize; ++ i) {
            if (old[i].status == FILLED) insert(old[i].value());
        }
        delete[] old;
    }

public:
    set(bool (*equals_in)(const T&, const T&) = ::equals,
        u64 (*hash_in)(const T&) = ::hash<T>): 
        equals(equals_in), hash(hash_in) {
        init(8);
        _mask = 7;
    }

    ~set() {
        free();
    }

    set(const set& other): set(other.equals, other.hash) {
        init(other._capacity);
        _size = other._size, _mask = other._mask;
        copy(other.data);
    }

    set& operator=(const set& other) {
        if (this != &other) {
            free();
            init(other._capacity);
            hash = other.hash, equals = other.equals;
            _size = other._size, _mask = other._mask;
            copy(other.data);
        }
        return *this;
    }

    class const_iterator {
        const bucket *ptr, *end;
        friend class set;
    public:
        const_iterator(const bucket* ptr_in, const bucket* end_in): 
            ptr(ptr_in), end(end_in) {
            //
        }

        const T& operator*() const {
            return ptr->value();
        }

        const T* operator->() const {
            return &(ptr->value());
        }

        const_iterator& operator++() {
            if (ptr != end) ++ ptr;
            while (ptr != end && ptr->status != FILLED) ++ ptr;
            return *this;
        }

        const_iterator operator++(int) {
            iterator it = *this;
            operator++();
            return it;
        }

        bool operator==(const const_iterator& other) const {
            return ptr == other.ptr;
        }

        bool operator!=(const const_iterator& other) const {
            return ptr != other.ptr;
        }
    };

    class iterator {
        bucket *ptr, *end;
        friend class set;
    public:
        iterator(bucket* ptr_in, bucket* end_in): ptr(ptr_in), end(end_in) {
            //
        }

        T& operator*() {
            return ptr->value();
        }

        T* operator->() {
            return &(ptr->value());
        }

        iterator& operator++() {
            if (ptr != end) ++ ptr;
            while (ptr != end && ptr->status != FILLED) ++ ptr;
            return *this;
        }

        iterator operator++(int) {
            iterator it = *this;
            operator++();
            return it;
        }

        bool operator==(const iterator& other) const {
            return ptr == other.ptr;
        }

        bool operator!=(const iterator& other) const {
            return ptr != other.ptr;
        }

        operator const_iterator() const {
            return const_iterator(ptr, end);
        }
    };

    iterator begin() {
        bucket *start = data, *end = data + _capacity;
        while (start != end && start->status != FILLED) ++ start;
        return iterator(start, end);
    }

    const_iterator begin() const {
        const bucket *start = data, *end = data + _capacity;
        while (start != end && start->status != FILLED) ++ start;
        return const_iterator(start, end);
    }

    iterator end() {
        return iterator(data + _capacity, data + _capacity);
    }

    const_iterator end() const {
        return const_iterator(data + _capacity, data + _capacity);
    }

    void insert(const T& t) {
        if (double(_size + 1) / double(_capacity) > 0.625) grow();
        u64 h = hash(t);
        u64 dist = 0;
        u64 i = h & _mask;
        T item = t;
        while (true) {
            if (data[i].status == EMPTY || data[i].status == GHOST) {
                data[i].fill(item);
                ++ _size;
                return;
            }

            if (data[i].status == FILLED && equals(data[i].value(), item)) {
                return;
            }

            u64 other_dist = (i - (hash(data[i].value()) & _mask)) & _mask;
            if (other_dist < dist) {
                if (data[i].status == GHOST) {
                    data[i].fill(item);
                    ++ _size;
                    return;
                }
                swap(item, data[i].value());
                dist = other_dist;
            }
            i = (i + 1) & _mask;
            ++ dist;
        }
    }

    void erase(const T& t) {
        u64 h = hash(t);
        u64 i = h & _mask;
        while (true) {
            if (data[i].status == EMPTY) return;
            if (data[i].status == FILLED && equals(data[i].value(), t)) {
                data[i].evict();
                -- _size;
                return;
            }
            i = (i + 1) & _mask;
        }
    }

    const_iterator find(const T& t) const {
        u64 h = hash(t);
        u64 i = h & _mask;
        while (true) {
            if (data[i].status == EMPTY) return end();
            u64 dist = (i - h) & _mask;
            u64 oh = hash(data[i].value());
            u64 other_dist = (i - (oh & _mask)) & _mask;
            if (other_dist < dist) return end();
            if (data[i].status == FILLED && equals(data[i].value(), t)) {
                return const_iterator(data + i, data + _capacity);
            }
            i = (i + 1) & _mask;
        }
    }

    iterator find(const T& t) {
        u64 h = hash(t);
        u64 i = h & _mask;
        while (true) {
            if (data[i].status == EMPTY) return end();
            u64 dist = (i - (h & _mask)) & _mask;
            u64 oh = hash(data[i].value());
            u64 other_dist = (i - (oh & _mask)) & _mask;
            if (other_dist < dist) return end();
            if (data[i].status == FILLED && equals(data[i].value(), t)) {
                return iterator(data + i, data + _capacity);
            }
            i = (i + 1) & _mask;
        }
    }

    u32 size() const {
        return _size;
    }

    u32 capacity() const {
        return _capacity;
    }
};

template<typename K, typename V>
class map : public set<pair<K, V>> {
public:
    map(): set<pair<K, V>>(::key_equals<pair<K, V>>, ::key_hash<pair<K, V>>) {
        //
    }

    void put(const K& key, const V& value) {
        set<pair<K, V>>::insert({ key, value });
    }

    void erase(const K& key) {
        set<pair<K, V>>::erase({ key, V() });
    }

    V& operator[](const K& key) {
        pair<K, V> dummy = { key, V() };
        auto it = set<pair<K, V>>::find(dummy);
        if (it == set<pair<K, V>>::end()) {
            set<pair<K, V>>::insert(dummy);
        }
        return set<pair<K, V>>::find(dummy)->second;
    }

    const V& operator[](const K& key) const {
        pair<K, V> dummy = { key, V() };
        auto it = set<pair<K, V>>::find(dummy);
        if (it == set<pair<K, V>>::end()) {
            return it->second;
        }
        return set<pair<K, V>>::find(dummy)->second;
    }

    typename set<pair<K, V>>::const_iterator find(const K& key) const {
        return set<pair<K, V>>::find({ key, V() });
    }

    typename set<pair<K, V>>::iterator find(const K& key) {
        return set<pair<K, V>>::find({ key, V() });
    }
};

// allocates writable, executable memory
void* alloc_exec(u64 size);

// protects executable memory from being written
void protect_exec(void* exec, u64 size);

// deallocates executable memory
void free_exec(void* exec, u64 size);

#endif