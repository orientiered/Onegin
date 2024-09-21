#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "error_debug.h"
#include "utils.h"
#include "sorters.h"
#include "onegin.h"
#include "metrics.h"

void percentageBar(unsigned value, unsigned maxValue, unsigned points, long long timePassed) {
    //draw nice progress bar like
    //[###|-----] 20.0% Remaining time: 20.4 s
    printf("\r[");
    for (unsigned i = 0; i < points; i++) {
        double pointFill = double(value) / maxValue - double(i) / points;
        if (pointFill > 0)
            printf("#");
        else if (pointFill > -0.5 / points)
            printf("|");
        else
            printf("-");
    }
    printf("] %5.1f%%", double(value)/maxValue * 100);
    if (value > 0 && timePassed > 0) {
        printf(" Remaining time: %4.1f s", double(timePassed) / value * (maxValue - value) / CLOCKS_PER_SEC);
    }
    fflush(stdout);
}

doublePair_t sortTimeTest(unsigned testNumber, text_t onegin, sortFuncPtr_t sortFunc, cmpFuncPtr_t cmp) {
    const int PROGRESS_BAR_POINTS = 15;
    const int MS_PER_SEC = 1000;
    const int MCS_PER_SEC = 1000*1000;
    const int MCS_PER_MS = 1000;
    struct stat stBuf = {};
    if (stat("statistics", &stBuf) == -1)
        system("mkdir statistics");
    FILE *sortTimesFile = fopen("statistics/data.txt", "wb");

    clock_t startTime = 0, endTime = 0;
    clock_t totalTime = 0;

    restoreOriginal(&onegin);

    runningSTD(0, -1); //reseting runningSTD
    printf("Testing:\n");
    for (unsigned test = 0; test < testNumber; test++) {
        percentageBar(test, testNumber, PROGRESS_BAR_POINTS, totalTime);
        startTime = clock();
        sortFunc(onegin.lines, onegin.size, sizeof(string_t), cmp);
        endTime = clock();
        runningSTD(double(endTime - startTime), 0);
        fprintf(sortTimesFile, "%.3f\n", double(endTime-startTime) / CLOCKS_PER_SEC * MS_PER_SEC);
        DBG_PRINTF(">>sortTime %3.3f\n", double(endTime - startTime) / CLOCKS_PER_SEC * MS_PER_SEC);
        restoreOriginal(&onegin);
        totalTime += clock() - startTime;
        percentageBar(test+1, testNumber, PROGRESS_BAR_POINTS, totalTime);
    }
    fclose(sortTimesFile);
    //returning average time in microseconds
    doublePair_t result = runningSTD(0, 1);
    result.first  *= MCS_PER_SEC / CLOCKS_PER_SEC;
    result.second *= MCS_PER_SEC / CLOCKS_PER_SEC;

    printf("\n\n");
    printf("Average sorting time is %.3f+-%.3f ms\n", result.first / MCS_PER_MS, result.second / MCS_PER_MS);
    printf("Testing took %3.2f s\n", double(totalTime) / CLOCKS_PER_SEC);
    return result;
}

void plotSortTimeGraph() {
    struct stat stBuf = {};
    if (stat("statistics/data.txt", &stBuf) == -1) {
        fprintf(stderr, "There's no info to plot the graph\n");
        return;
    }
    FILE *graph = fopen("statistics/sortGraph.py", "wb");
    fprintf(graph,  "import matplotlib.pyplot as plt\n"
                    "fdata = open(\"statistics/data.txt\")\n"
                    "data = list(map(float, fdata.readlines()))\n"
                    "plt.plot(data, 'o')\n"
                    "plt.plot([sum(data)/len(data)]*len(data))\n"
                    "plt.xlabel(\"Test number\")\n"
                    "plt.ylabel(\"Test time, ms\")\n"
                    "plt.grid()\n"
                    "plt.show()\n");
    fclose(graph);

    system("python3 statistics/sortGraph.py");
}
