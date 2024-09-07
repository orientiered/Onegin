/// @file
/// @brief header with structs and functions to easily parse cmd args

#ifndef ARGV_PROCESSOR_H
#define ARGV_PROCESSOR_H

/// @brief Enum with available types for cmd args
enum argType {
    tBLANK = 0,     ///< Doesn't expect next argument
    tINT,           ///< Next argument is integer
    tFLOAT,         ///< Next argument is double
    tSTRING,        ///< Next argument is string(char*)
    tARRAYPTR       ///< Next argument is pointer to itself in argv
};


/// @brief Struct to describe cmd arguments
typedef struct argDescriptor {
    enum argType type;          ///< Expected type of data
    const char *argShortName;   ///< Short name (-e)
    const char *argFullName;    ///< Full name (--encode)
    const char *argHelp;        ///< Message to print in help call
} argDescriptor_t;


/// @brief Struct with argument parameters
typedef struct argVal {
    int set;                    ///< Indicates if argument is activated
    enum argType type;          ///< Data type which argument stores
    union {                     ///< Union with available types
        int _int;
        double _float;
        const char *_string;
        char **_arrayPtr;
    } val;
} argVal_t;


/// @brief Enum to easily find needed flag
enum argNamesEnum {
    INPUT,
    OUTPUT,
    SORT_ALG,
    HELP
};


/// @brief Global array with descriptions of all available arguments
const argDescriptor_t args[] {
    {tSTRING,   "-i",   "--input",    "Next argument is name of input file"},
    {tSTRING,   "-o",   "--output",   "Next argument is name of output file"},
    {tSTRING,   "-s",   "--sort",     "Next argument is name of sorting algorithm"},
    {tBLANK,    "-h",   "--help",     "Prints help message"}
};

const size_t argsSize = sizeof(args)/sizeof(argDescriptor_t);


/*!
    @brief Function to process cmd args

    @param flags [out] Array of flags, where parsed data will be written
    @param argc  [in] Number of arguments
    @param argv  [in] Array of arguments

    @return GOOD_EXIT if parsed correctly, BAD_EXIT otherwise

    Uses global constant array of arguments args and it's size argsSize
*/
enum error processArgs(argVal_t flags[], int argc, char *argv[]);


/*!
    @brief Scans argument in full form (--encode)

    @param flags [out] Array with flags
    @param remainToScan [in] Number of arguments that were'nt already scanned
    @param argv [in] Current argv position

    @return Number of arguments remained to scan. Can return int < 0, is something goes wrong

    Counts current argument as processed only after next argument was processed by scanToFlag() function
*/
int scanFullArgument(argVal_t flags[], int remainToScan, char *argv[]);


/*!
    @brief Scans argument in short form (-eio)

    @param flags [out] Array with flags
    @param remainToScan [in] Number of arguments that weren't already scanned
    @param argv [in] Current argv position

    @return Number of arguments remained to scan. Can return int < 0, is something goes wrong

    Counts current argument as processed only after all next argument were processed by scanToFlag() function

*/
int scanShortArguments(argVal_t flags[], int remainToScan, char *argv[]);


/*!
    @brief Scans value to flag

    @param flag [out] Flag to write value
    @param remainToScan Number of arguments that weren't scanned
    @param argv [in] Current argv position

    @return Number of arguments remained to scan. Can return int < 0, is something goes wrong

    Decreases remainToScan by number of elements it processed (typically 0 or 1)

    argv must point to value, that should be scanned to flag
*/
int scanToFlag(argVal_t* flag, int remainToScan, char *argv[]);


/*!
    @brief Puts zero's in every flag in given array

    @param flags [out] Array of flags

    Deactivates all flags, sets types to flags from global args array, puts zero in val field
*/
void initFlags(argVal_t flags[]);


/*!
    @brief Prints help message containing descriptions of all flags

    Prints all argHelps in args array
*/
void printHelpMessage();
#endif
