/**
 * dbj_common.h - Common DBJ utilities
 *
 * DBJ string types and logging macros
 *
 * _GNU_SOURCE enables all GNU/POSIX extension like strnlen, localtime_r
 */
#ifndef DBJ_COMMON_H
#define DBJ_COMMON_H

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define UNUSED(x) (void)(x)

/* DBJ String type - stack allocated fixed buffer */
#define DBJ_STR(S_) struct { char data[S_]; }

/* Common string typedefs */
typedef DBJ_STR(80) str_80;
typedef DBJ_STR(256) str_256;
typedef DBJ_STR(512) str_512;

/* Generic string assign - works with any DBJ_STR type */
#define dbj_str_assign(dest_, src_) \
    dbj_str_assign_impl((dest_)->data, sizeof((dest_)->data), (src_))

static inline bool dbj_str_assign_impl(char *dest, size_t dest_size, const char *src)
{
    if (!dest || !src) return false;
    size_t max_len = dest_size - 1;
    size_t src_len = strnlen(src, max_len);
    memcpy(dest, src, src_len);
    dest[src_len] = '\0';
    return true;
}

/* ============ Logging ============ */

#define DBJ_LOG(fmt, ...) DBJ_LOG_IMPL(__LINE__, fmt, ##__VA_ARGS__)

#define DBJ_LOG_IMPL(line, fmt, ...) do { \
    str_512 buf_; \
    time_t t_ = time(NULL); \
    struct tm tm_buf_; \
    struct tm *tm_ = localtime_r(&t_, &tm_buf_); \
    const char *file_ = strrchr(__FILE__, '/'); \
    file_ = file_ ? file_ + 1 : __FILE__; \
    int n_ = snprintf(buf_.data, sizeof(buf_.data), "<%02d:%02d:%02d> | %s | #%-6d | ", \
        tm_->tm_hour, tm_->tm_min, tm_->tm_sec, \
        file_, line); \
    if (n_ > 0 && (size_t)n_ < sizeof(buf_.data)) \
        snprintf(buf_.data + n_, sizeof(buf_.data) - n_, fmt, ##__VA_ARGS__); \
    puts(buf_.data); \
} while(0)

#define DBJ_ERR(fmt, ...) do { \
    DBJ_LOG(fmt, ##__VA_ARGS__); \
    fputs("ERROR EXIT\n", stderr); \
    exit(-1); \
} while(0)

#define DBJ_WARN(fmt, ...) do { \
    DBJ_LOG(fmt, ##__VA_ARGS__); \
    fputs("WARNING\n", stderr); \
} while(0)

/* ============ Contracts ============ */

/*
 * DBJ_ASSERT - Design by Contract assertion
 *
 * Similar to standard assert() but uses DBJ_ERR on failure,
 * providing consistent logging with file/line info before exit.
 * Active in all builds (not disabled by NDEBUG).
 */
#define DBJ_ASSERT(expr_) do { \
    if (!(expr_)) { \
        DBJ_ERR("Assertion failed: %s", #expr_); \
    } \
} while(0)

/* ============ String Formatting ============ */

/*
 * DBJ_STR_FMT - Safe formatted string into DBJ_STR types
 *
 * Usage:
 *     str_256 msg;
 *     DBJ_STR_FMT(&msg, "Hello %s, count: %d", name, count);
 *     puts(msg.data);
 *
 * Contract:
 *     - Destination must be a pointer to a DBJ_STR type
 *     - Format + args must fit within the destination buffer
 *
 * Consequences:
 *     - If output would be truncated, DBJ_ASSERT fails and
 *       the program exits via DBJ_ERR with file/line info
 *     - This is intentional: silent truncation hides bugs
 *     - Size your DBJ_STR appropriately for your data
 *
 * If you need truncation-tolerant formatting, use snprintf directly:
 *     snprintf(dest->data, sizeof(dest->data), fmt, ...);
 */
#define DBJ_STR_FMT(dest_, fmt_, ...) do { \
    int ret_ = snprintf((dest_)->data, sizeof((dest_)->data), fmt_, ##__VA_ARGS__); \
    DBJ_ASSERT(ret_ >= 0 && (size_t)ret_ < sizeof((dest_)->data)); \
} while(0)

#endif /* DBJ_COMMON_H */
