#include <stdio.h>
#include <string.h>
#include "error_debug.h"
#include "argvProcessor.h"

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

        if (remainToScan < 0) return BAD_EXIT; //remainToScan < 0 is universal error code
        i  = argc - remainToScan; //moving to next arguments
    }
    return GOOD_EXIT;
}

int scanFullArgument(argVal_t flags[], int remainToScan, char *argv[]) {
    for (int flagIndex = 0; flagIndex < argsSize; flagIndex++) {        //just iterating over all flags
        if (strcmp(argv[0], args[flagIndex].argFullName) != 0) continue;
        return scanToFlag(&flags[flagIndex], remainToScan, argv+1)-1;   //we pass remainToScan forward
    }                                                                   //but scanToFlag reads flag argument, so argv+1
    return -1;                                                          //-1 because we read argv flag
}

int scanShortArguments(argVal_t flags[], int remainToScan, char *argv[]) {
    for (char *c = argv[0]+1; (*c != '\0') && (remainToScan > 0); c++) { //iterating over short flags string
        int scannedArg = 0;
        for (int flagIndex = 0; flagIndex < argsSize; flagIndex++) {
            if (*c != args[flagIndex].argShortName[1]) continue;
            scannedArg = 1;
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
    flag->set = 1;          //activating flag
    switch(flag->type) {
    case tINT:
        if (--remainToScan >= 0) //checking if there is argument to scan
            sscanf(argv[0], "%d", &flag->val._int);
        break;
    case tFLOAT:
        if (--remainToScan >= 0)
            sscanf(argv[0], "%lf", &flag->val._float);
        break;
    case tSTRING:
        if (--remainToScan >= 0)
            flag->val._string = argv[0];
        break;
    case tBLANK:
        break;
    default:
        break;
    }
    return remainToScan;
}

void initFlags(argVal_t flags[]) {          //deactivating flags and filling their argTypes
    for (int i = 0; i < argsSize; i++) {
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
    for (int i = 0; i < argsSize; i++) {
        printf("%5s,%10s %s\n", args[i].argShortName, args[i].argFullName, args[i].argHelp);
    }
    printf("orientiered, MIPT 2024\n");
}
