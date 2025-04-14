#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>

#define bool int
#define true 1
#define false 0

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

#define LIKELY(x)   __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

#define STATIC_ASSERT(cond, msg) typedef char static_assertion_##msg[(cond) ? 1 : -1]

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

#define CHECK_NULL(ptr) if ((ptr) == NULL) { return; }

#define ZERO_STRUCT(x) memset(&(x), 0, sizeof(x))

#ifdef DEBUG
  #define DEBUG_PRINT(fmt, ...) fprintf(stderr, "DEBUG: " fmt "\n", __VA_ARGS__)
#else
  #define DEBUG_PRINT(fmt, ...)
#endif

#define REPL_BUFF_SIZE 256
#define FILE_BUFF_SIZE 2048

#define PRINT_STRUCT_BEGIN(Name)  printf("struct '%s' {\n", #Name);
#define PRINT_STRUCT_END(Name)    printf("} // struct '%s'\n", \
        #Name);

#define PRINT_MEMBER(name, _type, fmt, cast, nested) \
                                                  \
        if (nested) { \
            /* increase indentation */ \
            /* print sub-struct */ \
            /* etc. */ \
        } else \
            printf("%s:\t" fmt ",\n", cast name);

#endif
