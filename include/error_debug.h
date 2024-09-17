/// @file
/// @brief Exit status, error handling

#ifndef ERROR_DEBUG_H
#define ERROR_DEBUG_H

#include "colors.h"

/*!
  // TODO: A better doc would be: «Propagate error»
    @brief Propagate error

    @param[in] expr Expression of type enum error

    If expr is BAD_EXIT or FAIL, macro will print file, function and line where expression ocurred <br>
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
    @brief Assert with custom behaviour

    @param[in] expr Any expression of integer type
    @param[in] ... Set of commands to run

    If expr is false (=0), macro will print file, function and line where assertion was made <br>
    ALso prints expression to stderr <br>
    Will run all commands after expression <br>
    You can deactivate assert by defining NDEBUG
*/
//#define NDEBUG

#ifndef NDEBUG
#define MY_ASSERT(expr, ...)                                                                                                    \
        do {                                                                                                                    \
            if (!(expr)) {                                                                                                      \
                fprintf(stderr, RED "Assertion failed:\n\t[" #expr "]\n" RESET_C);                                              \
                fprintf(stderr, RED "File: %s, function: %s, line: %d\n" RESET_C, __FILE__, __PRETTY_FUNCTION__, __LINE__);     \
                {                                                                                                               \
                    __VA_ARGS__;                                                                                                \
                }                                                                                                               \
            }                                                                                                                   \
        }while(0)
#else
#define MY_ASSERT(expr, run)
#endif


/*!
    @brief Printf for debug

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

#ifdef DEBUG_PRINTS
#define DBG_STR(str, size)                                      \
    do {                                                        \
        for (int i = 0;str[i] && i < size; i++) {               \
            printf("%c(%d)", str[i], (unsigned char)str[i]);    \
        }                                                       \
    } while (0)
#else
#define DBG_STR(str, size)
#endif
/// @brief Error codes which can be used in many functions
enum error { // TODO: no good errors ever, it's «status», not «error»
    GOOD_EXIT = 0,      ///< Function worked correctly and all is as expected // TODO: Why _EXIT? SUCCESS?
    BAD_EXIT,           ///< Function handled fail inside it correctly // TODO: how queer
    STRANGE_EXIT,       ///< Exit in conditions that can't be reached // TODO: I would like a thorough explanation on how that happens...
    FAIL,               ///< Emergency exit
    BLANK               ///< Empty error // TODO: empty??
};

#endif
