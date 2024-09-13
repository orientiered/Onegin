/// @file
/// @brief Input and output functions

#ifndef ONEGIN_IO_H
#define ONEGIN_IO_H

/*!
    @brief Reads strings from file

    @param fileName [in] Name of file

    @return BAD_EXIT or FAIL if something went wrong, GOOD_EXIT otherwise
*/
enum error readTextFromFile(const char* fileName, text_t *textInfo);

/*!
    @brief Prints strings from array of strings

    @param file     [in] Stream to print strings
*/
void writeTextToFile(text_t textInfo, FILE *file);

size_t getFileSize(const char *fileName);

void splittedStringToArray(text_t *textInfo);

void splitString(text_t *textInfo);

/// @brief Prints nice progress bar ([###----], calculates estimated time)
void percentageBar(unsigned value, unsigned maxValue, unsigned points, long long timePassed);


#endif
