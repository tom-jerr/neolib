#ifndef MEMTOOLS_H
#define MEMTOOLS_H
#include <cstring>

namespace neonet {
inline void memZero(void *p, size_t n) { memset(p, 0, n); }
}; // namespace neonet

#endif // MEMTOOLS_H