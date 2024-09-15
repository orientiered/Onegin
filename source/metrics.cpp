#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "error_debug.h"
#include "utils.h"
#include "onegin.h"
#include "sorters.h"
#include "metrics.h"
#include "oneginIO.h"

doublePair_t sortTimeTest(unsigned testNumber, text_t onegin, sortFuncPtr_t sortFunc, cmpFuncPtr_t cmp, pthread_t *plotThread) {
    struct stat stBuf = {};
    if (stat("statistics", &stBuf) == -1)
        system("mkdir statistics"); // TODO: mkdir -p
    FILE *graph = fopen("statistics/sortGraph.py", "wb");
    fprintf(graph,  "import matplotlib.pyplot as plt\n"
                    "data = []\n");

    clock_t startTime = 0, endTime = 0;
    clock_t totalTime = 0;
    char **textCopy = (char**) calloc(onegin.textLen, sizeof(char*));
    memcpy(textCopy, onegin.text, onegin.textLen * sizeof(char*));

    runningSTD(0, -1); //reseting runningSTD
    printf("Testing:\n");
    for (unsigned test = 0; test < testNumber; test++) {
        percentageBar(test, testNumber, 15, totalTime);
        startTime = clock();
        sortFunc(onegin.text, onegin.textLen, sizeof(char*), cmp);
        endTime = clock();
        runningSTD(endTime - startTime, 0);
        fprintf(graph, "data.append(%f)\n", double(endTime-startTime) / CLOCKS_PER_SEC * 1000);
        DBG_PRINTF(">>sortTime %3.3f\n", double(endTime - startTime) / CLOCKS_PER_SEC * 1000);
        memcpy(onegin.text, textCopy, onegin.textLen * sizeof(char*));
        totalTime += clock() - startTime;
        percentageBar(test+1, testNumber, 15, totalTime);
    }
    //returning average time in microseconds
    doublePair_t result = runningSTD(0, 1);
    result.first  *= 1000.0*1000/CLOCKS_PER_SEC;
    result.second *= 1000.0*1000/CLOCKS_PER_SEC;
    free(textCopy);

    fprintf(graph,  "plt.plot(data, 'o')\n"
                    "plt.plot([%f]*len(data))\n"
                    "plt.xlabel(\"Test number\")\n"
                    "plt.ylabel(\"Test time, ms\")\n"
                    "plt.grid()\n"
                    //"plt.gca().set_ylim(bottom=0)\n"
                    "plt.show()\n", result.first / 1000);
    fclose(graph);
    pthread_create(plotThread, NULL, plotGraph, NULL);
    printf("\n\n");
    printf("Testing took %3.2f s\n", double(totalTime) / CLOCKS_PER_SEC);
    return result;
}

void *plotGraph(void *arg) {
    system("python3 statistics/sortGraph.py");
    return NULL;
}
