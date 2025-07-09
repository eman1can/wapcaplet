#include <cassert>
#include <cstdlib>
#include <cstring>
#include <new>
#include <stdexcept>
#include <immintrin.h>

#include "wapcaplet/wapcaplet.h"

int32_t case_utf16ncmp(const uint16_t* s1, const uint16_t* s2, size_t n) {
    while (n-- && *s1) {
        if (*s1++ != *s2++) {
            return *s1 - *s2;
        }
    }
    return 0;
}

inline uint32_t case_calculate_hash(const uint16_t* str, size_t len) {
    uint32_t z = 0x811c9dc5;

    while (len > 0) {
        z *= 0x01000193;
        z ^= *str++;
        len--;
    }

    return z;
}

inline uint16_t to_lower(const uint16_t c) {
    if (c >= 0x41 && c <= 0x5A)
        return c + 0x20;
    return c;
}

inline uint32_t lcase_calculate_hash(const uint16_t* str, size_t len) {
    uint32_t z = 0x811c9dc5;

    while (len > 0) {
        z *= 0x01000193;
        z ^= to_lower(*str++);
        len--;
    }

    return z;
}

int lcase_utf16ncmp(const uint16_t* s1, const uint16_t* s2, size_t n) {
    while (n--) {
        if (*s1++ != to_lower(*s2++)) {
            return 1;
        }
    }
    return 0;
}

void case_memcpy(uint16_t* dest, const uint16_t* src, const size_t n) {
    memcpy(dest, src, n * sizeof(uint16_t));
}

void lcase_memcpy(uint16_t* dest, const uint16_t* src, const size_t n) {
    memcpy(dest, src, n * sizeof(uint16_t));
    for (size_t i = 0; i < n; ++i) {
        if (dest[i] >= 0x41 && dest[i] <= 0x5A) {
            dest[i] += 0x20;
        }
    }
}

namespace lwc {
constexpr uint32_t NR_BUCKETS_DEFAULT = 4091;

struct node {
    node** prev;
    node* next;
    string* val;
};

struct context {
    node** buckets;
    uint32_t bucket_count;
};

static context* ctx = nullptr;
static void initialise() {
    if (ctx == nullptr) {
        ctx = static_cast<context*>(malloc(sizeof(context)));
        if (ctx == nullptr) {
            throw std::bad_alloc();
        }

        memset(ctx, 0, sizeof(context));

        ctx->bucket_count = NR_BUCKETS_DEFAULT;
        ctx->buckets = static_cast<node**>(malloc(sizeof(node*) * ctx->bucket_count));

        if (ctx->buckets == nullptr) {
            free(ctx);
            ctx = nullptr;
            throw std::bad_alloc();
        }

        for (uint32_t i = 0; i < ctx->bucket_count; ++i) {
            ctx->buckets[i] = nullptr;
        }
    }
}

typedef uint32_t (* lwc_hasher)(const uint16_t*, size_t);
typedef int (* lwc_strncmp)(const uint16_t*, const uint16_t*, size_t);
typedef void (* lwc_memcpy)(uint16_t*, const uint16_t*, size_t);

string* _lwc_intern(const uint16_t* s, const size_t len, const lwc_hasher hasher, const lwc_strncmp compare, const lwc_memcpy copy) {
    if (s == nullptr || len == 0) {
        throw std::out_of_range("Invalid string or length");
    }

    initialise();

    const uint32_t h = hasher(s, len);
    const uint32_t bucket_ix = h % ctx->bucket_count;
    node* bucket = ctx->buckets[bucket_ix];

    while (bucket != nullptr) {
        const auto str = bucket->val;
        if (str->hash == h && str->len == len) {
            if (compare(str->data, s, len) == 0) {
                str->ref++;
                return str;
            }
        }
        bucket = bucket->next;
    }

    const size_t total = sizeof(node) + sizeof(string) + (len + 1) * sizeof(uint16_t);
    bucket = static_cast<node*>(malloc(total));
    if (bucket == nullptr) {
        throw std::bad_alloc();
    }
    bucket->prev = &ctx->buckets[bucket_ix];
    bucket->next = ctx->buckets[bucket_ix];
    if (bucket->next != nullptr) {
        bucket->next->prev = &bucket->next;
    }
    ctx->buckets[bucket_ix] = bucket;

    string* str = bucket->val = reinterpret_cast<string*>(reinterpret_cast<char*>(bucket) + sizeof(node));
    str->data = reinterpret_cast<uint16_t*>(reinterpret_cast<char*>(bucket) + sizeof(node) + sizeof(string));

    str->len = len;
    str->hash = h;
    str->ref = 1;
    str->insensitive = nullptr;

    copy(str->data, s, len);
    str->data[len] = 0;

    return str;
}

string* intern(const uint16_t* s, const size_t len) {
    return _lwc_intern(s, len, case_calculate_hash, case_utf16ncmp, case_memcpy);
}

string* intern_caseless(const uint16_t* s, const size_t len) {
    return _lwc_intern(s, len, lcase_calculate_hash, lcase_utf16ncmp, lcase_memcpy);
}

string* intern_substring(const string* s, const size_t offset, const size_t len) {
    if (s == nullptr || len == 0) {
        throw std::out_of_range("Invalid string or length");
    }
    if (offset >= s->len) {
        throw std::out_of_range("Offset >= string length");
    }
    if (offset + len > s->len) {
        throw std::out_of_range("Offset + length > string length");
    }

    return intern(s->data + offset, len);
}

string* to_lower(const string* s) {
    if (s == nullptr) {
        return nullptr;
    }

    if (s->insensitive == nullptr) {
        return intern_caseless(s->data, s->len);
    }

    return ref(s->insensitive);
}

bool strcmp(const string* s1, const string* s2) {
    return s1->len == s2->len && s1->hash == s2->hash;
}

bool strncmp(const string* s1, const string* s2, const size_t len) {
    return case_utf16ncmp(s1->data, s2->data, len) == 0;
}

bool stricmp(string* s1, string* s2) {
    if (s1->insensitive == nullptr) {
        s1->insensitive = intern_caseless(s1->data, s1->len);
    }

    if (s2->insensitive == nullptr) {
        s2->insensitive = intern_caseless(s2->data, s2->len);
    }

    return strcmp(s1->insensitive, s2->insensitive);
}

bool strincmp(string* s1, string* s2, const size_t len) {
    if (s1->insensitive == nullptr) {
        s1->insensitive = intern_caseless(s1->data, s1->len);
    }

    if (s2->insensitive == nullptr) {
        s2->insensitive = intern_caseless(s2->data, s2->len);
    }

    return strncmp(s1->insensitive, s2->insensitive, len);
}

string* ref(string* str) {
    str->ref++;
    return str;
}

void destroy(const string* str) {
    node* bucket = ctx->buckets[str->hash % ctx->bucket_count];
    *bucket->prev = bucket->next;
    if (bucket->next != nullptr) {
        bucket->next->prev = bucket->prev;
    }

    free(bucket);
}

void unref(string* str) {
    str->ref--;
    if (str->ref == 0 || str->ref == 1 && str->insensitive == str) {
        if (str->insensitive != nullptr && str->ref == 0) {
            destroy(str->insensitive);
        }
        destroy(str);
    }
}

void cleanup() {
    if (ctx != nullptr) {
        for (uint32_t i = 0; i < ctx->bucket_count; ++i) {
            node* bucket = ctx->buckets[i];
            while (bucket != nullptr) {
                node* next = bucket->next;
                free(bucket);
                bucket = next;
            }
        }
        free(ctx->buckets);
        free(ctx);
        ctx = nullptr;
    }
}
}  // namespace lwc
