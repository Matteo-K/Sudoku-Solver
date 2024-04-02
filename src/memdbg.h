/** @file
 * @brief Memory debugger header
 * @author 5cover
 *
 * This memory debugger allow seeing which allocations have been made at any point in the program.
 * It checks if everytying has been freed on exit.
 *
 * Current allocations can be printed in a table as such:
 * status    | ptr          | method | size | comment
 * freed     | 0x0008945613 | malloc | 18   | grid cell 0,1
 * allocated | 0x0008965431 | calloc | 1024 | grid
 *
 * Ideas for more features:
 * - Make memory allocation functions (calloc, malloc) articifially return null to test error handling.
 */

#ifndef MEMDBG_H
#define MEMDBG_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __GNUC__
#define attr_malloc __attribute__((__malloc__, __malloc__(dbg_free, 3)))
#else
#define attr_malloc
#endif

/// @brief Integer: an exit code for when a memory allocation fails.
#define EXIT_MALLOC_FAILED (-1)

/// @brief Aborts the program with a debug message.
#define dbg_fail(fmt_msg, ...)                                \
    do {                                                      \
        fputs("!! ", stderr);                                 \
        fprintf(stderr, (fmt_msg)__VA_OPT__(, ) __VA_ARGS__); \
        putc('\n', stderr);                                   \
        abort();                                              \
    } while (0)

#ifdef NDEBUG

// Disable memory allocation checks.
#define check_alloc(mallocResult, ...) (mallocResult)

#else

/// @brief Prints information about the state of the heap.
/// @param outStream in: the stream to print to.
void dbg_print_allocations(FILE *outStream);

/// @brief Performs an emergency full memory cleanup.
/// @remark This function is called just before exiting in @ref exit_mallocFailed, in order to shutdown with the memory freed.
/// @remark This function must be implemented by the client considering that it may be called at any point in the program.
void perform_emergencyMemoryCleanup(void);

/// @brief Checks that an allocation succeeded and adds a comment to it for memory debugging
/// @param mallocResult in: result of the memory allocation
/// @param fmt_allocComment in: comment format string
/// @param ...: comment format arguments
/// @return @p mallocResult. Aborts if @p mallocResult is null.
void *check_alloc(void *mallocResult, char const *fmt_allocComment, ...);

/// @brief Frees memory with debugging.
/// @param file in: current file
/// @param line in: current line
/// @param ptr in: pointer to free
void dbg_free(char const *file, int line, void *ptr);

/// @brief Allocates memory using malloc with debugging.
/// @param file in: current file
/// @param line in: current line
/// @param size in: number of bytes to allocate
/// @return A pointer to the newly allocated memory region.
void *dbg_malloc(char const *file, int line, size_t size) attr_malloc;

/// @brief Allocates memory using calloc with debugging.
/// @param file in: current file
/// @param line in: current line
/// @param nmemb in: number of members
/// @param size in: member size.
/// @return A pointer to the newly allocated memory region.
void *dbg_calloc(char const *file, int line, size_t nmemb, size_t size) attr_malloc;

// Redefine stdlib functions.
#define malloc(size) dbg_malloc(__FILE__, __LINE__, size)
#define calloc(nmemb, size) dbg_calloc(__FILE__, __LINE__, nmemb, size)
#define free(size) dbg_free(__FILE__, __LINE__, size)

#endif // NDEBUG

#endif // MEMDBG_H
