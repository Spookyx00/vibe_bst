#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "safesearch.h"

int compare_ints(const void* a, const void* b) {
    int32_t arg1 = *(const int32_t*)a;
    int32_t arg2 = *(const int32_t*)b;
    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

#define MAX_FUZZ_ELEMENTS 1024

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Size < sizeof(int32_t)) {
        return 0; // Not enough data for key
    }

    /* Fixed stack buffer to avoid malloc overhead */
    static int32_t arr[MAX_FUZZ_ELEMENTS];
    
    int32_t key;
    memcpy(&key, Data, sizeof(key));
    
    Data += sizeof(key);
    Size -= sizeof(key);

    /* Cap number of elements to fit in stack buffer */
    size_t n = Size / sizeof(int32_t);
    if (n > MAX_FUZZ_ELEMENTS) {
        n = MAX_FUZZ_ELEMENTS;
    }
    
    if (n == 0) {
        // Test empty array edge case explicitly
        lower_bound_i32(NULL, 0, key);
        return 0;
    }

    memcpy(arr, Data, n * sizeof(int32_t));

    // Sort to meet contract preconditions
    qsort(arr, n, sizeof(int32_t), compare_ints);

    // 1. Test Lower Bound
    search_result_t lb = lower_bound_i32(arr, n, key);
    
    // Invariant Checks for LB
    assert(lb.status == OK_NOT_FOUND || lb.status == OK_FOUND); 
    
    assert(lb.index <= n);
    if (lb.index < n) {
        assert(arr[lb.index] >= key);
    }
    if (lb.index > 0) {
        assert(arr[lb.index - 1] < key);
    }

    // 2. Test Upper Bound
    search_result_t ub = upper_bound_i32(arr, n, key);
    
    // Invariant Checks for UB
    assert(ub.index <= n);
    if (ub.index < n) {
        assert(arr[ub.index] > key);
    }
    if (ub.index > 0) {
        assert(arr[ub.index - 1] <= key);
    }

    // 3. Test Binary Search
    search_result_t bs = binary_search_i32(arr, n, key);
    if (bs.status == OK_FOUND) {
        assert(bs.index < n);
        assert(arr[bs.index] == key);
    } else {
         assert(bs.status == OK_NOT_FOUND);
         // If not found, it must not exist
         size_t oracle_idx = lb.index; // lower_bound index is insertion point
         if (oracle_idx < n) {
             assert(arr[oracle_idx] != key);
         }
    }

    /* No free needed */
    return 0;
}
