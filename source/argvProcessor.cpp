#include <stdio.h>
#include <string.h>
#include "error.h"
#include "argvProcessor.h"

enum error processArgs(argVal_t flags[], int argc, char *argv[]) {
    for (int i = 1; i < argc;) {
        if (argv[i][0] != '-')  {
            i++;
            continue;
        }

        int remainToScan = 0;
        if (argv[i][1] == '-')
            remainToScan = scanFullArgument(flags, argc-i, argv+i);
        else
            remainToScan = scanShortArguments(flags, argc-i, argv+i);

        if (remainToScan < 0) return BAD_EXIT;
        i  = argc - remainToScan;
    }
    return GOOD_EXIT;
}

int scanFullArgument(argVal_t flags[], int remainToScan, char *argv[]) {
    for (int flagIndex = 0; flagIndex < argsSize; flagIndex++) {
        if (strcmp(argv[0], args[flagIndex].argFullName) != 0) continue;
        return scanToFlag(&flags[flagIndex], remainToScan, argv+1)-1;
    }
    return -1;
}

int scanShortArguments(argVal_t flags[], int remainToScan, char *argv[]) {
    for (char *c = argv[0]+1; (*c != '\0') && (remainToScan > 0); c++) {
        int scannedArg = 0;
        for (int flagIndex = 0; flagIndex < argsSize; flagIndex++) {
            if (*c != args[flagIndex].argShortName[1]) continue;
            scannedArg = 1;
            int newRemainToScan = scanToFlag(&flags[flagIndex], remainToScan, argv+1);
            argv += remainToScan - newRemainToScan;
            if (newRemainToScan < 0) return newRemainToScan;
            remainToScan = newRemainToScan;
        }
        if (!scannedArg) return -1;
    }
    return remainToScan-1;
}

int scanToFlag(argVal_t* flag, int remainToScan, char *argv[]) {
    flag->set = 1;
    switch(flag->type) {
    case tINT:
        if (--remainToScan >= 0)
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

void initFlags(argVal_t flags[]) {
    for (int i = 0; i < argsSize; i++) {
        flags[i].set = 0;
        flags[i].type = args[i].type;
        flags[i].val._string = NULL;
    }
}

void printHelpMessage() {
    printf("Sort strings in file\n");
    printf("Example: ./main -io in.txt out.txt\n");
    printf("You can concatenate short version of flags\n");
    printf("Available flags:\n");
    for (int i = 0; i < argsSize; i++) {
        printf("%5s,%10s %s\n", args[i].argShortName, args[i].argFullName, args[i].argHelp);
    }
    printf("orientiered, MIPT 2024\n");
}
