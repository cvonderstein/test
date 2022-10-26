#include <cstdint>

// typedefs because were not in MUST
typedef uint64_t MustLocationId;
typedef uint64_t MustParallelId;

// MUST: in header
MustParallelId* pIdData;
MustLocationId* lIdData;
char* valueData;
uint64_t length;

template<typename T> void merge2x3arr(MustParallelId* pId, MustLocationId* lId, T* value, uint64_t count) {

}

int main() {
    return 0;
}
