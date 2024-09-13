#ifndef METRICS_H
#define METRICS_H

/// @brief tests sorting algorithm, calculates average time and STD, prints graph with python matplotlib
/// You need plotThread because matplotlib stops execution of program
doublePair_t sortTimeTest(unsigned testNumber, text_t onegin, sortFuncPtr_t sortFunc, cmpFuncPtr_t cmp, pthread_t *plotThread);

/// @brief Runs python script. arg could be anything
/// This function should be executed in separate thread
void *plotGraph(void *arg);

#endif
