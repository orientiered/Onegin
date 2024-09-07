/// @file
/// @brief Struct and macros for error handling

#ifndef ERROR_H
#define ERROR_H

#include "colors.h"

/*!
    @brief if expr is bad error, returns expr and prints debug info

    @param[in] expr Expression of function of type enum error

    If expr is BAD_EXIT of FAIL, macro will print file, function and line where expression ocurred <br>
    Then it will return expr <br>
    Prints to stderr
*/
#define PROPAGATE_ERROR(expr)                                                                                           \
        do{                                                                                                             \
            enum error res = expr;                                                                                      \
            if (res == BAD_EXIT || res == FAIL)                                                                         \
            {                                                                                                           \
                fprintf(stderr, "Error. File: %s, function: %s, line: %d\n", __FILE__, __FUNCTION__, __LINE__);         \
                return res;                                                                                            \
            }                                                                                                           \
        }while(0)


/*!
    @brief if expr is false, executes run

    @param[in] expr Any expression of integer type
    @param[in] run Set of commands to run

    If expr is false (=0), macro will print file, function and line where assertion was made <br>
    ALso prints expression <br>
    Prints to stderr <br>
    You can deactivate assert by defining NDEBUG
*/
//#define NDEBUG
#ifndef NDEBUG
#define MY_ASSERT(expr, run)                                                                                                    \
        do {                                                                                                                    \
            if (!(expr)) {                                                                                                      \
                fprintf(stderr, RED "Assertion failed:\n\t[" #expr "]\n" RESET_C);                                              \
                fprintf(stderr, RED "File: %s, function: %s, line: %d\n" RESET_C, __FILE__, __PRETTY_FUNCTION__, __LINE__);     \
                do {                                                                                                            \
                    run;                                                                                                        \
                } while (0);                                                                                                    \
            }                                                                                                                   \
        }while(0)
#else
#define MY_ASSERT(expr, run)
#endif


/*!
    @brief Macro that prints everything in stderr. Acts as printf

    Activated by defining DEBUG_PRINTS, defines DBG_PRINTF(...) macro
*/
//#define DEBUG_PRINTS
#ifdef DEBUG_PRINTS
#define DBG_PRINTF(...)                     \
    do {                                    \
        fprintf(stderr, __VA_ARGS__);       \
    } while (0)
#else
#define DBG_PRINTF(...)
#endif

/// @brief Error codes which can be used in many functions
enum error {
    GOOD_EXIT = 0,      ///< Function worked correctly and all is as expected
    BAD_EXIT,           ///< Function handled fail inside it correctly
    STRANGE_EXIT,       ///< Exit in conditions that can't be reached
    FAIL,               ///< Emergency exit
    BLANK               ///< Empty error
};

#endif
