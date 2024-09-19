#ifndef MAIN_H
#define MAIN_H

static flagDescriptor_t args[] = {
    {TYPE_STRING,   "-i",   "--input",      "Next argument is name of input file"},
    {TYPE_STRING,   "-o",   "--output",     "Next argument is name of output file"},
    {TYPE_BLANK,    "-t",   "--time",       "Prints average time to sort file"},
    {TYPE_BLANK,    "-g",   "--graph",      "Plots graph if flag -t is activated"},
    {TYPE_STRING,   "-s",   "--sort",       "Next argument is name of sorting algorithm\n"
                                            "Available algs: bubble, insertion, shell, qsort, cqsort"},
    {TYPE_BLANK,    "-h",   "--help",       "Prints help message"}
    };
FlagDescHolder_t flagsDescriptions = {args, ARRAY_SIZE(args)};

//these functions have flags as input parameter, so i don't want to move to other files
enum status getOutputFile(FlagsHolder_t flags, FILE **outFile);
void testSortingFunction(FlagsHolder_t flags, text_t onegin, sortFuncPtr_t sortFunc, cmpFuncPtr_t cmp);
void closeAndFreeAll(text_t *onegin, FlagsHolder_t *flags, FILE* outFile);
int checkIsSorted(text_t text, cmpFuncPtr_t cmp);

#endif
