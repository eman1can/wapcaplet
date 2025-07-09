#ifndef libwapcaplet_h_
#define libwapcaplet_h_

#include <cstdint>
#include <cstddef>

namespace lwc {
enum error {
    kOk = 0,
    kOutOfMemory = 1,
    kOutOfRange = 2
};

struct string {
    int64_t len;
    uint32_t hash;
    uint32_t ref;
    string* insensitive;
    uint16_t* data;
};

string* intern(const uint16_t* s, size_t len);
string* intern_caseless(const uint16_t* s, size_t len);
string* intern_substring(const string* s, size_t offset, size_t len);
string* to_lower(const string* s);
string* ref(string* str);
bool strcmp(const string* s1, const string* s2);
bool strncmp(const string* s1, const string* s2, size_t len);
bool stricmp(string* s1, string* s2);
bool strincmp(string* s1, string* s2, size_t len);
void destroy(const string* str);
void unref(string* str);
void cleanup();
}  // namespace lwc

#endif /* libwapcaplet_h_ */
