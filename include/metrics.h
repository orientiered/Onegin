#ifndef METRICS_H
#define METRICS_H

/// @brief Prints nice progress bar ([###----], calculates estimated time)
void percentageBar(unsigned value, unsigned maxValue, unsigned points, long long timePassed);

/// @brief Runs sorting function many times to calculate average time
/// Return pair of double's. First value is average time in ms, second - standard deviation of it. <br>
/// Also creates file "statistics/data.txt" with all data points
doublePair_t sortTimeTest(unsigned testNumber, text_t onegin, sortFuncPtr_t sortFunc, cmpFuncPtr_t cmp);

/// @brief Plots graph with sorting time
/// Uses "statistics/data.txt" and python script with matplotlib
void plotSortTimeGraph();

#endif
