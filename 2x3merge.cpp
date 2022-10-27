#include <cstdint>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// typedefs because were not in MUST
typedef uint64_t MustLocationId;
typedef uint64_t MustParallelId;

// MUST has this in the header
MustParallelId *pIdData;
MustLocationId *lIdData;
char *valueData;
uint64_t length;

template<typename T> void printArrays(char* value, MustParallelId* pId, MustLocationId* lId, int length) {
    T* val = (T*) value;
    printf("\n\n\nPrinting arrays:\n\n");
    printf("Some Metadata: valuePointer: %p, pIdPointer: %p, lIdPointer: %p, length: %d\n", val, pId, lId, length);

    for (int i = 0; i < length; i++) {
        printf("Value: %lu, \tpId: %lu, \tlId: %lu, \ti: %d\n", (uint64_t) val[i], pId[i], lId[i], i);
    }
    printf("\nDone.\n\n\n");
}

// TODO: value Array muss (auf erstem Level) noch vorher sortiert werden

template<typename valueType> void merge2x3arr(MustParallelId *pId, MustLocationId *lId, char *value, uint64_t count) {
    // expecting initialized pIdData, lIdData, valueData due to code before this call.

    if (length == 0) {
        printf("Special case of empty membervars\n");
        pIdData = pId;
        lIdData = lId;
        valueData = value;
        length = count;

        printArrays<valueType>(value, pId, lId, count);
        return;
    }

    // ========================================================================
    // this is basically some unnecessary abstraction, which should make the
    // code easier to read (hopefully gets optimized out anyways)
    // ========================================================================

    // local pointers to work on
    uint64_t        aPos,   bPos,   mPos;
    uint64_t        aLen,   bLen,   mLen;
    MustParallelId  *aP,    *bP,    *mP;
    MustLocationId  *aL,    *bL,    *mL;
    valueType       *aV,    *bV,    *mV;

    // a is the existing data from the header
    aPos = 0;
    aLen = length;
    aP = pIdData;
    aL = lIdData;
    aV = (valueType*) valueData;

    // b is the incoming data from the call
    bPos = 0;
    bLen = count;
    bP = pId;
    bL = lId;
    bV = (valueType*) value;

    // m is the merged result
    mPos = 0;
    mLen = length + count;
    mP = (MustParallelId*) malloc(sizeof(MustParallelId) * mLen);
    mL = (MustLocationId*) malloc(sizeof(MustLocationId) * mLen);
    mV = (valueType*) malloc(sizeof(valueType) * mLen);

    // ========================================================================
    // the actual merging
    // ========================================================================

    printf("Starting to merge\n");
    bool takeB;

    while (mPos < mLen) {
        // some code

        bool takeB = false;

        takeB = bV[bPos] < aV[aPos];            // usual evaluation
        if (aPos == aLen) { takeB = true; }     // catch case "a is completed"
        if (bPos == bLen) { takeB = false; }    // catch case "b is completed"

        // debug print works with uint64_t type only
        printf("aV: %lu   \t bV: %lu   \t aPos: %lu \t bPos: %lu \t mPos: %lu \t takeB: %d\n", aV[aPos], bV[bPos], aPos, bPos, mPos, takeB);

        if (takeB) {
            mP[mPos] = bP[bPos];
            mL[mPos] = bL[bPos];
            mV[mPos] = bV[bPos];
            bPos++;
        } else {
            mP[mPos] = aP[aPos];
            mL[mPos] = aL[aPos];
            mV[mPos] = aV[aPos];
            aPos++;
        }
        mPos++;
    }

    pIdData = mP;
    lIdData = mL;
    valueData = (char*) mV;
    length = mLen;

    printf("Done merging.\n");

    // ========================================================================
    // cleanup
    // ========================================================================

    // for some reason, these variables (that I just used) cannot be freed, as they are "invalid pointers"
    // free(aP);
    // free(bP);
    // free(aL);
    // free(bL);
    // free(aV);
    // free(bV);
}

// Params:
// - count: determines the number of elements generated for the new arrays
// - factor: the factor every value is multiplied with
// - offset: an offset for the stored value
// All arrays have equal content, to make it easier to check for correctness later
template<typename T> void test(int count, T factor, T offset) {

    T* val = (T*) malloc(count*sizeof(T));
    MustParallelId* pId = (MustParallelId*) malloc(count*sizeof(MustParallelId));
    MustLocationId* lId = (MustLocationId*) malloc(count*sizeof(MustLocationId));

    for (int i = 0; i < count; i++) {
        T content = factor * i + offset;

        val[i] = (T) content;
        pId[i] = (MustParallelId) content; // gets floored if T is e.g. float or double
        lId[i] = (MustLocationId) content; // gets floored if T is e.g. float or double
    }

    printf("These are the new arrays:\n");
    printArrays<T>((char*) val, pId, lId, count);

    printf("initialized shit, now merging\n");

    merge2x3arr<T>(pId, lId, (char*) val, count);
}

int main() {
    printf("Step 1:\n");
    test<uint64_t>(7, 3, 0);
    printArrays<uint64_t>(valueData, pIdData, lIdData, length);

    printf("Step 2:\n");
    test<uint64_t>(10, 3, 1);
    printArrays<uint64_t>(valueData, pIdData, lIdData, length);

    printf("Step 3:\n");
    test<uint64_t>(8, 3, 2);
    printArrays<uint64_t>(valueData, pIdData, lIdData, length);

    return 0;
}
