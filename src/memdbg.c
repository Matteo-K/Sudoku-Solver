/** @file
 * @brief Memory debugger implementation
 * @author 5cover, Matteo-K
 */

#include "memdbg.h"
#include "utils.h"
#include <inttypes.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>

#ifndef NDEBUG

#undef malloc
#undef calloc
#undef free

#define STB_DS_IMPLEMENTATION
#include "stb/ds.h"

#ifdef MEMBDG_VERBOSE
#define verbose
#else
#define verbose __attribute__((unused))
#endif

typedef enum {
    AM_malloc,
    AM_calloc,
} AllocationMethod;

typedef enum {
    AS_freed,
    AS_allocated,
} AllocationStatus;

typedef struct {
    size_t size;
    char *comment;
    AllocationMethod method;
    AllocationStatus status;
} Allocation;

typedef struct {
    void *key;
    Allocation value;
} AllocationsMapItem;

static AllocationsMapItem *gs_allocations_map;

static bool gs_initialized = false;

static void memdbg_exit(void) {
    // Check that everything has been freed
    bool foundUnfreedAlloc = false;
    for (size_t i = 0; i < hmlenu(gs_allocations_map); ++i) {
        Allocation alloc = gs_allocations_map[i].value;
        if (alloc.status != AS_freed) {
            foundUnfreedAlloc = true;
            fprintf(stderr, "! Allocation %zu unfreed at exit (%s)\n", i, alloc.comment);
        }
    }

    if (foundUnfreedAlloc) {
        dbg_fail("Unfreed allocations have been found.");
    }

    // Cleanup
    for (size_t i = 0; i < hmlenu(gs_allocations_map); ++i) {
        free(gs_allocations_map[i].value.comment);
    }
    hmfree(gs_allocations_map);
}

static void signalHandler(int sigid) {
    if (sigid == SIGABRT) {
#ifdef MEMDBG_VERBOSE
        // Print allocations on abort
        dbg_print_allocations(stderr);
#endif
    }
}

void lazyInit(void) {
    if (!gs_initialized) {
        signal(SIGABRT, signalHandler);
        atexit(memdbg_exit);
    }
    gs_initialized = true;
}

void *dbg_malloc(verbose char const *file, verbose int line, size_t size) {
    lazyInit();
    void *ptr = malloc(size);
#ifdef MEMDBG_VERBOSE
    fprintf(stderr, "%s:%d: memdbg: malloc(%zu) -> %p\n", file, line, size, ptr);
#endif
    hmput(gs_allocations_map, ptr,
        ((Allocation) {
            .method = AM_malloc,
            .size = size,
            .status = AS_allocated,
            .comment = NULL,
        }));
    return ptr;
}

void *dbg_calloc(verbose char const *file, verbose int line, size_t nmemb, size_t size) {
    lazyInit();
    void *ptr = calloc(nmemb, size);
#ifdef MEMDBG_VERBOSE
    fprintf(stderr, "%s:%d: memdbg: calloc(%zu, %zu) -> %p\n", file, line, nmemb, size, ptr);
#endif
    hmput(gs_allocations_map, ptr,
        ((Allocation) {
            .method = AM_malloc,
            .size = nmemb * size,
            .status = AS_allocated,
            .comment = NULL,
        }));
    return ptr;
}

void dbg_free(char const *file, int line, void *ptr) {
    lazyInit();
    AllocationsMapItem *item = hmgetp_null(gs_allocations_map, ptr);
    if (item != NULL) {
#ifdef MEMDBG_VERBOSE
        fprintf(stderr, "%s:%d: memdbg: free: %s\n", file, line, item->value.comment);
#endif
        free(ptr);
        item->value.status = AS_freed;
    } else {
        fprintf(stderr, "%s:%d: memdbg: free(%p)\n", file, line, ptr);
        dbg_fail("Tried to free an invalid pointer: %p", ptr);
    }
}

void *check_alloc(void *mallocResult, char const *fmt_allocComment, ...) {
    lazyInit();
    va_list args;
    va_start(args, fmt_allocComment);

    if (mallocResult != NULL) {
        if (gs_initialized) {
            AllocationsMapItem *item = hmgetp_null(gs_allocations_map, mallocResult);
            if (item != NULL) {
                item->value.comment = malloc(bufferSize(fmt_allocComment, args));
                // reset the arguments after bufferSize consumed them
                va_end(args);
                va_start(args, fmt_allocComment);
                vsprintf(item->value.comment, fmt_allocComment, args);
            } else {
                fprintf(stderr, "!! Tried to check an allocation that never occured: ");
                vfprintf(stderr, fmt_allocComment, args);
                fprintf(stderr, "(%p)", mallocResult);
                assert(false);
            }
        }

        return mallocResult;
    }

    fprintf(stderr, "!! Out of memory allocating ");
    vfprintf(stderr, fmt_allocComment, args);
    putc('\n', stderr);

    va_end(args);

    perform_emergencyMemoryCleanup();

    exit(EXIT_MALLOC_FAILED);
}

void dbg_print_allocations(FILE *outStream) {
    lazyInit();
#define STR_AS_ALLOCATED "allocated"
#define STR_AS_FREED "freed"

#define STR_AM_MALLOC "malloc"
#define STR_AM_CALLOC "calloc"

#define TH_INDEX "#"
#define TH_STATUS "status"
#define TH_PTR "ptr"
#define TH_METHOD "method"
#define TH_SIZE "size (bytes)"
#define TH_COMMENT "comment"

#define COL_LEN_INDEX (digitCount(hmlenu(gs_allocations_map) - 1, 10))
#define COL_LEN_STATUS ((int)max(sizeof TH_STATUS, max(sizeof STR_AS_FREED, sizeof STR_AS_ALLOCATED)) - 1)
#define COL_LEN_PTR ((int)sizeof(void *) + 4)
#define COL_LEN_METHOD ((int)max(sizeof TH_METHOD, max(sizeof STR_AM_MALLOC, sizeof STR_AM_CALLOC)) - 1)
#define COL_LEN_SIZE ((int)sizeof TH_SIZE - 1)

    // Header
    fprintf(outStream, "%*s | %*s | %*s | %*s | %*s | %s\n",
        COL_LEN_INDEX, TH_INDEX,
        COL_LEN_STATUS, TH_STATUS,
        COL_LEN_METHOD, TH_METHOD,
        COL_LEN_SIZE, TH_SIZE,
        COL_LEN_PTR, TH_PTR,
        TH_COMMENT);

    size_t freedCount = 0, allocatedCount = 0;
    for (size_t i = 0; i < hmlenu(gs_allocations_map); ++i) {
        Allocation alloc = gs_allocations_map[i].value;

        allocatedCount += alloc.status == AS_allocated;
        freedCount += alloc.status == AS_freed;

        void *ptr = gs_allocations_map[i].key;

        fprintf(outStream, "%*zu | %*s | %*s | %*zu | %.*" PRIxPTR " | %s\n",
            COL_LEN_INDEX, i,
            COL_LEN_STATUS, alloc.status == AS_freed ? STR_AS_FREED : STR_AS_ALLOCATED,
            COL_LEN_METHOD, alloc.method == AM_malloc ? STR_AM_MALLOC : STR_AM_CALLOC,
            COL_LEN_SIZE, alloc.size,
            COL_LEN_PTR, (intptr_t)ptr,
            alloc.comment);
    }
    fprintf(outStream, "%zu allocations (%zu currently allocated, %zu freed)\n", hmlen(gs_allocations_map), allocatedCount, freedCount);
}

#endif // MEMDBG_ENABLE
