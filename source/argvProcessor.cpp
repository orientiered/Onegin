#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "error_debug.h"
#include "argvProcessor.h"


/*!
    @brief Scans argument in full form (--encode)

    @param flags [out] Array with flags
    @param remainToScan [in] Number of arguments that were'nt already scanned
    @param argv [in] Current argv position

    @return Number of arguments remained to scan. Can return int < 0, is something goes wrong

    Counts current argument as processed only after next argument was processed by scanToFlag() function
*/
static int scanFullArgument(argVal_t flags[], int remainToScan, char *argv[]); // TODO: Why export it?

/*!
    @brief Scans argument in short form (-eio)

    @param flags [out] Array with flags
    @param remainToScan [in] Number of arguments that weren't already scanned
    @param argv [in] Current argv position

    @return Number of arguments remained to scan. Can return int < 0, is something goes wrong

    Counts current argument as processed only after all next argument were processed by scanToFlag() function

*/
static int scanShortArguments(argVal_t flags[], int remainToScan, char *argv[]);

/*!
    @brief Scans value to flag

    @param flag [out] Flag to write value
    @param remainToScan Number of arguments that weren't scanned
    @param argv [in] Current argv position

    @return Number of arguments remained to scan. Can return int < 0, is something goes wrong

    Decreases remainToScan by number of elements it processed (typically 0 or 1)

    argv must point to value, that should be scanned to flag
*/
static int scanToFlag(argVal_t* flag, int remainToScan, char *argv[]);

enum error processArgs(argVal_t flags[], int argc, char *argv[]) {
    for (int i = 1; i < argc;) {
        if (argv[i][0] != '-')  {   //all arguments start with -
            i++;                    //parameters of args are skipped inside scan...Argument() functions
            continue;
        }

        int remainToScan = 0;
        if (argv[i][1] == '-') //-abcd or --argument
            remainToScan = scanFullArgument(flags, argc-i, argv+i);
        else
            remainToScan = scanShortArguments(flags, argc-i, argv+i);
        // TODO: remainToScan? I think skipped is better.

        if (remainToScan < 0) return BAD_EXIT; //remainToScan < 0 is universal error code
        i  = argc - remainToScan; //moving to next arguments
    }
    return GOOD_EXIT;
}

static int scanFullArgument(argVal_t flags[], int remainToScan, char *argv[]) {
    for (int flagIndex = 0; flagIndex < int(argsSize); flagIndex++) {        //just iterating over all flags
        if (strcmp(argv[0], args[flagIndex].argFullName) != 0) continue;
        return scanToFlag(&flags[flagIndex], remainToScan, argv + 1) - 1;   //we pass remainToScan forward
    }                                                                   //but scanToFlag reads flag argument, so argv+1
    return -1;                                                          //-1 because we read argv flag
}

static int scanShortArguments(argVal_t flags[], int remainToScan, char *argv[]) {
    // TODO: c??
    for (char *c = argv[0]+1; (*c != '\0') && (remainToScan > 0); c++) { //iterating over short flags string
        bool scannedArg = false;
        for (int flagIndex = 0; flagIndex < int(argsSize); flagIndex++) {
            if (*c != args[flagIndex].argShortName[1]) continue;
            scannedArg = true;
            int newRemainToScan = scanToFlag(&flags[flagIndex], remainToScan, argv+1); //scanning flag param
            argv += remainToScan - newRemainToScan; //moving argv
            if (newRemainToScan < 0) return newRemainToScan; //checking for error
            remainToScan = newRemainToScan;
        }
        if (!scannedArg) return -1;
    }
    return remainToScan-1; //scanned current argv -> -1
}

int scanToFlag(argVal_t* flag, int remainToScan, char *argv[]) {
    flag->set = 1;          //activating flag // TODO: think, do you need to have redundant flags in your array?

    // TODO: can you do it like that? (just a thought)
    // const char *format = "";

    // switch(flag->type) {
    //     case tINT: format = "%d"; break;
    //     // ...
    // };

    switch(flag->type) {
    case tINT: // TODO: tINT? What is «t»
        if (--remainToScan >= 0) //checking if there is argument to scan
            sscanf(argv[0], "%d", &flag->val._int);
        break;
    case tFLOAT:
        if (--remainToScan >= 0) // TODO: duplication?
            sscanf(argv[0], "%lf", &flag->val._float);
        break;
    case tSTRING:
        if (--remainToScan >= 0)
            flag->val._string = argv[0];
        break;
    case tBLANK:
        break;
    default:
        MY_ASSERT(0, fprintf(stderr, "Logic error, unknown flag type"); abort(););
        break;
    }
    return remainToScan;
}

void initFlags(argVal_t flags[]) {          //deactivating flags and filling their argTypes
    for (size_t i = 0; i < argsSize; i++) {
        flags[i].set = 0;
        flags[i].type = args[i].type;
        flags[i].val._string = NULL;
    }
}

void printHelpMessage() {           //building help message from flags descriptions
    printf("Sort strings in file\n");
    printf("Example: ./main -io in.txt out.txt\n");
    printf("You can concatenate short version of flags\n");
    printf("Available flags:\n");
    for (size_t i = 0; i < argsSize; i++) {
        printf("%5s,%10s %s\n", args[i].argShortName, args[i].argFullName, args[i].argHelp);
    }
    printf("orientiered, MIPT 2024\n");
}
